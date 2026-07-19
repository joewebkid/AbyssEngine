#!/usr/bin/env python3
"""elf.py -- minimal ELF32 reader shared by the verify pipeline.

Just enough to: enumerate .dynsym function symbols with their (value, size, thumb-bit), and map a
virtual address to the file bytes that back it. Pure Python, little-endian ELF32 (armeabi-v7a).
"""
import struct
from pathlib import Path

SHT_SYMTAB = 2
SHT_DYNSYM = 11
STT_FUNC = 2
STB_LOCAL = 0
SHN_UNDEF = 0


class Elf32:
    def __init__(self, path):
        self.path = str(path)
        self.data = Path(path).read_bytes()
        d = self.data
        if d[:4] != b"\x7fELF" or d[4] != 1 or d[5] != 1:
            raise ValueError(f"{path}: not a little-endian ELF32 file")
        e_shoff = struct.unpack_from("<I", d, 0x20)[0]
        e_shentsize = struct.unpack_from("<H", d, 0x2E)[0]
        e_shnum = struct.unpack_from("<H", d, 0x30)[0]
        self.sections = []
        for i in range(e_shnum):
            off = e_shoff + i * e_shentsize
            sh_type, = struct.unpack_from("<I", d, off + 0x04)
            sh_addr, sh_offset, sh_size = struct.unpack_from("<III", d, off + 0x0C)
            sh_link, = struct.unpack_from("<I", d, off + 0x18)
            sh_entsize, = struct.unpack_from("<I", d, off + 0x24)
            self.sections.append(dict(type=sh_type, addr=sh_addr, offset=sh_offset,
                                      size=sh_size, link=sh_link, entsize=sh_entsize))

    def _symbols(self, sh_type):
        sec = next((s for s in self.sections if s["type"] == sh_type), None)
        if sec is None:
            return
        entsize = sec["entsize"] or 16
        strtab = self.sections[sec["link"]]
        sbase, ssize = strtab["offset"], strtab["size"]
        d = self.data

        def cstr(idx):
            end = d.find(b"\x00", sbase + idx)
            return d[sbase + idx:end].decode("utf-8", "replace")

        for off in range(sec["offset"], sec["offset"] + sec["size"], entsize):
            st_name, st_value, st_size = struct.unpack_from("<III", d, off)
            st_info = d[off + 12]
            st_shndx, = struct.unpack_from("<H", d, off + 14)
            yield dict(name=cstr(st_name), value=st_value, size=st_size,
                       info=st_info, shndx=st_shndx)

    def functions(self):
        """name -> dict(value, size, thumb) for every defined, *exported* .dynsym function symbol.

        Only .dynsym non-local entries, so the set matches what the stripped original exposes (the
        original has no .symtab); our build's local symbols would otherwise inflate the diff. The ARM
        Thumb bit lives in st_value's LSB; the real entry address clears it.
        """
        out = {}
        for s in self._symbols(SHT_DYNSYM):
            if not s["name"] or s["shndx"] == SHN_UNDEF or s["size"] == 0:
                continue
            if (s["info"] & 0xF) != STT_FUNC:
                continue
            if (s["info"] >> 4) == STB_LOCAL:
                continue
            out[s["name"]] = dict(value=s["value"] & ~1, size=s["size"], thumb=s["value"] & 1)
        return out

    def bytes_at(self, vaddr, size):
        """Raw bytes backing [vaddr, vaddr+size) via the containing section's file mapping."""
        for s in self.sections:
            if s["addr"] and s["addr"] <= vaddr < s["addr"] + s["size"]:
                start = s["offset"] + (vaddr - s["addr"])
                return self.data[start:start + size]
        return b""
