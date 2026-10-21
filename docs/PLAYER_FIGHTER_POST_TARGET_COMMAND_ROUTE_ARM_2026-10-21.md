# PlayerFighter post-target command route (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xdcfd8`. Original instructions at
`0xdda88–0xddb94` were checked against the ELF disassembly and REA/IDA
pseudocode. This is the continuous block after the previously recovered
route/enemy handoff, not the complete update method.

With active AI and outside states 3/4, `jumperFlag` and a selected target
(`field_0x12c`) accumulate `dt` in `field_0xf0`. At 20,000 or above, ARM
sets state 6 and clears both timer and selection flag. If either condition
is false, the timer resets; disabled AI skips this timer branch entirely.

Outside states 3/4, a wingman with command 2 updates its `commandRoute`
from the geometry position. A null route or `getCurrent()` above
`field_0x1e4` changes the command to 1; the latter case first destroys
and clears the route. When the route remains in range and has a waypoint,
its integer coordinates become `resetVecB` and `field_0x12c` is set.
The following shared block computes `resetVecC = resetVecB - renderPosition`;
it also runs after the state-3/4 sound-stop path, but not after the earlier
inactive return.

The production-method smoke checks timer accumulation and reset, the
AI-disabled skip, missing/in-range/completed command routes, destruction,
waypoint coordinates and the shared vector at `-O0` and `-O2`. All 11
Fighter smokes, native UCRT64 `gof2`, all 208 NDK r18b ARM translation
units, and 93 verifier tests pass (one existing skip). Focused whole-
`update` source-shape/strict fuzzy changes **36.7%/18.7% → 39.9%/18.5%**.
The 44-function Fighter unit retains **21 linked-exact** and **10
byte-exact** matches without pair losses. The strict fuzzy decrease is
recorded rather than optimized away at the cost of the evidenced behavior.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact** matches,
with zero skipped, wrong-type, or exact-pair losses and 124 extra symbols.
Comparing `(unit, symbol)` pairs with the preceding handoff report finds
only the expected `update` score change. Full report:
`fighter-update-command-route-full.json` (SHA-256
`97b19ddde7bf7c9edf34026e9fd6ba98830e103e0e7f9509ea1fcbc42636ab27`).

The special follow-camera override after `0xddb94`, later movement, and
the method's common tail remain unresolved. A complete game frame has not
been validated.
