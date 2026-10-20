# Level::createPlayer Player-allocation gate — 2026-10-20

The continuous original-asset Pan/GLES gate now executes the extracted
production `Level::createPlayer` through allocation and construction of a
real `Player`, followed by its production shield/armor setters and the
`field_69` initialization. The explicit test boundary is immediately before
`PlayerEgo` allocation; `Level::init` stage 0 has not returned.

Original Android 2.0.16 ARM (SO SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
allocates **0x114 bytes** at `0xaf232`, invokes `Player::Player` at `0xaf250`,
calls the max-shield and max-armor setters at `0xaf262` and `0xaf274`, writes
`Player+0x69` at `0xaf27a`, then begins the **0x3a0-byte PlayerEgo** allocation
at `0xaf282`. The source uses `sizeof(Player)`, whose ARM size is asserted as
0x114. This gate checks values after the same source-level sequence, not
machine-code exactness.

The inherited Pan fixture still reads original station/system/mesh/texture
assets. Its `Ship` has four available positions but no installed items, and
sets synthetic max shield/armor values **80/90**. `Ship::refreshValue` remains
an explicit empty-equipment test boundary. The gate verifies Player radius
**1200**, max/current hull **300**, zero used weapon slots, max/current shield
**80**, max/current armor **90**, `field_69 == 1`, and three empty gun buckets.
The Player constructor, `updateDamageRate`, shield/armor setters and Ship
getters are production bodies; no constructor is replaced with a result stub.

Windows SwiftShader and Android ARMv7 pass at `-O0/-O2`, preserving **18
meshes**, **22 transforms**, **44 collision volumes**, **26/26** original-file
opens/closes and six GLES texture readbacks. Native `gof2`, ABI/lifetime smoke
and **91 verifier tests** pass (one prior skip). No production source changed;
the last full ARM report remains **4,323/4,524** originals, **4,735 pairs**,
**93.93%** mean source-shape, **83.22%** strict fuzzy, **2,551 linked-exact**
and **1,079 byte-exact**. Its SHA-256 is
`79c04bc41f531c75cceb997f4e3304229011790690c5dffef864c1442e2dc50a`.
This test-only gate cannot alter exact-pair sets.

Next: integrate real catalog-backed Ship/equipment and the production
`PlayerEgo` constructor, `setShip` and weapon-position loader. This gate does
not prove those paths, Level stage-0 return or a playable native frame.
