# Whole-`.so` relink — requirements toward a byte-identical libgof2hdaa.so

Per-function disassembly matching (see `build/README.md`) is the unit of progress. Producing a
byte-identical **`libgof2hdaa.so`** is the final assembly step. Parameters extracted from the
target ELF (NDK r18b, armeabi-v7a):

## Link inputs (from the target)
- **Type/arch**: `ET_DYN`, `EM_ARM`, 32-bit LSB. SONAME `libgof2hdaa.so`.
- **NEEDED** (this order): `liblog.so, libGLESv2.so, libGLESv1_CM.so, libEGL.so, libandroid.so,
  libfmodex.so, libfmodevent.so, libm.so, libdl.so, libc.so`.
- **Sections**: `.text .rodata .data.rel.ro .init_array .data .bss` (+ `.note.gnu.build-id`,
  dynamic/reloc/symtab). 20-byte GNU build-id present.
- C++ runtime: NDK r18 **libc++** (libc++abi); static libgcc/compiler-rt builtins.

## `make link` (scaffold, in this repo)
Links the recovered objects with the right soname/NEEDED/build-id via NDK `lld`. Currently uses
`--allow-shlib-undefined -nostdlib` because the Android **platform stub libraries**
(`$NDK/platforms/android-<api>/arch-arm/usr/lib/*.so`) and the libc++ runtime aren't extracted in
this environment — extract them (or the full NDK) to resolve symbols for a real link.

## To reach byte-identical
1. **Coverage**: every `.text` function recovered and matched (the long grind).
2. **Link order**: object/section order must match the original (driven by the original Makefile/
   ndk-build module order). Derive from the address order of functions in the target.
3. **Compiler/STL**: build all TUs with the verified flags (`-target armv7-none-linux-androideabi16
   -march=armv7-a -mthumb -Oz -fpic -frtti`, libc++), matching per-TU options where they differ.
   (`-Oz`, not `-O2` — see the toolchain-archaeology finding in `DECOMP_NOTES.md`.)
4. **Linker**: the target was linked with **GNU gold 1.12** (`.note.gnu.gold-version`), *not* lld.
   `make link` currently uses NDK `lld` for the scaffold, which differs from gold in section/segment
   layout, padding, and emits no `.note.gnu.gold-version` — so a **byte-identical** relink needs the
   original `gold` (NDK r18 ships `arm-linux-androideabi-ld.gold`) with the original version script
   (export list) + `--build-id`. The build-id is content-derived, so it falls out once the rest matches.
5. **Data sections**: `.rodata/.data` must match — these come from the recovered source's literals/
   globals + the asset/string tables; diff them too.
6. **Verify**: `cmp`/section-diff the produced `.so` against the target; iterate.

## Other targets (later)
- **iOS** `armv7` Mach-O (1.1.4 HD): Xcode/iOS SDK 6.0-era Apple llvm-gcc/clang — harder to source;
  Mach-O segment/section + code-signing differences.
- **Windows** `GOF2.exe` x86: MSVC (vc10) — content-cut build; reference only.
