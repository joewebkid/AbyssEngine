# Ship 010 mesh resource link (2026-10-30)

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
registers mesh resource **17010** (`ship_010_terran.aem`) with constructor
argument **34110**. In `BuildResourceList`, `0xec392` loads `r8 = 0x84fa`
(34042); `0xee454` computes `r6 = r8 + 0x44` (34110); `0xee46e` saves that
value at `[sp, #0x1ac]`; and `0x1031ec–0x1031f6` loads it, narrows it to
16 bits, and passes it as the second argument to `ResourceMesh` immediately
before the registration of ID `0x4272` (17010) at `0x103204–0x103216`.
The previous C++ entry supplied zero. The entry now supplies 34110.

This is source-backed registration data, not a proven startup fix. The
current source also registers 34110 as a kind-2 texture resource. The
original `PaintCanvas::MeshCreate` passes the mesh's second field to
`MaterialCreate`, which lacks a resource-kind guard. The actual lookup
chain and resulting material pointer at first player-ship creation have
not been observed dynamically. `PlayerEgo::setShip` itself is unchanged.

ARMv7 compiles 209/209 translation units. Native `gof2` builds. Focused
`BuildResourceList` comparison: source-shape **77.0% before and after**;
strict fuzzy **18.1% to 17.7%**; no linked-exact or byte-exact match for this
function either way. The complete ARM corpus compares **4,323/4,524**
original functions over **4,745** pairs: **94.86% source-shape**, **84.43%
strict fuzzy**, **2,638 linked-exact**, **1,086 byte-exact**, zero skips.
Comparison against `id_zero_material_full_20261028.json` found no lost
linked-exact or byte-exact pairs. Full output is
`D:\gof2-arm64-crypto\ship10-full-20261030.json`.

Next, capture the original `MeshCreate`/`MaterialCreate` arguments and
material pointer when resource 17010 is loaded, then compare with the
rebuilt ARM64 run before treating the null dereference as resolved.
