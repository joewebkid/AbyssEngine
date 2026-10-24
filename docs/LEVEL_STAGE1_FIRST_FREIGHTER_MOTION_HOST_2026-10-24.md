# Pan first ordinary freighter: moving and relocation (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` `Level::createMission` (`0xaffd8`)
calls class-one `Level::createShip` at `0xb30e2`, stores the returned
pointer at `0xb30ec`, and calls `PlayerFixedObject::setMoving(true)` at
`0xb30fc`. Its next RNG bounds are 60,000 (`0xb310c`), 2 (`0xb311a`),
40,000 (`0xb312e`) and 160,000 (`0xb314e`, literal); the virtual
position setter is called at `0xb3174`. `PlayerFixedObject::setPosition`
(`0x153eec`) stores float/int coordinates, updates geometry and Player
transform, then all collision bounds and wreck geometry (`0x153f60–0x153f94`).

`tools/run_level_stage1_first_freighter_motion_pan_gate.py` extends the
original-asset Pan host chain with the source-extracted `setMoving` and
`setPosition` bodies and the remaining ordinary-freighter mission
statements. It replays the four relocation RNG draws from the captured
post-construction seed and checks the final seed, moving flag, spawn,
float/int and geometry positions, wreck position, and all three
`BoundingAAB` centers. Windows SwiftShader `-O0/-O2` passes after
**3/4** ordinary friends, one pre-dead Fighter and one freighter.

The current ARM focused verifier reports `setPosition`: **100.0%**
source-shape, **94.2%** strict fuzzy, nonexact (69/69 instructions).
`setMoving` is **linked-exact and byte-exact** (2/2 instructions);
its fuzzy score is 0.0% because of this verifier's normalization,
not an exact-match loss. These functions were not changed. Native
`gof2` remains green (no-op); no new full ARM corpus was needed, so
the latest full figures remain **4,323/4,524** originals, mean
**94.19%/83.54%**, **2,564 linked-exact** and **1,082 byte-exact**.

The same synthetic security-3, one-pre-dead, one-freighter and
no-outpost fixture still applies. The host still models the opaque
`TransformRemoveChild` helper's child-array effect; this does not
verify the complete renderer implementation. The gate stops after the
first ordinary freighter: later freighters, outpost alternatives,
stage-one completion, full lifetime and live Android remain untested.
Next priority is the large/small outpost dispatch and then the rest
of `createMission`.
