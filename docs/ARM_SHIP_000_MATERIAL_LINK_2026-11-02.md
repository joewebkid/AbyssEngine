# Ship 000 material links (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `BuildResourceList` creates a kind-6 material resource with
ID **34200**: `0xf171e` loads `0x84fa` (34042), `0xf176a` adds `0x9e`,
and `0xf1762`–`0xf1772` write the resulting ID and kind into the
resource record. That same value is stored at `[sp, #0x1e0]` at
`0xf18b6`, reloaded at `0x102618`, narrowed to 16 bits and passed as
the second argument of `ResourceMesh` at `0x102622`. The immediately
following registration (`0x10262c`) assigns mesh ID **17000**.

The material ID remains in `r4` for the next two `ResourceMesh`
constructor calls (`0x102656`–`0x10265a` and
`0x102690`–`0x102694`). Their registrations assign IDs **17100** and
**17101**. These are the two LOD meshes for `ship_000_midorian.aem`.
The C++ registrations had zero for all three material links; they now
use the instruction-backed ID **34200**. The existing resource table
defines ID 34200 as a material using the ship's diffuse and
normal/specular textures.

This closes one proven gap in the path from `getShipGroup(0, 0, true)`
to `PlayerEgo::setShip(0, 0)`: the mesh loader can receive a material
resource rather than an absent ID. It does not establish that the
previous ARM64 null material dereference is resolved, because the
current APK has not been observed after this change. Material loading,
mesh assignment and the rendered frame still need a live trace.

Windows native, ARM64 static and strict ARM64 shared builds pass. The
source-extracted full resource table smoke passes at `-O0` and `-O2`;
five material-table tests and two ARM ship-table tests pass. ARMv7
compiles 209/209 units. Focused `BuildResourceList` source-shape stays
**77.0%**, while strict fuzzy improves **20.2% → 23.9%**; generated
instructions rise from 51,415 to 51,461 versus 60,817 in the
original. The function remains nonexact. Focused report:
`D:/gof2-arm64-crypto/ship000-material-focused-20261102.json`.

A signed ARM64 test APK is
`D:/gof2-arm64-crypto/gof2-arm64-ship000-redmi-debug-20261102.apk`,
SHA-256 `b2c78bd64997afd5da10d615b90d70386b5c2ea2e3b04248487b060d4303782c`.
It has the 52 expected Java JNI exports, and v1/v2/v3 signatures
verify. It was copied to the Redmi as
`/sdcard/Download/gof2-arm64-current-test.apk`; manual installation
of the separate test package is pending because Play Protect blocks
the ADB install path.

The full ARM corpus compares **4,323/4,524** originals across **4,762**
pairs at **96.20%** mean source-shape and **85.96%** strict fuzzy,
with **2,741 linked-exact / 1,096 byte-exact**, zero skipped and 201
missing originals. Normalized pair and both exact-match sets are
identical to the preceding `fmod-progress-20261102-full.json` report:
zero lost pairs or exact matches. Full report:
`D:/gof2-arm64-crypto/ship000-material-full-20261102.json`.

Next: observe this APK's material pointer and game frame on the Redmi.
For the following offline resource batch, recover the still-zero
material links of ships 001–009 from their own ARM constructor calls.
