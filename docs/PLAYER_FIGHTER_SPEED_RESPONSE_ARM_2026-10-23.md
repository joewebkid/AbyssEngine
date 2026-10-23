# Fighter state-1/state-7 speed response (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` shared state-1/state-7 dispatch at
`0xde1c4`, damage branch at `0xde1fa–0xde26c`, and speed timer at
`0xde840–0xde9c6`. The original reads `+0x1ec` as a float and `+0x1f4`
as a byte; the reconstructed fields and ARM layout assertions now
reflect those access widths.

Both states call `handleCloaking()`. When `+0x13d` is set, a fall in
hitpoints accumulates the difference at `+0x1dc`. The current value is
saved at `+0x1d8` only on a decrease. Accumulated damage strictly above
**40%** of maximum hitpoints sets the `+0x1c0` timer to **10000**, clears
the accumulator and sets `+0x1e0`. The original has no local zero-max-HP
guard. When the timer exceeds **5000** and the `+0x1f4` byte is clear,
the timer resets and either the pending `+0x1e0` flag or an RNG draw
strictly below `boostProb` starts a speed interval of
`5000 + nextInt(3000)` ms. Target speed becomes **5.5f**, and target
rotation becomes **1.3f**.

While the interval is active, current speed is multiplied by **1.05f**
toward a higher target or **0.95f** toward a lower one. A value reaching
5.5f or falling below the base speed is clamped to the target, and the
target slot is cleared. After the interval duration is exceeded, the
pending flag and timer clear, and speed/rotation targets return to their
base fields. The active byte clears only when speed has reached its
base value. These thresholds and float literals follow the ARM branches
and data words at `0xdec20–0xdec28` and `0xdf840–0xdf848`.

The production-method fixture checks the exact 40% boundary, a later
damage-triggered interval, its duration and cooldown, the strict RNG
threshold, continued active flag after reaching 5.5f, and the
disabled-response gate at `-O0/-O2`. Its RNG and route
providers are mocks. Later state-1/state-7 orientation, shooting and
movement still end at the unresolved `PF_update_body`; this change does
not validate a complete game frame or physical route following.

Focused NDK r18b verification compiles all 208 ARM translation units
and compares 44 Fighter pairs. Whole-`PlayerFighter::update`
source-shape/strict fuzzy changes **79.7%/27.9% → 81.1%/33.7%** and
remains nonexact. All 21 linked-exact and 10 byte-exact Fighter pairs
are retained. Retyping the speed/flag slots changes the two nonexact
constructor variants' rounded source-shape from 94.6% to 94.5%; their
strict fuzzy and exact status are unchanged. Native UCRT64 `gof2`, all
11 Fighter smokes and 93 verifier tests pass (one existing skip).
The full `--no-build --fail-on-wrong-type` corpus compares
**4,323/4,524** original functions across **4,735** pairs, with zero
skipped or wrong-type symbols. Mean source-shape/strict fuzzy remains
**94.05%/83.38%** after rounding. A pair-by-pair comparison with the
navigation-to-docking baseline retains all **2,557 linked-exact** and
**1,082 byte-exact** pairs, with zero exact losses or gains. Only the
`update` and two constructor scores named above changed.
