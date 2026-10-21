# Fighter follow target and PlayerEgo camera pointer (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`PlayerFighter::update(int)` at `0xdcfd8` was checked at
`0xddb98–0xddd76` against the ELF disassembly and REA/IDA pseudocode.
`PlayerEgo::setTargetFollowCamera` at `0xa21f2` stores the camera address
at `+0x88`; `getTargetFollowCamera` at `0xa2202` loads the same slot.

The Fighter follow override requires the body's low enemy flag, state 5
or wingman command 1, and `field_0x128 >= 1`. In rocket control the target
is the follow camera's geometry; otherwise it is the ego's Player position.
Both paths update `targetPos` and `resetVecC`. The original then checks
strict `(-25000, 25000)` bounds on X and Y but **only an upper bound** on
Z. In range it enters state 1, makes the secondary geometry visible when
present (otherwise the primary geometry), and activates the body. The
production-method smoke tests the gates, both target sources, strict
boundaries and the asymmetric Z case at `-O0` and `-O2`.

The camera field and getter formerly used `int`, which truncated the
pointer on 64-bit hosts. They now use `TargetFollowCamera*`; the ARM32
offset, symbol names and getter/setter machine-code matches remain intact.
The related Gun call uses the typed pointer. A host smoke extracts the
production getter/setter and `hitCamera`, checks pointer round-trip and
adjacent storage, and asserts the real header leaves enough room before
`explosion`. Native UCRT64 `gof2`, all 208 NDK r18b ARM translation units,
all 11 Fighter smokes, the camera-pointer smoke, and 93 verifier tests
pass (one existing skip).

Focused whole-`update` source-shape/strict fuzzy changes **39.9%/18.5%
→ 42.2%/12.3%**; the strict fuzzy decrease is reported, not hidden.
The 44-function Fighter unit retains **21 linked-exact** and **10
byte-exact** pairs. All 173 PlayerEgo pair scores remain unchanged,
including **99 linked-exact** and **61 byte-exact**; the 38 Gun pairs
retain **27 linked-exact** and **13 byte-exact** with no score changes.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact** matches,
with zero skipped, wrong-type, or exact-pair losses and 124 extra symbols.
Comparing `(unit, symbol)` pairs with the preceding command-route report
finds only the expected `PlayerFighter::update` score change. Full report:
`fighter-follow-camera-pointer-full.json` (SHA-256
`71ce97bee9c40b5891440617d1bd3b50d03eb33e5915b039f26bf298554807a1`).

A wider optional sweep of 28 older PlayerEgo smokes passed 22 and failed
six (`cloak_ctor`, `cloak`, `dock_asteroid`, `handle_ship`, `jump_drive`,
`turret_view`). Those standalone fixtures extract source ranges untouched
by this package; their prior baseline was not rerun, so this result is
not treated as a regression verdict or a green gate.

Docking navigation, health/particle effects, later movement and the
common tail remain unresolved. A complete game frame has not been
validated.
