# 编译 flash_nuke_full 并生成 ..\flash_nuke.uf2
# 依赖: Arduino IDE 已安装 arduino-pico 核心(自带 arm-none-eabi-gcc)、python3
$ErrorActionPreference = "Stop"
$src = $PSScriptRoot
$out = Join-Path $env:TEMP "flash_nuke_full_build"
New-Item -ItemType Directory -Force $out | Out-Null

$gccDir = Get-ChildItem "$env:LOCALAPPDATA\Arduino15\packages\rp2040\tools\pqt-gcc" -Directory |
    Sort-Object Name -Descending | Select-Object -First 1
if (-not $gccDir) { throw "未找到 arduino-pico 工具链 (pqt-gcc)" }
$bin = Join-Path $gccDir.FullName "bin"
$gcc = Join-Path $bin "arm-none-eabi-gcc.exe"
$objcopy = Join-Path $bin "arm-none-eabi-objcopy.exe"
$size = Join-Path $bin "arm-none-eabi-size.exe"

$cflags = @("-mcpu=cortex-m0plus", "-mthumb", "-Os", "-ffreestanding", "-fno-builtin",
            "-nostdlib", "-nostartfiles", "-Wall", "-Wextra",
            "-T", "$src\ram.ld", "-Wl,--gc-sections")

& $gcc @cflags -o "$out\flash_nuke_full.elf" "$src\start.S" "$src\flash_nuke_full.c"
& $size "$out\flash_nuke_full.elf"
& $objcopy -O binary "$out\flash_nuke_full.elf" "$out\flash_nuke_full.bin"
python "$src\make_uf2.py" "$out\flash_nuke_full.bin" "$src\..\flash_nuke.uf2"
