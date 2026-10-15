# PlayerFighter ABI and setLevel recovery — 2026-10-15

## Evidence and scope

The reference is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA used its IDA adapter for named-function ARM instruction reads. The
`Level::createShip` allocation at `0xc06b6–0xc06f8` is `0x2f0` bytes.
`PlayerFighter::PlayerFighter` at `0xdc2f8` passes group `-1` to its
KIPlayer base constructor, writes its second argument at `this+0x28`,
constructs Matrix members at `+0x218` and `+0x258`, and uses fields through
`+0x2e8`. The final four allocated bytes are not yet classified.

`PlayerFighter::setLevel` at `0xdca30–0xdcb08` calls `KIPlayer::setLevel`,
obtains `AEGeometry::getReferenceMatrix`, then registers particle sets
9, 15, 42, 17 and 18 through Level managers at `+0x74`, `+0x78`,
`+0x84`, `+0x8c`, `+0x8c`. It stores handles at Fighter `+0x1a0`,
`+0x80`, `+0x84`, `+0x134`, `+0x138` respectively, disabling emission
after each registration. Other named ARM reads, including `awake`,
`handleCloaking`, `initPush`, `reset` and `revive`, corroborate the inherited
KIPlayer field accesses. These reads are static evidence, not a live
behavioral observation.

## Recovery

The former Fighter declaration duplicated KIPlayer storage and was
`0x368` bytes on ARM. The header now uses aliases for existing inherited
fields, begins derived storage with `Explosion*` at `+0x124`, places the
two Matrix members at their observed offsets, and asserts the ARM type
size and critical field offsets at compile time. The constructor passes
the observed base group and assigns its own group at `+0x28`.
`setLevel` now makes five typed `ParticleSystemManager` calls rather than
calling null placeholders. These changes also recover several small
Fighter methods whose previous accesses referred to shadow fields.

## Checks and metrics

- Native `gof2` build: pass.
- ARM verifier build: 206 translation units compiled, zero failed.
- `python tools/run_player_fighter_set_level_smoke.py`: pass. This compiles
  the production `setLevel` body with recording managers and checks manager
  choice, set IDs, matrix, handles, and disabled emission.
- Verifier unit tests: 37 passed, one skipped.
- Focused `setLevel` source-shape/strict fuzzy: **6.6%/0.0% → 94.8%/82.0%**.
  Constructor source-shape: **82.8% → 85.2%**. The focused Fighter family
  contains 37 compared symbols, of which 16 are linked-exact and 10
  byte-exact.
- Full corpus: **4,323/4,524** original functions compared; 201 missing
  (all absent, none wrong-type), 123 extras, zero skipped. Mean
  source-shape/strict fuzzy **93.47%/82.37% → 93.52%/82.55%**. Exact sets
  **2,486 → 2,499 linked-exact**, **1,065 → 1,074 byte-exact**. Comparing
  `(unit, symbol)` pairs shows 13 and 9 gains and **zero losses**.

Reports: `_work/player_fighter_focused_20261015.json` and
`_work/player_fighter_full_20261015.json` in the workspace. These are
generated validation data, not source files.

## Remaining uncertainty

The smoke verifies this production method in a mock environment, not the
original Android binary or the complete Fighter object lifecycle. At this
stage the constructor still had unresolved null guard and route
placeholders; [the subsequent route recovery](PLAYER_FIGHTER_ROUTE_ARM_2026-10-15.md)
removes them. Its `Level::createShip` path still must not be described as
runtime-ready. The
allocated tail at `+0x2ec` has no established meaning. Next, recover the
constructor's external dependencies and ownership/initialization sequence,
then execute a narrow ARM32 object-lifecycle smoke with known inputs.
Further source-shape gains must not override observed behavior or ABI.
