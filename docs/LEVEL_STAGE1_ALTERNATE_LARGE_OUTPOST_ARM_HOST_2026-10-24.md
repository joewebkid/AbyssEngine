# Alternate large outpost and eight turrets — Android ARM/host (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`
calls `Level::createStaticObject(wp, 18804, true)` in
`Level::createMission` at `0xb2e9c–0xb2ea6`, then clears space
points, sets position/moving/health and performs the shared final
relocation. Its eight turret records begin at `.rodata` `0x1fcd74`;
the original turret loop at `0xb329a–0xb3394` selects this table
when the alternate flag is set. The recovered `createStaticObject`
branch loads the carrier's base and six child meshes, dock group 5
from `data/bin/docks_hd.bin`, wreck mesh **18766** and static
collision **2005**.

`tools/run_level_stage1_alternate_large_outpost_pan_gate.py` forces
the alternate flag after the original selection draws. This is a
synthetic variant: the Pan fixture's campaign mission does not
naturally select it. The source-extracted host chain passes Windows
SwiftShader `-O0/-O2`, constructs type **18804** and eight distinct
type-**6772** turrets, and checks the carrier's loaded meshes,
nonempty dock data before mission clearing, health **9,999,999**,
collision, final position and each turret's mesh, rotation, scale,
group and position. Unlike the non-alternate class-one ship, this
static carrier is not added to the Level LOD object list. The
fixture still supplies a local label for turret text key 1666 and
fail-closed implementations of unreached virtual methods.

Production C++ did not change. Native `gof2` is green (no-op).
Focused ARM `createMission` is **98.5% source-shape / 40.3% strict
fuzzy**; `createStaticObject` is **98.9% / 44.3%**, both nonexact.
The last full ARM corpus remains **4,323/4,524** originals over
**4,736** pairs, mean **94.19% source-shape / 83.54% strict fuzzy**,
**2,564 linked-exact** and **1,082 byte-exact**; the test-only
change cannot alter exact sets. Turret combat/teardown, the small
outpost and live Android remain unverified.
