# StarSystem supernova-intro transform handoff (2026-10-11)

`StarSystem::switchSunForSupernovaIntro` previously called two null function
pointers after changing the sun mesh and scale. The Android 2.0.16 ARM body
at `0x134ea0` instead calls `PaintCanvas::TransformGetTransform` for the
first sun geometry and then `Transform::SetAnimationState` with modes
**0, 3, 1**, passing null as the second argument each time. The original
reloads the sun geometry and transform for every call (`0x134f2a..0x134f70`)
and uses `Globals::Canvas` and `Globals::status`. The source now follows
that order through the existing typed engine methods and removes the null
call targets. It also reloads `sunStreak` after `setMesh`, as at
`0x134ee4..0x134ef8`.

The only source caller found is `LevelScript`'s supernova transition in
state 3. Focused ARM verification rose from **57.5% source-shape / 42.6%
strict fuzzy** to **100% / 100%** at **108/108 instructions**; it is not
linked- or byte-exact. Native `gof2`, all **206** ARM translation units and
36 verifier tests pass (one skipped). The full-corpus result and retained
report are **4,323/4,524** compared at **91.95%** mean source-shape (+0.01)
and **80.04%** strict (+0.01), with **2,319 linked-exact** and **997
byte-exact**, no lost exact pair, no skipped or wrong-type symbols. The JSON
is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_star_intro_20261011.json`.

This verifies the recovered call sequence and build shape, not an Android
device playthrough. `TransformGetTransform` can return null for an invalid
handle, and the original path makes no null check; no invented fallback was
added. Next priority is a controlled supernova transition/runtime test and
the remaining `StarSystem` initialization assumptions.
