# Fighter state-1/state-7 shooting gate (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xded26–0xded44`,
`0xdef12–0xdef52`, `0xdf480–0xdf550` and `0xdf7bc–0xdf818`.
Recovered IDA `PlayerFighter.c` lines 2335–2390 independently
cross-check the control flow; it is not original C++ source.

The gate runs after the direction prelude, before the matrix copy, when
Fighter `+0x12e` is set, `+0x12c` is clear, target Player `+0x148`
exists and this Fighter's Player does not report `doesNeverAttack`.
Target Player byte `+0x5e` clears `+0x12e`; if Fighter byte `+0x2b5`
was set, `Player::stopShooting(slot +0x140)` runs and that byte is
cleared. `+0x2b5` now has an explicit `shootingFlag` field at its
confirmed ABI offset. No explicit write to this byte was found in the
visible constructor disassembly; its initial value outside these
transitions remains unverified.

Otherwise, the local direction's X and Y components must each lie
strictly inside ±`shootError` (`+0x1a8`). The target position minus
Fighter cached position must lie strictly inside ±35,000 on all three
axes. Failing either gate leaves the shooting state unchanged. Within
the gate, Fighter byte `+0x25`, target active/dead state, and the
target Player `+0x69` docked-height exception determine whether to
clear `+0x12e` or call `Player::shoot(slot +0x140, dt, false)` and set
`+0x2b5`. The shoot return value is ignored, as in ARM.
ARM `0xdd048` sign-extends `dt` into `r9`; `0xdf808` passes `dt/r9`
as the low/high words of `Player::shoot(int, long long, bool)`.

The production-method smoke checks firing, stop-on-invalid-target,
strict angle and distance gates, inactive target, docked-height
exception, `doesNeverAttack`, the `+0x12c` gate and byte `+0x25`
at `-O0/-O2`. It also accounts for the independently recovered
8,000/12,000-unit direction-substitution prelude: shooting tests use a
target beyond that substitution threshold. All 11 Fighter smokes,
native UCRT64 `gof2`, 208 NDK r18b ARM translation units and 93
verifier tests pass (one existing skip). Focused comparison of 44
Fighter pairs changes whole-`update` source-shape/strict fuzzy
**90.3%/35.0% → 92.5%/36.8%**. The function remains nonexact; all
**21 linked-exact** and **10 byte-exact** focused pairs are retained.

The full ARM corpus compares **4,323/4,524** originals across **4,735**
pairs, with **201** still absent and no skipped or wrong-type symbols.
Rounded means remain **94.05%** source-shape and **83.38%** strict
fuzzy. All **2,557 linked-exact** and **1,082 byte-exact** pairs are
retained: pairwise comparison found no missing pair, exact loss or
score change outside `PlayerFighter::update`.

Post-reference-matrix landmark/enemy reactions and their trailing
virtual `PlayerFighter::push(int)` dispatch at `0xdf260–0xdf450`
remain unreconstructed. The vtable identification is backed by ARM
relocation `0x20db74` (`_ZTV13PlayerFighter + 0x38`) to
`_ZN13PlayerFighter4pushEi`. This smoke is a bounded method test, not
a full game-frame validation.
