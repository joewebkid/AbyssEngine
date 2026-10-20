# PlayerFighter wingman route rebuild (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This package extends the
wingman target/audio handoff at `0xdd22c` with the guarded maneuver
branch at `0xdd2d4–0xdd74c`; it is not a recovery of the complete
update method.

Only when `wingmanFlag`, state != 4, `routeClone`, and `field_0xe4 == 1`
are present does ARM rebuild the wingman route. Command 0 subtracts
the Ego geometry's right vector scaled by 4,000 from `targetPos`;
command 1 adds it. Command 2 adds the up vector scaled by 2,000.
Commands 0/1 then subtract the normalized direction scaled by 3,000;
command 2 subtracts it scaled by 2,000. Other command values leave
the copied Ego position unshifted. The constants are the literal ARM
words `0x457a0000`, `0x453b8000`, and `0x44fa0000`. The scaled side/up
vector is retained in the Fighter scratch `objectPosition` at `+0x9c`.

At `0xdd6ca–0xdd74c`, a route of fewer than two points receives
`setNewCoords(targetPos)`. A route of at least two points is destroyed
and replaced by `Route(int[3], 3)` with each float coordinate truncated
to an integer. Both paths set the route looping and then rejoin the
previously recovered Player audio handoff at `0xdd240`. This follows
the original's pointer ownership and avoids leaving a stale route.

The production-method smoke covers commands 0/1/2 and an unknown
command, both route-length branches, scaled vector order, integer
truncation, destruction/replacement, loop setting, and the audio
handoff at `-O0` and `-O2`. All 11 Fighter smokes pass. Native UCRT64
`gof2` builds, all 208 NDK r18b ARM translation units compile, and
93 verifier tests pass (one pre-existing skip). Focused whole-method
source-shape/strict fuzzy improves **12.1%/8.4% → 17.9%/10.1%**.
The low whole-method score reflects the much larger unreconstructed
remainder.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, at **94.03%/83.37%** mean source-shape/strict
fuzzy, with **2,557 linked-exact** and **1,082 byte-exact**. There are
zero skipped or wrong-type symbols. Comparing `(unit, symbol)` with
the preceding audio-handoff report finds no exact-pair losses; only
`PlayerFighter::update` changes source-shape. Full report:
`fighter-update-wingman-route-full-current.json` (SHA-256
`a01914be03a1824eca6449b8dad3e065801c4d9fc7662a26e58fabdce5c2f449`).

The enemy-list, route-following, combat and movement branches after
`0xdd27e` remain in unresolved `PF_update_body`. Complete game-frame
and visual wingman behavior have not been validated.
