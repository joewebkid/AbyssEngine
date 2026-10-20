# PlayerFighter update route/trail/matrix block (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. This package extends the
previously recovered prelude and allegiance block with the contiguous
ARM section at `0xdd1c6–0xdd22c`. It is not a recovery of the complete,
approximately 3,556-instruction update method.

At `0xdd1c6`, ARM checks the Fighter's wingman flag at `+0xdc` and the
base `routeClone` at `+0x6c`; only for a non-wingman with no clone does
it call `Route::clone()` on the Fighter route at `+0x144`. The trail
timer at `+0x1c8` accumulates `dt`. Once the value reaches 201 or more,
it calls `Trail::update(current, previous)` if a trail exists, using
`renderPosition` at `+0x2c` and `workingPosition` at `+0x158`, copies the
current position into `workingPosition`, and resets the timer. The timer
also resets if there is no trail.

ARM then copies `geometry->getMatrix()` into the associated Player's
matrix at `+0x4`; when `subGeometry` at Fighter `+0x0c` exists, it
right-multiplies that matrix by `subGeometry->getMatrix()`. The C++ uses
the existing typed Route, Trail and Matrix APIs in the same order.

The expanded production-method smoke exercises clone creation and
wingman suppression, the 201 ms boundary with and without a trail,
current/previous vector order, and matrix composition at `-O0` and
`-O2`. Native UCRT64 `gof2` builds, all 208 NDK r18b ARM translation
units compile, all 11 Fighter smokes pass, and 93 verifier tests pass
(one existing skip). Focused whole-method source-shape/strict fuzzy
improves **8.4%/6.0% → 10.2%/7.5%**. The 44-function Fighter unit keeps
21 linked-exact and 10 byte-exact matches. The low whole-method score
reflects the much larger unreconstructed remainder.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, at **94.03%/83.37%** mean source-shape/strict
fuzzy, with **2,557 linked-exact** and **1,082 byte-exact**. There are
zero skipped or wrong-type symbols. Comparing `(unit, symbol)` with the
preceding allegiance-block report finds no exact-pair losses; only
`PlayerFighter::update` changes source-shape. Full report:
`fighter-update-route-trail-full-current.json` (SHA-256
`ac64fccac0d968e1ccb6cef4fa197f3a75ea4dcb606d12e58cc1581594179944`).

The smoke uses recording geometry and Trail mocks; it does not show a
complete game frame or visual trail. `PF_update_body` remains unresolved
after this block. The next bounded ARM region begins at `0xdd22c` with
route/wingman state and `Player::update`, then branches into combat and
movement; its branch joins need mapping before implementation.
