# Pan ordinary-friend loop — host gate (2026-10-24)

`tools/run_level_stage1_all_ordinary_friends_pan_gate.py` extends the
original-asset Pan gate through the complete ordinary-friend loop in
the recovered `Level::init` source. It uses the Pan catalog, meshes and
textures read-only, while setting the test fixture's security level to
3 before `init` to guarantee at least three friends. This is a
synthetic security variant, not Pan's catalog value.

The host gate reached the boundary after the loop at both `-O0` and
`-O2` with Windows SwiftShader. The runs created **3** and **4**
Fighters respectively. For every created friend, it checks a distinct
`PlayerFighter`, its player back-reference, geometry, level, route,
explosion, ship-group flag and exact sequence of requested material
IDs. It verifies `enemies[0..friendCount-1]`, null remaining slots, and
all friend parent geometries in the same order after the asteroids in
the LOD array. The first Fighter's seed and constructor arguments
remain independently checked by the inherited gate. Test-only
material recording and fail-closed virtual stubs remain in place.

No production C++ changed. The latest full NDK r18b ARM corpus remains
**4,323/4,524** originals over **4,736** pairs, mean **94.19%**
source-shape and **83.53%** strict fuzzy, **2,564 linked-exact**,
**1,082 byte-exact**, **2,730 100%-fuzzy**, **201 missing**, zero
wrong-type and zero skipped. The last production revision passed native
`gof2`, **208/208** ARM units and **93** verifier tests (one skipped);
these were not repeated for this test-only addition.

The gate stops before the following `field_164` dead-ship loop. Wanted
targets/wingmen, Fighter teardown, update/render, the rest of stage
one and live Android behavior remain unverified. ADB listed no device.
Next: exercise the following dead-ship loop and its lifetime/ownership
boundaries, then run the fixture on ARMv7 hardware when available.
