# PlayerFighter roll recovery (Android ARM, 2026-10-20)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::roll(int)` at `0xdf860–0xdfa50`. The ARM literal pool at
`0xdfa14–0xdfa48` contains the 0.015 level threshold and signed rates
0.0002, 0.00025, 0.00035 and 0.00075. Its branches use ship-matrix floats
at `+0x10/+0x14`, cap the frame amount at 60, and update Fighter bytes
`+0x100/+0x254/+0x255`. The independently recovered IDA listing and the
separate `PlayerEgo::roll` implementation corroborate the broad control
path. Fighter's negative-X/zero-Y branch was read directly from its own ARM
instructions rather than inferred from PlayerEgo.

The prior Fighter source dereferenced a null stack-guard placeholder, set
every rate constant to zero, and passed the resulting angle as X rotation
through declared-only matrix shims. The recovered method uses the real
`MatrixIdentity`/`MatrixSetRotation` return-and-assignment contract, with
zero X/Y and `rate * angle` as Z rotation. It retains the original NaN
checks, direction reversal, autoleveling and exact threshold behavior.

`tools/run_player_fighter_roll_smoke.py` compiles the production method with
recording matrix services at `-O0` and `-O2`. It checks the disabled path,
level reset, frame cap, positive/negative and inverted banks, zero-Y and
threshold boundaries, both direction reversals, autolevel continuation and
NaN handling. The native UCRT64 `gof2` library, 208 ARM translation units,
the other Fighter smokes, and 93 verifier tests (one existing skip) pass.
Focused NDK r18b source-shape/strict fuzzy improves from **67.9%/52.7%**
to **91.2%/67.3%**. The function remains nonexact.

The full NDK r18b corpus compares **4,323/4,524** original functions across
**4,735** pairs, with **94.02%/83.36%** mean source-shape/strict fuzzy,
**2,557 linked-exact** and **1,082 byte-exact**. There are zero skipped and
wrong-type symbols. An identity-by-identity comparison against the prior
corpus finds no linked- or byte-exact losses or gains; `roll` is the only
changed source-shape result. Full report SHA-256:
`ad36dcb3fcfbab278dc7e1e17b0b2cc3364ca33ea5c58b8988644c4fa8dc2f98`.

No full game frame or visual roll motion was executed. The larger
`PlayerFighter::update` body currently stops at an unresolved
`PF_update_body` call, so it does not yet drive this recovered method in a
continuous game frame. That integration and the cumulative effect of rolling
on the game camera remain separate verification priorities.
