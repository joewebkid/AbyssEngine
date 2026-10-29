# Ship 011 material links (2026-10-30)

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
passes **34211** to the `ResourceMesh` constructor for ship 011 and its
two LOD meshes. `BuildResourceList` forms `r11 = 0x84fa` (34042) at
`0xf19cc`, then `r9 = r11 + 0xa9` (34211) at `0xf1a00` and stores that
value at `[sp, #0x1a4]` at `0xf1b10`. At `0x10331c–0x103326`, the
value is loaded, narrowed to 16 bits, and passed for resource `0x4273`
(**17011**) registered at `0x103334–0x103346`. The retained `r4` is
passed for resources `0x42ed` (**17133**, LOD 1) and `0x42ee`
(**17134**, LOD 2) at `0x10335c–0x103360` and `0x103396–0x10339a`.

The C++ resource list previously used zero for all three links. It now
uses 34211, which is registered as a kind-6 `ResourceMaterial` at slot
645 with texture IDs 34011 and 34111. It is the only registration of
ID 34211 in the 2,488-entry C++ table. This is an instruction-backed
resource correction; it does not prove a live rendering result.

ARMv7 compiles 209/209 translation units. Native and ARM64 static
libraries build. Focused `BuildResourceList` source-shape stays at
**77.0%** and strict fuzzy rises **17.7% → 17.8%**; the function remains
nonexact. The full ARM corpus compares **4,323/4,524** originals over
**4,745** pairs: **94.86% source-shape**, **84.43% strict fuzzy**,
**2,638 linked-exact**, **1,086 byte-exact**, zero skips. The linked- and
byte-exact sets have no lost pair relative to the preceding
`ship10-lods-full-20261030.json`. Full output is
`D:\gof2-arm64-crypto\ship11-full-20261030.json`. Five resource-table
regression tests pass.
