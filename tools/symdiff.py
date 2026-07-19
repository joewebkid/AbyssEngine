#!/usr/bin/env python3
"""symdiff.py - dynamic-symbol parity diff between two ELF shared objects.

The headline acceptance gate: our relinked libgof2hdaa.so must export exactly the
same defined dynamic symbols as the original (no extra, no missing). This reads
.dynsym directly (pure Python, no ARM toolchain needed) so it works on macOS.

Usage:
    python3 tools/symdiff.py [OURS.so]
        OURS.so defaults to cmake-build-match/libgof2hdaa.so
        original defaults to _work/bins/android_2.0.16_libgof2hdaa.so
    python3 tools/symdiff.py --list ORIG.so      # just dump defined dynsyms

Exit code 0 iff 0 extra and 0 missing.
"""
import os, sys, struct

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ORIG = os.path.normpath(os.path.join(ROOT, "..", "_work", "bins", "android_2.0.16_libgof2hdaa.so"))
OURS_DEFAULT = os.path.join(ROOT, "cmake-build-match", "libgof2hdaa.so")

STB_LOCAL = 0


def read_defined_dynsyms(path):
    """Return set of names of DEFINED (st_shndx != SHN_UNDEF), non-local dynamic
    symbols in a 32-bit little-endian ELF. Handles the typical Android arm32 .so."""
    with open(path, "rb") as f:
        data = f.read()
    if data[:4] != b"\x7fELF":
        raise ValueError(f"{path}: not ELF")
    ei_class = data[4]   # 1 = 32-bit
    ei_data = data[5]    # 1 = LE
    if ei_class != 1 or ei_data != 1:
        raise ValueError(f"{path}: expected 32-bit LE ELF (class={ei_class} data={ei_data})")
    end = "<"
    # ELF32 header
    (e_shoff,) = struct.unpack_from(end + "I", data, 0x20)
    (e_shentsize,) = struct.unpack_from(end + "H", data, 0x2e)
    (e_shnum,) = struct.unpack_from(end + "H", data, 0x30)
    (e_shstrndx,) = struct.unpack_from(end + "H", data, 0x32)

    sections = []  # (name_off, type, offset, size, link, entsize)
    for i in range(e_shnum):
        base = e_shoff + i * e_shentsize
        name, stype, flags, addr, off, size, link, info, align, entsize = \
            struct.unpack_from(end + "IIIIIIIIII", data, base)
        sections.append((name, stype, off, size, link, entsize))

    def sec_str(strtab_off, idx):
        e = data.index(b"\x00", strtab_off + idx)
        return data[strtab_off + idx:e].decode("utf-8", "replace")

    # find .dynsym (SHT_DYNSYM = 11) and its linked strtab
    dynsym = next((s for s in sections if s[1] == 11), None)
    if not dynsym:
        raise ValueError(f"{path}: no .dynsym")
    _, _, sym_off, sym_size, sym_link, sym_entsize = dynsym
    str_off = sections[sym_link][2]
    entsize = sym_entsize or 16

    names = set()
    n = sym_size // entsize
    for i in range(n):
        b = sym_off + i * entsize
        st_name, st_value, st_size, st_info, st_other, st_shndx = \
            struct.unpack_from(end + "IIIBBH", data, b)
        if st_shndx == 0:        # SHN_UNDEF -> imported, not defined here
            continue
        bind = st_info >> 4
        if bind == STB_LOCAL:    # locals aren't part of the exported ABI
            continue
        nm = sec_str(str_off, st_name)
        if nm:
            names.add(nm)
    return names


def main():
    args = sys.argv[1:]
    if args and args[0] == "--list":
        for n in sorted(read_defined_dynsyms(args[1])):
            print(n)
        return 0
    ours = args[0] if args else OURS_DEFAULT
    orig = read_defined_dynsyms(ORIG)
    if not os.path.exists(ours):
        print(f"[symdiff] original defines {len(orig)} dynamic symbols")
        print(f"[symdiff] ours not found: {ours} (relink not produced yet)")
        return 1
    mine = read_defined_dynsyms(ours)
    extra = sorted(mine - orig)
    missing = sorted(orig - mine)
    print(f"original: {len(orig)}  ours: {len(mine)}  extra: {len(extra)}  missing: {len(missing)}")
    if extra:
        print("\n-- EXTRA (in ours, not original) --")
        for n in extra[:60]:
            print("  +", n)
        if len(extra) > 60:
            print(f"  ... {len(extra) - 60} more")
    if missing:
        print("\n-- MISSING (in original, not ours) --")
        for n in missing[:60]:
            print("  -", n)
        if len(missing) > 60:
            print(f"  ... {len(missing) - 60} more")
    return 0 if not extra and not missing else 1


if __name__ == "__main__":
    sys.exit(main())
