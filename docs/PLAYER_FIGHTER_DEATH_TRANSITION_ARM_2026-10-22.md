# Fighter death transition (Android ARM, 2026-10-22)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`PlayerFighter::update(int)` at `0xdcfd8` was checked at
`0xddf18–0xde1a4` against the ARM disassembly and the previously saved
IDA-derived pseudocode. The live REA function-analysis provider did not
return usable output during this cycle, so the instruction trace is the
primary behavioral evidence.

After the type-13 freelance-mission flag handling, a Fighter with its
`+0x42` wanted marker set terminates and deactivates the indexed Wanted
entry, displays its reward, adds the reward to credits, and increments
the appropriate bounty-board count. The original calls `getReward`
twice; the source keeps that order. An enemy death then handles the
ship-group-9 cargo pair, or the ship-group-8 pirate-kill count when
`Player::destroyedByte` is clear, before notifying `Level::enemyDied`
with `kind` and that byte. A friend death notifies `friendDied`, then
`wingmanDied(name)` when the wingman byte is set. The unusual loot
branch for other groups checks `nextInt(100) < 0`; it is retained from
ARM, though the reconstructed normal RNG returns only 0–99.

All death paths join at a one-time transition to state 3. They clear
`KIPlayer +0x73`, set a 1500–2999 random timer at Fighter `+0x1f8`,
copy the geometry direction to the newly typed Vector at `+0x194`,
emit a manual particle at the current render position, play sound 20
with positional routing controlled by `Globals::options[0x0f]`, and
form a normalized random drift scaled by 0.05 at the newly typed
Vector `+0x188`. The code enables the engine-trail emitter, disables
the three render-system emitters, refreshes the cargo-present byte,
and hides exhaust. The ARM32 layout assertions lock both Vector offsets.
Existing `PlayerFixedObject` death handling independently corroborates
the sound-option byte and general notification ordering; it does not
prove Fighter-specific effects.

The production-method smoke now covers friend, wingman, wanted/pirate
and ship-group-9 cargo outcomes, the ordered effect handles, positional
versus nonpositional sound, direction/drift, and no duplicate payout on
a subsequent state-3 frame. All 11 Fighter smokes, native UCRT64
`gof2`, all 208 NDK r18b ARM units, and 93 verifier tests pass (one
existing skip). Focused whole-`update` source-shape/strict fuzzy
changes **45.9%/20.1% → 53.3%/17.8%**; the fuzzy decrease is reported
explicitly. The 44-function Fighter unit retains **21 linked-exact**
and **10 byte-exact** pairs.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact**
matches, with zero skipped, wrong-type, or exact-pair losses and 124
extra symbols. Comparing `(unit, symbol)` pairs with the preceding
freelance-death report finds only the expected `PlayerFighter::update`
score change. Full report: `fighter-death-transition-full.json`
(SHA-256
`4825c418735d0aefcb181d79c2bbccff05c00dfeb1f3d460cef4f130feba7d2f`).

The subsequent state-3 animation/update path and the rest of the frame
still remain behind `PF_update_body`. This test does not validate a
complete game frame or production audio/particle rendering.

Follow-up ARM map (not yet implemented): the state dispatcher at
`0xde1a8–0xde1c8` skips state handling when `aiDisabled` is set;
state 3 enters at `0xde2c0`. It clears the `+0x101` and `+0x13e`
bytes, rotates the geometry using the `deathDrift` vector at `+0x188`
when `dt >= 1`, translates by `deathDirection * float(dt) *
currentSpeed`, and decrements the `+0x1f8` timer. Only after that timer
passes below zero does it start `explosion`, disable the engine-trail
emitter and (when `Globals::options + 0x28` is positive) two further
particle emitters, apply a randomized bomb force, choose a normalized
vector at `+0x17c`, switch to state 4, release an occupied `SpacePoint`,
and conditionally create a crate. These observations delimit the next
implementation package; they do not change the verified results above.
