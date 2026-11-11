/* RAM 镜像入口：bootrom 在 UF2 全部为 SRAM 地址时，直接跳转到 0x20000000 执行 */
    .syntax unified
    .cpu cortex-m0plus
    .thumb

    .section .entry, "ax"
    .global _entry
    .thumb_func
_entry:
    ldr   r0, =0x20042000      /* SRAM 顶端作为栈 */
    msr   msp, r0
    cpsid i
    bl    nuke_main
1:  b     1b
    .pool
