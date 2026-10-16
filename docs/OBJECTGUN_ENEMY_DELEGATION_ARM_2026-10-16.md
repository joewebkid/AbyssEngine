# ObjectGun enemy delegation (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

`ObjectGun::setEnemies(Array<Player*>*)` at `0x15f95c` loads the
`Gun*` field at offset `+0x8` and tail-branches through the Thumb/ARM
veneer at `0x19c628` to PLT `0x62e8c`, identified as
`Gun::setEnemies(Array<Player*>*)`. The original forwards the array
pointer unchanged, including when it is null. The previous source
dereferenced the array and called an undefined free helper; that
would fail on a null argument. The typed direct call now matches the
observed behavior and ABI.

`ObjectGun::setEnemy(Player*)` at `0x15f962` and
`ObjectGun::translate(const Vector&)` at `0x160358` each consist of
`bx lr`. Their existing empty bodies are intentional and remain
byte-exact; no behavior was added to them.

The extracted production method in
`tools/run_objectgun_canvas_calls_smoke.py` forwards both a nonnull
array and `nullptr` to a recording `Gun` under host `-O0` and `-O2`.
Focused `setEnemies` source-shape/strict fuzzy rises
**90.0%/50.0% -> 100%/100%**, becoming linked-exact but not
byte-exact. The two no-op methods remain byte-exact.

Native `gof2`, all **207/207** ARM translation units and **40**
verifier tests (one existing skip) pass. The full corpus compares
**4,323/4,524** original functions, with **201 missing**, **124
extras**, zero skipped or wrong signatures, **93.67%/82.77%**
average source-shape/strict fuzzy, **2,511 linked-exact / 1,074
byte-exact**. Exact `(unit,symbol)` sets against the preceding full
report show one linked gain (`setEnemies`) and zero linked/byte
losses. Reports:
`A:/Galaxy on Fire new version/_work/objectgun_update_math_full_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_setenemies_before_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_setenemies_focused_20261016.json`,
`A:/Galaxy on Fire new version/_work/objectgun_noops_focused_20261016.json`,
and `A:/Galaxy on Fire new version/_work/objectgun_setenemies_full_20261016.json`.

This establishes pointer forwarding, not the behavior of
`Gun::setEnemies` consumers or live game AI.
