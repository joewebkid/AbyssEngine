# PlayerFighter push recovery (Android ARM, 2026-10-20)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::push(int)` at `0xdfdc8–0xdff2c`. ARM loads the push timer
and duration at `+0x104/+0x108`, the three rotation components at
`+0x118/+0x11c/+0x120`, and the signed 64-bit frame delta at `+0x1d0`.
Its calls set an identity-based rotation, multiply the current geometry
matrix by that basis only for positive frame delta, then scale the vector at
`+0x10c` by frame delta, current speed and
`(2 - fraction) * 3 * (duration / 5000)`. The independent IDA listing
agrees with the call sequence; the ARM argument registers establish matrix
multiplication order.

The previous C++ dereferenced a null stack-guard placeholder, dropped one
rotation axis, truncated a geometry pointer to 32 bits, divided by zero, and
called declared-only matrix/vector shims. The recovered method uses the
existing Matrix and Vector operators and a signed 64-bit frame-delta value.
It preserves the original one-last-update behavior when the timer crosses
zero and still translates by the resulting vector when frame delta is zero.

`tools/run_player_fighter_push_smoke.py` compiles the production method at
`-O0` and `-O2` against recording matrix/vector services. It verifies an
inactive timer, all three rotation axes, matrix call order, translation
amount, zero and negative frame deltas, and the 64-bit positive boundary.
The native UCRT64 `gof2` library and all 208 NDK r18b ARM translation units
build. The other Fighter smokes and 93 verifier tests pass (one existing
skip). Focused source-shape/strict fuzzy improves from **79.1%/39.4%** to
**92.1%/57.9%**; `push` remains nonexact. The 44-function Fighter unit
retains its 21 linked-exact and 10 byte-exact pairs.

The full NDK r18b corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **94.03%/83.36%** mean source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**. There are zero skipped or
wrong-type symbols. The only changed source-shape result is `push`; an
identity-by-identity comparison finds no linked- or byte-exact losses or
gains. Full report SHA-256:
`1d66eccaa359dd8be3504b4531b4654d725e07143f274fe6b2cad6292eb96639`.

The production method was exercised with mock geometry/math services, not
inside a complete game frame. Its call path in the unresolved
`PF_update_body` and the resulting visible ship motion remain unverified.
