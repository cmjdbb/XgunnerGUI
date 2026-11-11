#!/usr/bin/env python3
"""将裸 bin 打包成 RP2040 UF2(加载到 SRAM 0x20000000)。用法: make_uf2.py in.bin out.uf2"""
import struct
import sys

UF2_MAGIC0 = 0x0A324655
UF2_MAGIC1 = 0x9E5D5157
UF2_MAGIC_END = 0x0AB16F30
UF2_FLAG_FAMILY_ID = 0x00002000
RP2040_FAMILY_ID = 0xE48BFF56
BASE_ADDR = 0x20000000
BLOCK_PAYLOAD = 256


def main(src: str, dst: str) -> None:
    data = open(src, "rb").read()
    if len(data) % BLOCK_PAYLOAD:
        data += b"\0" * (BLOCK_PAYLOAD - len(data) % BLOCK_PAYLOAD)
    num_blocks = len(data) // BLOCK_PAYLOAD
    out = bytearray()
    for i in range(num_blocks):
        payload = data[i * BLOCK_PAYLOAD:(i + 1) * BLOCK_PAYLOAD]
        out += struct.pack(
            "<8I", UF2_MAGIC0, UF2_MAGIC1, UF2_FLAG_FAMILY_ID,
            BASE_ADDR + i * BLOCK_PAYLOAD, BLOCK_PAYLOAD, i, num_blocks, RP2040_FAMILY_ID,
        )
        out += payload + b"\0" * (476 - BLOCK_PAYLOAD)
        out += struct.pack("<I", UF2_MAGIC_END)
    open(dst, "wb").write(out)
    print(f"{dst}: {len(data)} bytes payload, {num_blocks} block(s), {len(out)} bytes")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
