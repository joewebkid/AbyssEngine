# Pan first-friend Route clone prerequisite (2026-10-24)

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `Route::clone` at `0x11c0b6`; the prior [Route analysis](ROUTE_UNIQUE_ARM_98_2026-10-08.md)
established its coordinate copy, docking-target scan, constructor selection,
and loop transfer. `PlayerFighter::PlayerFighter` at `0xdc2f8` calls `clone`
when campaign mission is not 41. This checkpoint tests that dependency on
the original-asset Pan stage-one route after the first ordinary friend's
production `Player` and EMP setup.

`tools/run_level_stage1_first_friend_route_clone_pan_gate.py` source-extracts
the production `Route::clone`, docking constructor, and `setLoop` into the
existing Pan gate. For both the plain and populated-docking-target paths it
checks a distinct route and waypoint with identical coordinates. The second
path also checks a distinct target array, preserved target pointer and docking
time, and loop propagation. The fixture restores the source route afterward.
The two test clones live until process exit because this gate does not link
the unrelated `Waypoint`/`Player` destructor chain; this is not a lifetime
or leak check.

Windows SwiftShader runs at `-O0` and `-O2` pass with the original Pan assets.
NDK r18b ARMv7 `-O0` and `-O2` fixture executables compile and link. At
the initial checkpoint ADB listed no device; the follow-up below adds
hardware execution. The real
`PlayerFighter` constructor, its `Generator`/`Explosion` services,
`Level::createShip` return, enemy-array insertion, and subsequent ships
remain unverified. Directly linking the full Fighter translation unit into
this bounded gate exposed 179 unresolved references; linking the complete
native library also collided with the gate's existing definitions. Those
experiments are not treated as a constructor pass.

No production source changed at this checkpoint. Its then-current full ARM corpus was
**4,323/4,524** originals across **4,736** pairs, **94.09%/83.41%** mean
source-shape/strict fuzzy, **2,557 linked-exact** and **1,082 byte-exact**
counts, with zero skipped or wrong-type symbols. The new test did not
remeasure exact-pair identities. Later corpus metrics are in `PROJECT_STATUS.md`.
Next, isolate a linkable real Fighter
constructor and its runtime services without silently replacing them with
test-only behavior.

Device follow-up (2026-10-24): the same source-extracted gate was rebuilt
with NDK r18b as ELF32 ARM PIE executables and passed at `-O0` and `-O2`
on a connected Android 16 device. Original Android OBB bytes were served
from the local asset tree over the gate's ADB-reverse file boundary; the
game itself and its renderer were not launched. The executable SHA-256
values are `31a38af5167672220306b400645bb6f645113d5d124afb46ca95661fc6af20b`
(`-O0`) and `57e5cb81a6c6d32f0bc87ebad06491e51c4077231e2af17ab01a98d440ff2aa1`
(`-O2`). Both temporary executables were removed from `/data/local/tmp`
after the pass.
