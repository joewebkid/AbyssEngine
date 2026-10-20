# PlayerFighter setPosition host-pointer ABI recovery (2026-10-20)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::setPosition(float,float,float)` at `0xdcbb8`. The ARM body
stores Fighter position at `+0x58/+0x5c/+0x60`, updates geometry and the
working vector, conditionally resets the trail, then assigns the geometry
matrix into the associated Player matrix. The independently recovered IDA
listing agrees with that order. `PlayerFighter::setPosition(const Vector&)`
delegates to this overload; Level and LevelScript call the setter directly.

The old source narrowed the address returned by `AEGeometry::getMatrix()`
to `int`, reconstructed a pointer from that 32-bit value, and passed it to a
declared-only `AEMath_MatrixAssign` shim. That was ABI-safe only by accident
on 32-bit builds and could address invalid memory on the native 64-bit host.
The method now invokes the actual `Matrix::operator=` through typed Player
and geometry members. It does not add a host-only branch or change the ARM
layout.

`tools/run_player_fighter_set_position_smoke.py` compiles the production
method at `-O0` and `-O2`. It checks geometry/trail/matrix call order, both
trail branches, all three positions, and all 15 matrix elements. The native
UCRT64 `gof2` library and all 208 NDK r18b ARM translation units build; 93
verifier tests pass with one existing skip. Focused ARM comparison remains
**100% source-shape, 100% strict fuzzy and linked-exact** at 39/39
instructions. The 44-function Fighter unit retains its 21 linked-exact and
10 byte-exact pairs.

The full NDK r18b report is byte-identical to the preceding `roll` report:
**4,323/4,524** original functions across **4,735** pairs,
**94.02%/83.36%** mean source-shape/strict fuzzy, **2,557 linked-exact**,
**1,082 byte-exact**, and zero skipped or wrong-type symbols. No exact pair
was gained or lost. Report SHA-256:
`ad36dcb3fcfbab278dc7e1e17b0b2cc3364ca33ea5c58b8988644c4fa8dc2f98`.

This is an ABI and local-behavior check, not a full LevelScript playthrough.
The separate `PlayerFighter::push` still has pointer-width casts; it needs
its own ARM and runtime audit. The larger `PF_update_body` remains unresolved.
