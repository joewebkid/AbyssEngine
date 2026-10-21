# PlayerFighter target-search prefix (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This package extends the
previously recovered enemy-index validation with the target-search
prefix at `0xdd3c6–0xdd620` and acquisition joins at `0xdf770` and
`0xdf77a`. It is not a recovery of the complete update method.

With a non-null enemy list, ARM clears Fighter `field_0x148` and
branches on `maneuverTimer < 5001`. Below the threshold, if
`field_0x12e` is clear it scans the list in order, skipping null,
inactive and dead Players. Each eligible Player's position is copied
to `targetPos`. It selects the first target either when the Fighter's
ship group is not 8 and its Player has `turnedEnemy()`, or when all
three `renderPosition - targetPos` components are strictly inside
`(-field_0x128, +field_0x128)`. Selection sets `field_0x12e` and the
index in `field_0x38`.

At or above 5,001, ARM clears `field_0x12d` if it was set; otherwise
it sets the byte only on `Globals::rnd->nextInt(100) < 20`. It resets
`maneuverTimer`, then makes a separate `nextInt(100)` draw. For a
result at most 29 and at least two enemies, it clears `field_0x12e`
and tries up to five random indices. Only an active candidate has
its position copied and tested by the same enemy/cube condition.
If no candidate qualifies, or if the second draw/list-size gate
fails, the prefix sets `field_0x38` to zero. ARM relocations for both
draws resolve to the same `Globals::rnd` pointer at `0x2101f8`.

The production-method smoke tests ordered scan, far-target rejection,
the turned-enemy override, the exact 5,001 boundary, the two-draw
order, early success and five failed retries at `-O0` and `-O2`.
All 11 Fighter smokes pass. Native UCRT64 `gof2` builds, all 208
NDK r18b ARM translation units compile, and 93 verifier tests pass
(one pre-existing skip). Focused whole-method source-shape/strict
fuzzy improves **20.2%/11.2% → 26.4%/12.9%**. The 44-function Fighter
unit retains 21 linked-exact and 10 byte-exact matches, with no
exact-pair losses. The low whole-method
score reflects the much larger unreconstructed remainder.

The final no-lambda source passes the full NDK r18b ARM corpus:
**4,323/4,524** original functions across **4,735** pairs, at
**94.04%/83.38%** mean source-shape/strict fuzzy, with **2,557
linked-exact** and **1,082 byte-exact**. There are zero skipped or
wrong-type symbols and **124** extra symbols, unchanged from the
previous checkpoint. Comparing `(unit, symbol)` with the preceding
enemy-route report finds no linked-exact or byte-exact pair losses.
Full report: `fighter-update-target-search-no-lambda-full-current.json`
(SHA-256 `6f5d83673ec74706441a22fbf2d57f6239ebadbf66c05e8071bed2a8935927b4`).

The prefix's subsequent wingman-target priority, active/dead target
validation at `0xdd50e–0xdd7ac`, combat and movement remain in
unresolved `PF_update_body`. In particular, the index-zero fallback
is a pre-validation state, not a guarantee that target zero remains
selected for the rest of the original method. A complete game frame
has not been validated.
