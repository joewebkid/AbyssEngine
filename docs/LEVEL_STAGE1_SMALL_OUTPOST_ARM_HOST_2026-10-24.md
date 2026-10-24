# Small outpost and five turrets — Android ARM/host (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` SHA256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`
selects the small-outpost path in `Level::createMission` at
`0xb2f3c–0xb3078`. The call at `0xb2fc2` constructs static type
**19051** with jitter; the branch then clears space points, sets its
position, movement state, health, text key **1667**, field `0x70`,
and cargo. The five-turret loop calls type **6774** at `0xb3408`;
its pose records are in `.rodata` at `0x1fcedc–0x1fcf54`.

`tools/run_level_stage1_small_outpost_pan_gate.py` reuses the Pan
original-asset host chain. The test changes the system to faction
**1** only on entering `createMission`, then forces `smallOutpost`
after the original selection draw. Pan's mission **50** could not
naturally select this variant; this is a bounded synthetic test, not
proof of an ordinary campaign-141 environment. Source-extracted
production branches construct static type **19051**, load its base and
two child meshes, dock group **8**, wreck mesh **18300** and collision
**2006**, clear the dock points, and construct five distinct type
**6774** turrets. The gate checks the original-position RNG draws,
health **9,999,999**, name assignment, collision, resources, dock
file lifetime, each turret's mesh, rotation, scale, group and
position, and Level LOD membership. Test-only localized labels and
unreached virtual-method stubs remain; combat and teardown are not
exercised.

Windows SwiftShader `-O0/-O2` passes with original assets. Native
`gof2` builds (no-op). Focused ARM source-shape/strict fuzzy remains
**98.5%/40.3%** for `createMission` and **98.9%/44.3%** for
`createStaticObject`, both nonexact. Production C++ did not change,
so the last full corpus remains **4,323/4,524** originals across
**4,736** pairs, mean **94.19% source-shape / 83.54% strict fuzzy**,
**2,564 linked-exact**, **1,082 byte-exact**. Natural faction-one
mission selection, live Android, turret combat and teardown are still
unverified. Next: a real faction-one, post-140 campaign fixture.
