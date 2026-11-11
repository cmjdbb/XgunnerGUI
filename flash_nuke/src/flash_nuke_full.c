/*
 * flash_nuke_full — RP2040 整片 Flash 完全擦除固件
 *
 * 与官方 pico-examples/flash_nuke 的区别：
 *   官方版本只擦除编译时固定的 PICO_FLASH_SIZE_BYTES(默认 2MB)，
 *   Flash 容量大于 2MB 的板子上，放在芯片末尾的 EEPROM / LittleFS 分区不会被清除。
 *   本固件上电后先通过 JEDEC ID(0x9F) 读取 Flash 实际容量，再从地址 0 擦到芯片末尾；
 *   若容量字节无法识别，则按 RP2040 XIP 窗口最大值 16MB 全部擦除(超出芯片容量的地址
 *   会回绕，只是重复擦除，无副作用)。
 *
 * 运行方式：UF2 加载到 SRAM(0x20000000)，由 bootrom 直接跳转执行，不依赖 Flash 上的任何代码，
 * 全程只调用 bootrom 自带的 Flash 例程。擦除完毕后自动重新进入 BOOT(RPI-RP2) 盘。
 */
#include <stdint.h>

#define ROM_CODE(a, b) ((uint32_t)(a) | ((uint32_t)(b) << 8))

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
typedef void (*rom_void_fn)(void);
typedef void (*rom_range_erase_fn)(uint32_t addr, uint32_t count, uint32_t block_size, uint8_t block_cmd);
typedef void (*rom_usb_boot_fn)(uint32_t gpio_activity_pin_mask, uint32_t disable_interface_mask);

#define REG(addr) (*(volatile uint32_t *)(addr))
#define SSI_SR          REG(0x18000028u)
#define SSI_DR0         REG(0x18000060u)
#define SSI_SR_RFNE     (1u << 3)
#define QSPI_SS_CTRL    REG(0x4001800Cu)   /* IO_QSPI GPIO_QSPI_SS_CTRL */
#define OUTOVER_MASK    (3u << 8)
#define OUTOVER_LOW     (2u << 8)
#define OUTOVER_HIGH    (3u << 8)

/* bootrom 表格指针位于 ROM 低地址 0x14 / 0x18，gcc 会把常量低地址误报为越界，这里屏蔽 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
static void *rom_func(uint32_t code) {
    rom_table_lookup_fn lookup = (rom_table_lookup_fn)(uintptr_t)(*(volatile uint16_t *)0x18u);
    uint16_t *table = (uint16_t *)(uintptr_t)(*(volatile uint16_t *)0x14u);
    return lookup(table, code);
}
#pragma GCC diagnostic pop

static void cs_force(uint32_t outover) {
    QSPI_SS_CTRL = (QSPI_SS_CTRL & ~OUTOVER_MASK) | outover;
}

/* 单字节全双工 SPI 传输(需已处于命令模式，由 _flash_exit_xip 配置好 SSI) */
static uint8_t spi_xfer(uint8_t tx) {
    SSI_DR0 = tx;
    while (!(SSI_SR & SSI_SR_RFNE)) { }
    return (uint8_t)SSI_DR0;
}

/* 读取 JEDEC ID，返回容量字节(2^N 字节)，例如 0x15 = 2MB, 0x18 = 16MB */
static uint8_t read_capacity_byte(void) {
    while (SSI_SR & SSI_SR_RFNE) { (void)SSI_DR0; }   /* 清空残留 RX FIFO */
    cs_force(OUTOVER_LOW);
    (void)spi_xfer(0x9Fu);        /* RDID */
    (void)spi_xfer(0x00u);        /* manufacturer */
    (void)spi_xfer(0x00u);        /* memory type */
    uint8_t cap = spi_xfer(0x00u);/* capacity */
    cs_force(OUTOVER_HIGH);
    return cap;
}

__attribute__((noreturn)) void nuke_main(void) {
    rom_void_fn        connect_internal_flash = rom_func(ROM_CODE('I', 'F'));
    rom_void_fn        flash_exit_xip         = rom_func(ROM_CODE('E', 'X'));
    rom_range_erase_fn flash_range_erase      = rom_func(ROM_CODE('R', 'E'));
    rom_void_fn        flash_flush_cache      = rom_func(ROM_CODE('F', 'C'));
    rom_usb_boot_fn    reset_usb_boot         = rom_func(ROM_CODE('U', 'B'));

    connect_internal_flash();
    flash_exit_xip();

    uint8_t cap = read_capacity_byte();
    uint32_t size;
    if (cap >= 0x11u && cap <= 0x18u) {
        size = 1u << cap;             /* 128KB ~ 16MB */
    } else {
        size = 16u << 20;             /* 无法识别：按 XIP 窗口最大 16MB 全擦 */
    }

    /* 64KB 块擦除(0xD8)，非对齐部分由 bootrom 用 4KB 扇区擦除补齐 */
    flash_range_erase(0, size, 65536u, 0xD8u);
    flash_flush_cache();

    reset_usb_boot(0, 0);
    for (;;) { }
}
