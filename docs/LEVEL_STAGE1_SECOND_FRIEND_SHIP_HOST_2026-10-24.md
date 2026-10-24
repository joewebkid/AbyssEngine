# Pan second ordinary friend — host gate (2026-10-24)

`tools/run_level_stage1_second_friend_ship_pan_gate.py` extends the
first-friend original-asset gate through the next ordinary
`Level::createShip` call and `enemies[1]` assignment. The test changes
the fixture's security level to 2 before `Level::init` so its friend
count is at least two; the Pan catalog, mesh and texture assets remain
original and read-only. It is a synthetic security variant, not a claim
about Pan's catalog security or a live Android run.

The generated fixture executes the production class-zero `createShip`,
Fighter constructor, ship-group and level setup twice. It preserves the
first ship's captured arguments/RNG state, checks both distinct Fighters
and their player back-references, routes, explosions, material requests,
enemy slots and ordered LOD parent geometries. It stops immediately
after inserting the second ship. The original-asset Windows SwiftShader
gate passes at both `-O0` and `-O2`.

No production C++ changed; the last full NDK r18b ARM corpus is still
**4,323/4,524** originals over **4,736** pairs, mean **94.19%**
source-shape and **83.53%** strict fuzzy, **2,564 linked-exact**,
**1,082 byte-exact**, **2,730 100%-fuzzy**, **201 missing**, zero
wrong-type and zero skipped. No exact count changed in this test-only
package. The last production revision passed native `gof2`, **208/208**
ARM units and **93** verifier tests (one skipped); those were not
repeated for this test-only change.

The material handoff is still a recording stub, not full rendering;
uncalled virtuals fail closed. Fighter ownership/teardown, remaining
friends/enemies, complete stage one and a playable Android scene are
unverified. ADB listed no device during this gate. Next: run the gate
on ARMv7 hardware if available, then continue through the remaining
friend loop and verify lifetime before removing the bounded exception.
