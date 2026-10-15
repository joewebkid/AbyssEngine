# PlayerFighter update prelude — 2026-10-15

## Scope and Android evidence

The reference is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA reads of `PlayerFighter::update(int)` at `0xdcfd8` show an
approximately 3,556-instruction original body. The current C++ body
was only 81 ARM instructions before this package, so this is a
**prelude recovery**, not a claim to reconstruct all of `update`.

At `0xdcff8–0xdd03c`, state 4 with a finished explosion and either
no captured crate or a death timer above 60,000 ends in
`KIPlayer::setActive(false)`. The former source called an undefined
`PF_update_dead` helper. The stack guard seen in the disassembly is
compiler instrumentation, not source behavior; a null guard read in
the C++ body was removed.

At `0xdd044–0xdd080`, the original increments `+0x1b8/+0x1c0`,
stores the signed `dt` pair at `+0x1d0`, gets position from the
actual `AEGeometry` at `this+0x08`, and copies that Vector to
`this+0x2c`. The previous source called `getPosition` on a stack
buffer reinterpreted as geometry.

At `0xdd080–0xdd116`, the method updates Player bytes `+0x5c/+0x5d`:
for ship groups 8/9 they are enemy=1/friend=0; a wingman is
enemy=0/friend=1; otherwise they come from `Standing::isEnemy` and
`Standing::isFriend`. For the alternative `field_0x43` path, it
forces enemy=1 only when `damageDoneByPlayer` exceeds
`getMaxHitpoints()/20`. The former source updated only the first byte
and omitted this threshold path. The subsequent original branches
starting at `0xdd116` are not recovered here.

## Checks and limits

`python tools/run_player_fighter_update_prelude_smoke.py` compiles the
production method with recording dependencies and checks the dead
branch, signed dt, position, both allegiance bytes, standing calls and
the damage threshold. Both prior Fighter smokes and 37 verifier tests
(one skip) pass. Native `gof2` builds. ARM and full-corpus metrics are
recorded in `PROJECT_STATUS.md` and local reports in the parent `_work`
directory.

All 206 ARM translation units compile. Focused `update` source-shape
improves **3.6% → 5.3%** and strict fuzzy **2.2% → 3.7%**; it is
not exact. This low whole-method score is expected while most of the
original body is absent.

The remaining `PF_update_body` call is an unresolved placeholder for
most of the original method. The smoke stubs it; neither a full
Fighter update nor an original ARM32 runtime observation is proven.
Next priority is to map and recover a complete path after `0xdd116`,
including its external effects, before treating `update` as usable.
