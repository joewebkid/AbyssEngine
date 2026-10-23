# Fighter state dispatch and placeholder removal (2026-10-24)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xde1bc–0xde1dc` and `0xde26e–0xde274`.
The local recovered IDA `PlayerFighter.c` lines 2215–2221 and 3042–3043
independently show the same switch and default return; they are not
original C++ source.

The ARM halfword jump table reads the state at Fighter `+0x88`. State
`0` branches to `0xde26e`, stores state `1` and returns through the
stack-check epilogue at `0xde1dc`. State `2` branches directly to that
epilogue; values above `9` do likewise. States `1` and `7` dispatch to
their already recovered movement branch. The previous `PF_update_body`
call at this boundary had no matching original call contract and is
removed. The native static library no longer retains that unresolved
reference from `PlayerFighter::update`.

The production-method smoke at `-O0/-O2` checks `0 → 1`, state `2` and
an out-of-range state; it also verifies none of them dispatches `push`.
The fixture's obsolete fallback counter was removed. All 11 Fighter
smokes, native UCRT64 `gof2`, 208 NDK r18b ARM translation units and
93 verifier tests pass (one existing skip). Focused comparison of 44
Fighter pairs changes whole-`update` source-shape/strict fuzzy
**94.7%/39.1% → 94.7%/38.3%**. This deliberate strict-fuzzy loss does
not imply a semantic regression: the original has the state write and
epilogue, not a fallback call. The function remains nonexact; all
**21 linked-exact** and **10 byte-exact** focused pairs are retained.

The full ARM corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **201** still absent, zero skipped and zero
wrong-type symbols. Rounded means remain **94.05%** source-shape and
**83.38%** strict fuzzy. Pairwise comparison with the preceding full
report found no missing pair, linked-exact or byte-exact loss, and no
score change outside `PlayerFighter::update`. All **2,557 linked-exact**
and **1,082 byte-exact** pairs are retained.

This is a switch-tail recovery, not a full game-frame validation. State
may change earlier in `update`, and the test covers the dispatch outcome
after that prelude, not every possible transition. `PlayerFighter::roll(int)`
had already been recovered and tested separately; the next unverified
Fighter method was `revive()`.
