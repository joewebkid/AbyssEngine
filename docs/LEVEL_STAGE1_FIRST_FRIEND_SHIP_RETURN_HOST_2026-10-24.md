# Pan first-friend ship return and insertion — host gate (2026-10-24)

The earlier Android ARM-backed Pan checks reach the first ordinary-friend
dispatch with confirmed race, class, type, waypoint and RNG state. The
existing Fighter-constructor gate executes the production constructor but
stops before `Level::createShip` returns. The extended original-asset gate
`tools/run_level_stage1_first_friend_ship_return_pan_gate.py` now runs the
recovered `createShip` class-zero path through production `PlayerFighter`,
`Globals::getShipGroup`, `setShipGroup`, `setLevel` and the enemy-array
assignment. It uses the original OBB catalog and mesh assets read-only.

The inherited pre-insertion assertions were corrected for the new boundary:
`enemies[0]` is the Fighter, every other enemy slot is null, and the
Fighter's parent geometry is the additional LOD object after the asteroid
entries. A clean Windows SwiftShader run passes at both `-O0` and `-O2`.
The gate also checks the Fighter's player back-reference, position, route
and clone, explosion and loot, particle-system setup, and requested ship
materials. The `createShip` source is deliberately sliced to the Pan
class-zero branch; a test-only material handoff records valid requests but
does not implement the renderer. Uncalled virtuals fail closed, and the
allocated Fighter lives until process exit. Other ship classes, lifetime,
update/render, later friends and a playable frame remain unverified.

No production C++ changed in this package. The latest full NDK r18b ARM
corpus therefore remains **4,323/4,524** compared originals over **4,736**
pairs, mean **94.19% source-shape / 83.53% strict fuzzy**, with **2,564
linked-exact**, **1,082 byte-exact**, **2,730 100%-fuzzy**, **201 missing**,
zero wrong-type and zero skipped. `Level::createShip` itself is **96.4%**
source-shape and **48.5%** strict fuzzy, nonexact. The runtime gate is a
behavioral check of the reconstruction, not an exact-match improvement.

ADB lists no device in this run. The next priority is to run this extended
gate as ARMv7 on a device, then continue through subsequent friends and
verify Fighter cleanup/ownership before claiming the scene complete.
