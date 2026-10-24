# Pan maximum ordinary-freighter loop — host gate (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` has `Level::createMission` at
`0xaffd8`; the ordinary class-one creation, array store,
`setMoving(true)` and relocation sequence is at `0xb30e2–0xb3174`.
The recovered production source draws `field_168` with `nextInt(5)`,
so its ordinary-freighter count can range from zero to four. The
existing full ARM comparison for `createMission` is **98.5%**
source-shape and **40.3%** strict fuzzy, nonexact; this host gate does
not improve or remeasure that function.

`tools/run_level_stage1_four_freighters_pan_gate.py` keeps the original
count-selection RNG draw, then forces its maximum value of four. It
extends the original-asset Pan host chain through all four ordinary
loop iterations, after synthetic security 3 and one pre-dead Fighter.
The gate checks four distinct `PlayerFixedObject`/Player pairs in
ordered enemy slots, Pan/foreign faction, class-one type 15, moving
state, wreck mesh/collision, three bounds, aligned geometry/wreck/
collision positions and LOD order. The final iteration also retains
the prelude and relocation RNG replay from the prior gate. Windows
SwiftShader `-O0/-O2` passes with **5/4** ordinary friends respectively.

No production C++ changed. Native `gof2` is green (no-op); the latest
full ARM corpus remains **4,323/4,524** originals over **4,736** pairs,
mean **94.19% source-shape / 83.54% strict fuzzy**, **2,564 linked-exact**
and **1,082 byte-exact**. The fixture still excludes outposts after
their original selection draws and uses a test-only
`TransformRemoveChild` model for opaque renderer helpers. It does not
prove the natural frequency of four freighters, later `createMission`
work, ship teardown or live Android execution. Next priority is the
large/small outpost branch and its following turret placement.
