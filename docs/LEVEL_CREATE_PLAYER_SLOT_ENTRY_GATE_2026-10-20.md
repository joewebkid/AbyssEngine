# Level::createPlayer slot-entry gate — 2026-10-20

The continuous original-asset Pan/GLES gate now returns from the production
`Level::createSpace`, dispatches through `Level::init` into the extracted
production `Level::createPlayer`, reads `Status::ship` and its equipment, and
executes all three real `Ship::getUsedSlots` calls. The explicit host boundary
is immediately after the slot counts and before `Player` allocation. This is
an additional continuous-path checkpoint, not completed stage 0.

Original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
shows `Status::getShip` at `0xaf1e8`, `Ship::getEquipment` at `0xaf1f0`,
three `Ship::getUsedSlots` calls at `0xaf208`, `0xaf214` and `0xaf222`, then
the 276-byte `Player` allocation at `0xaf232`. The original uses a heap
12-byte slot-counter array; current C++ uses a stack array. This gate checks
the call path and values, not that source-shape difference.

The gate uses the real original station/system and graphic assets and the
previously verified Pan fixture. It supplies an explicit, empty-equipment
`Ship` with four available positions; `Ship` construction, `Status::getShip`,
`Ship::getEquipment` and `Ship::getUsedSlots` are extracted from production
source. The fixture's `Ship::refreshValue` is deliberately bounded because
there are no installed items; it is not evidence for real equipment effects.
The boundary asserts the active Ship/equipment pointers and zero used slots.

Windows SwiftShader and an Android ARMv7 device pass at `-O0/-O2`: **18
meshes**, **22 transforms**, **44 collision volumes**, **26/26** original-file
opens/closes and six GLES texture readbacks remain checked. Native `gof2`,
ABI/lifetime smoke and **91 verifier tests** pass (one existing skip).
No production source changed. The latest full ARM report remains
**4,323/4,524** originals, **4,735 pairs**, **93.93%** mean source-shape,
**83.22%** strict fuzzy, **2,551 linked-exact** and **1,079 byte-exact**;
its SHA-256 is `79c04bc41f531c75cceb997f4e3304229011790690c5dffef864c1442e2dc50a`.
There can be no exact-pair loss from this test-only change.

Next: supply original catalog-backed Ship/equipment values and cross the
real `Player` and `PlayerEgo` allocation and weapon-position path in this
continuous gate. The separate full-method CPU contract does not prove those
services in `Level::init`; stage-0 return and a playable frame remain open.
