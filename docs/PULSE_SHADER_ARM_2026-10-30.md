# PulseShader ARM recovery (2026-10-30)

Primary evidence: original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PulseShader::Init` starts at ELF `0x92424`. Its first two PC-relative
arguments are embedded GLSL; the old source passed placeholder
filenames.

| Source | ELF address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| Vertex | `0x1fa650` | 343 | `a519c4a2d04681002f7d60d3da1e22176727421e35df07f880f3bc98c210eceb` |
| Fragment | `0x1fa7a8` | 283 | `e017c8e01069f979abd6b28e7a3a13f6e60c11c5df5c50447055e6f72c5662d6` |

Both C++ GLSL arrays were decoded and compared byte-for-byte against
those ELF ranges. All 15 queried input names and their order match the
original `Init` PC-relative literals. `Init` was already linked-exact
when those referenced bytes were wrong; its code metric alone did not
verify the shader content.

The original `UpdateMeshData` at ELF `0x92594` loads the application
manager from the passed `Engine` at offset `0x30` before calling
`GetCurrentTimeMillis`. The old source used the process-global
`ApplicationManager::gAppManager`. The original `vldr` at `0x92636`
reads a float literal at `0x927c4`, whose raw bytes `00 00 7a 44` are
`1000.0f`. The old source divided by `0.0f`; the new source uses
`1000.0f`. The ARMv7 object and original ELF contain those same four
literal bytes.

Native `gof2` and ARM64 static builds pass. ARMv7 compiles **209/209**
translation units. Focused `UpdateMeshData` rises from **99.1% to
100% source-shape** and from **98.6% to 100% strict fuzzy**. All six
functions have **100% source-shape and strict fuzzy**, but only **5/6
are reported linked-exact**. The verifier's disassembly parser prints
the identical raw literal pool bytes as two Thumb halfwords in the
delinked original and one word in our object; `linked_equal` therefore
sees four apparent unmasked byte differences. This is a measurement
limit, not evidence of a different `1000.0f` value. The metric was not
adjusted. Focused report:
`D:\gof2-arm64-crypto\pulse-shader-focused-20261030.json`.

The strict ARM64 shared library links without undefined symbols and
retains all **53 JNI exports**. A signed, zip-aligned parallel-install
APK at
`D:\gof2-arm64-crypto\gof2-arm64-pulse-shader-poco-clone-debug-20261030.apk`
contains that library byte-for-byte. It has not been installed or run
on Poco X3; runtime shader compilation, rendering, and gameplay remain
unverified.

The full ARM corpus compares **4,323/4,524** originals across **4,745**
pairs: **94.87%** mean source-shape, **84.46%** strict fuzzy,
**2,650 linked-exact**, **1,086 byte-exact**, zero skipped, and 201
missing originals. Compared with
`D:\gof2-arm64-crypto\color-mix-full-20261030.json`, the complete
`(unit, symbol)` sets have zero linked-exact and byte-exact gains or
losses. Full report:
`D:\gof2-arm64-crypto\pulse-shader-full-20261030.json`.
