# Fighter post-movement reactions (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdf260–0xdf450`. The local recovered IDA
`PlayerFighter.c` lines 2621–2693 cross-check the control flow but are not
original C++ source. ARM relocations at `0x20d98c`, `0x20d99c` and
`0x20db74` identify the virtual `KIPlayer::outerCollide(Vector const&)`,
`KIPlayer::getProjectionVector(Vector const&)` and
`PlayerFighter::push(int)` slots used here.

After `AEGeometry::updateReferenceMatrix`, Fighter byte `+0x13e` gates two
independent passes: landmarks first, enemies second. Each pass skips null
entries and candidates whose virtual `outerCollide` rejects the cached
Fighter position at `+0x2c`. The original reloads the array entry after
that call, then obtains its projection vector. A zero projection is ignored.
For the first nonzero projection in each pass, the geometry direction is
saved to Fighter `+0x90`; `(projection - direction) * (currentSpeed * 0.03f)`
is added to it, normalized into Fighter `+0x9c`, and passed to
`AEGeometry::setDirection` with up `(0,1,0)`. The geometry moves forward
by `currentSpeed * float(dt)`, and only that pass stops. The enemy pass
still runs after a landmark reaction. Finally the Fighter virtual `push(dt)`
runs whether or not reactions were gated in. The state-1/state-7 branch
now returns there rather than falling through the unreconstructed body.

The production-method smoke at `-O0/-O2` covers the gate-off case, null
entries, collision rejection, zero projection, first-hit stop, landmark
then enemy order, position argument, steering result, forward distance and
trailing push. All 11 Fighter smokes and the native UCRT64 `gof2` build
pass. The 93 verifier tests pass with one existing skip. Focused ARM
comparison of 44 Fighter pairs changes whole-`update` source-shape/strict
fuzzy **92.5%/36.8% → 94.7%/39.1%**; the function remains nonexact. All
**21 linked-exact** and **10 byte-exact** focused pairs are retained.

The full ARM corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **201** still absent, zero skipped and zero wrong-type
symbols. Rounded means remain **94.05%** source-shape and **83.38%** strict
fuzzy. Pairwise comparison with the preceding full report found no missing
pair, no linked-exact or byte-exact loss, and no score change outside
`PlayerFighter::update`. All **2,557 linked-exact** and **1,082 byte-exact**
pairs are retained. The ARM build has **208** translation-unit objects.

The initial value of `+0x13e` is not independently confirmed, and this
fixture does not execute every derived collision/projection method in a
complete game frame. Other state fallthroughs still use `PF_update_body`;
classify and recover them before removing that boundary.
