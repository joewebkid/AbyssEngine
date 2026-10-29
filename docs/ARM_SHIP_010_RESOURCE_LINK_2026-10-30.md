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

## LOD resource links

The same original ARM function retains the 34110 value in callee-saved `r4`
and passes it to the next two `ResourceMesh` constructors at `0x10322c–0x103230`
and `0x103268–0x10326c`. Those registrations write IDs `0x42ea` (**17130**,
LOD 1) and `0x42eb` (**17131**, LOD 2) at `0x103240–0x103252` and
`0x10327c–0x10328e`. The C++ zero placeholders for both LOD entries were
replaced with 34110. ARMv7 compiles 209/209 units; native and ARM64 static
libraries build. Focused `BuildResourceList` source-shape and strict fuzzy
remain **77.0%** and **17.7%**, nonexact. The focused output is
`D:\gof2-arm64-crypto\ship10-lods-focused-20261030.json`. The complete
corpus remains **4,323/4,524** originals across **4,745** pairs, **94.86%
source-shape / 84.43% strict fuzzy**, **2,638 linked-exact / 1,086
byte-exact**, zero skips. The exact-match sets have no losses against the
prior `ship10-full-20261030.json` report. Full output is
`D:\gof2-arm64-crypto\ship10-lods-full-20261030.json`. Five resource-table
regression tests pass. No live rendering result is claimed.

## Material lookup limitation

REA pseudocode for original ARM `PaintCanvas::MaterialCreate` at
`0x79e4c–0x79f14` confirms that it calls `FindResource(id)`, reads the
payload at resource offset `+12` as eight texture IDs, and creates a
`Material` without checking the resource kind. `PaintCanvas::MeshCreate`
at `0x79d5c–0x79e2e` invokes it for the mesh payload's 16-bit field at
offset `+4` only on the uncached mesh path. The original resource-list
instructions at `0xee454–0xee45a` register ID 34110 with kind 2, a
`ResourceTexture`, whereas the current source also has no separate
kind-6 registration for that ID. This is a genuine ambiguity in the
original lookup path: the ARM arguments are proven, but a fresh
`MaterialCreate(34110)` would interpret a texture payload as material
fields. Whether the player ship instead reuses a previously created mesh
or takes another path has not been established. Do not add a type guard or
claim that the `PlayerEgo::setShip` crash is resolved on this evidence.

The available physical ARM phone has the original package, but `run-as`
reports it is not debuggable and production `adbd` refuses root; its screen
was asleep during this check. A targeted trace therefore requires a
separate instrumented build or another supported debugging route. The
Android 9 x86/ARM-bridge emulator reached an opening scene previously,
but a later run crashed in `libndk_translation` before the intended trace.
