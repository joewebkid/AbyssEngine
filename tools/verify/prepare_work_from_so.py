#!/usr/bin/env python3
"""Prepare local verifier inputs from an original Android libgof2hdaa.so.

The historical verifier expected these files under _work:
  _work/bins/android_2.0.16_libgof2hdaa.so
  _work/symbols/android_2.0.16.symbols.tsv
  _work/symbols/android_thumb_map.tsv

This script builds the two symbol TSVs directly from the ELF dynsym table. ARM
Thumb functions carry bit 0 in st_value; delink.py needs even code addresses in
symbols.tsv and a name keyed thumb map.
"""

from __future__ import annotations

import argparse
import shutil
import struct
from pathlib import Path


def cstr(data: bytes, off: int) -> str:
    end = data.index(b"\0", off)
    return data[off:end].decode("utf-8", errors="replace")


def section_headers(data: bytes):
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise ValueError("expected little-endian ELF32")
    e_shoff = struct.unpack_from("<I", data, 0x20)[0]
    e_shentsize = struct.unpack_from("<H", data, 0x2E)[0]
    e_shnum = struct.unpack_from("<H", data, 0x30)[0]
    e_shstrndx = struct.unpack_from("<H", data, 0x32)[0]
    shstr = e_shoff + e_shstrndx * e_shentsize
    shstr_off = struct.unpack_from("<I", data, shstr + 16)[0]

    out = []
    for i in range(e_shnum):
        base = e_shoff + i * e_shentsize
        name_off, sh_type, sh_flags, sh_addr, sh_offset, sh_size, sh_link, sh_info, sh_addralign, sh_entsize = (
            struct.unpack_from("<IIIIIIIIII", data, base)
        )
        out.append(
            {
                "index": i,
                "name": cstr(data, shstr_off + name_off),
                "type": sh_type,
                "addr": sh_addr,
                "offset": sh_offset,
                "size": sh_size,
                "link": sh_link,
                "entsize": sh_entsize,
            }
        )
    return out


def dynsym_functions(data: bytes):
    sections = section_headers(data)
    dynsym = next(s for s in sections if s["name"] == ".dynsym")
    dynstr = sections[dynsym["link"]]
    dynstr_data = data[dynstr["offset"]: dynstr["offset"] + dynstr["size"]]
    entsize = dynsym["entsize"] or 16
    count = dynsym["size"] // entsize
    funcs = []
    for i in range(count):
        base = dynsym["offset"] + i * entsize
        st_name, st_value, st_size, st_info, st_other, st_shndx = struct.unpack_from("<IIIBBH", data, base)
        st_type = st_info & 0x0F
        if st_name == 0 or st_value == 0 or st_type != 2:
            continue
        name = cstr(dynstr_data, st_name)
        if not name:
            continue
        thumb = 1 if (st_value & 1) else 0
        addr = st_value & ~1
        funcs.append((addr, thumb, name))
    funcs.sort(key=lambda item: (item[0], item[2]))
    return funcs


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--so", required=True, type=Path)
    parser.add_argument("--work", default=Path("_work"), type=Path)
    args = parser.parse_args()

    so = args.so.resolve()
    work = args.work.resolve()
    bins = work / "bins"
    symbols = work / "symbols"
    bins.mkdir(parents=True, exist_ok=True)
    symbols.mkdir(parents=True, exist_ok=True)

    target_so = bins / "android_2.0.16_libgof2hdaa.so"
    shutil.copyfile(so, target_so)

    data = so.read_bytes()
    funcs = dynsym_functions(data)

    sym_path = symbols / "android_2.0.16.symbols.tsv"
    thumb_path = symbols / "android_thumb_map.tsv"
    with sym_path.open("w", encoding="utf-8", newline="\n") as f:
        for addr, _, name in funcs:
            f.write(f"{addr:08x}\t{name}\n")
    with thumb_path.open("w", encoding="utf-8", newline="\n") as f:
        for addr, thumb, name in funcs:
            f.write(f"{addr:08x}\t{thumb}\t{name}\n")

    print(f"copied {target_so}")
    print(f"wrote {sym_path} ({len(funcs)} functions)")
    print(f"wrote {thumb_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
