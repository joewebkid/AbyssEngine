# Pan first ordinary freighter: class-one return and enemy insertion (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` `Level::createShip` starts at `0xc0214`;
its class-one dispatch is at `0xc032c`. The existing production body in
`src/game/world/Level.cpp` constructs `PlayerFixedObject`, selects
race-specific `BoundingAAB` bounds, assigns a wreck mesh/collision set,
creates the ship group, adds its geometry to LOD and returns the object.
This gate executes that source-extracted branch, not a separate test
constructor. `PlayerFixedObject::setBV` and `setWreckedMeshId`,
`Globals::getWreckCollision` and `FileRead::loadWreckCollision` are also
source-extracted. The original read-only `wreck_collisions.bin` is 2,232
bytes, SHA256 `f67fd119f3622cb76d818ae1ded0ba5aa4e9d363cbaf68abc757de462938264a`.

`tools/run_level_stage1_first_freighter_ship_return_pan_gate.py` runs
the Pan chain with original catalog, mesh, texture and collision assets
on Windows SwiftShader. It checks RNG/HP/EMP prelude, the returned
freighter in `enemies[friendCount + field_164]`, Player back-reference,
position, level pointer, three race-0/race-2 bounds, nonempty wreck
collision, race-specific wreck mesh/type, LOD ordering and unchanged
special-kind-15 material handoffs. The test's random sample reached a
race-2 freighter; race-0 logic is present but not independently claimed
as observed. `-O0` passed after three ordinary friends plus one pre-dead
Fighter, and `-O2` passed after four plus one.

The fixture still forces security 3, one pre-dead Fighter, one ordinary
freighter and no outpost after preserving selection RNG draws. It
stops immediately after insertion: the subsequent `setMoving(true)`,
relocation, additional freighters, outpost paths, stage-one completion,
render/update, teardown and live Android behavior are unverified. The
host uses a small `TransformRemoveChild` child-array/animation-data
model because its original opaque engine helpers are not linked; this
is a test seam, not a production-source claim. `FileRead_ctor/dtor`
aliases in the gate only invoke the existing empty `FileRead` lifecycle.

No production C++ changed. Native `gof2` build is green (no-op); a new
ARM corpus was not run for this test-only change. The latest full ARM
result remains **4,323/4,524** originals across **4,736** pairs, mean
**94.19% source-shape / 83.54% strict fuzzy**, **2,731 100%-fuzzy**,
**2,564 linked-exact**, **1,082 byte-exact**, **201 missing**, with no
previously observed exact loss. Next priority: run the ordinary
freighter's `setMoving` and `setPosition` calls, then cover the
large/small outpost alternatives.
