# Fighter maneuver-bypass roll recovery (2026-10-23)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`PlayerFighter::update(int)` at `0xded44–0xded6c` and `0xdf042–0xdf110`.
The local recovered IDA `PlayerFighter.c` at lines 2399–2557 independently
supports the control-flow interpretation; it is not treated as original C++.

With maneuver flag Fighter `+0x12d` set, the original copies the geometry
matrix, then clears the five-sample roll index at `+0x2b0`, the float
target roll at `+0x20c`, and the buffer-filled byte at `+0x2b4`. It skips
the ordinary heading/roll-target calculation and joins the common roll
section. The ABI types of target roll `+0x20c` and smoothed roll `+0x210`
are `float`, as shown by the VFP loads, arithmetic and stores; the prior
header described them as integers of the same size.

When inherited roll-active byte `+0x100` is set, `roll(dt)` runs first.
Smoothed roll approaches target by `float(dt) * 1.25f / 3.9f` and is
clamped to the target on overshoot. The timer bits at inherited `+0xfc`
are compared as signed and decrease by `dt` while positive; reaching
zero or below sets roll-active. If target and smoothed roll are equal,
the still-positive timer decreases a second time in the same frame.
Otherwise the timer is reset to 750. The second decrement is unusual
but appears in both ARM and recovered IDA control flow, so it is
preserved.

This patch executes the confirmed roll-state operations on the maneuver
bypass. The normal-path steering calculation at `0xded6e–0xdf03e`,
shooting branch at `0xdef12`, and subsequent matrix application,
translation, particle and landmark updates remain unrecovered. The
fallback `PF_update_body` still marks that boundary. No full game-frame
maneuver is validated.

The production-method smoke exercises partial smoothing, overshoot,
roll invocation, timer reset and the equal-roll double decrement at
`-O0/-O2`. Native UCRT64 `gof2`, all 11 Fighter smokes, 208 NDK r18b
ARM translation units, and 93 verifier tests pass (one existing skip).
Focused 44-pair Fighter verification changes whole-`update`
source-shape/strict fuzzy from **83.2%/34.3%** to **84.4%/34.0%**;
only this function's score changes. The **21 linked-exact** and
**10 byte-exact** Fighter pairs are retained without loss.

The full ARM corpus compares **4,323/4,524** originals across **4,735**
pairs at rounded **94.05%/83.38%** mean source-shape/strict fuzzy. All
**2,557 linked-exact** and **1,082 byte-exact** pairs are retained with
zero exact-pair losses, skipped or wrong-type symbols. Pairwise
comparison with the preceding full report finds only
`PlayerFighter::update` changed. The 201 absent originals remain
unchanged; the corpus means round to the same displayed values.
