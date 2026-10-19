# Level player-entry host layout (2026-10-19)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Level::createPlayer` at `0xaf1bc`. ARM allocates 0x114 bytes for `Player`
at `0xaf236`, stores 1 to `Player+0x69` at `0xaf27a`, and allocates 0x3a0
bytes for `PlayerEgo` at `0xaf286`. The ARM-only class-size assertions
independently retain the two original sizes.

The same fixed allocation sizes are too small for the pointer-rich x64
host classes. `Level::createPlayer` now allocates both objects with `sizeof`.
The `+0x69` store now addresses the corresponding named byte of the
existing `empDisabled` storage rather than a raw byte offset, which would
hit a different field after the host pointer expands. Its exact gameplay
meaning is not established; the member deliberately remains `field_69`.
ARM-only `offsetof` checks lock the recovered byte at `+0x69`.

The host layout smoke checks that both host types exceed the ARM allocation
sizes, that the named byte moves with the type, and that the production
source uses size-based allocations and the member store. The existing
production `Player` constructor smoke passes. Native `gof2` and all
208 ARM translation units build. Focused `createPlayer` source-shape/strict
fuzzy remains 86.5%/37.8%, nonexact. Focused reports:
`A:/Galaxy on Fire new version/_work/level_create_player_before_20261017.json`
and `level_create_player_after_20261017.json`.

The full ARM report
`A:/Galaxy on Fire new version/_work/level_create_player_full_20261019.json`
compares 4,323/4,524 original functions in 4,730 rows, with 201 missing,
zero skipped and zero wrong-type. Mean source-shape/strict fuzzy remains
93.86%/83.04%; linked-exact/byte-exact remains 2,528/1,076. Pairwise
comparison with `level_stage0_alloc_full_20261017.json` found zero
linked- or byte-exact losses. The verifier unit suite passes 40 tests
(one existing skip).

This package does not execute the complete `PlayerEgo` constructor or the
weapon-position/equipment branch with real assets. The next integration
gate remains an uninterrupted resource-backed `Level::init` path through
`createSpace` and `createPlayer`, followed by stage 1 and the first frame.
