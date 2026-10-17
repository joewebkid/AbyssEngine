# Level weapon-position cleanup (2026-10-17)

Android 2.0.16 ARM `Level::createPlayer()` at `0xaf634..0xaf67c`
checks the outer weapon-position array, calls
`ArrayReleaseClasses<Vector*>` on each nonnull inner slot, destroys
and deletes the inner array, clears its outer slot, then calls
`ArrayReleaseClasses<Array<Vector*>*>` before destroying and deleting
the outer array. The recovered code manually deleted vectors and
inner arrays but omitted the outer release call. This package restores
the observed sequence in `Level.cpp`.

`Gun::setOffset` and `PlayerEgo::setTurretPosition` copy coordinate
values, so the subsequent array cleanup does not invalidate pointers
they retain. The extracted production-fragment smoke test exercises
null outer input, two populated slots (including a type-3 pair) and
two empty slots at host `-O0/-O2`; all three vectors are destroyed
exactly once. Native `gof2`, **208/208** ARM units and 40 verifier
tests (one existing skip) pass.

Focused `Level::createPlayer` source-shape/strict fuzzy rises from
**86.2%/37.2%** to **86.5%/37.8%**, still nonexact. The reports are
`A:/Galaxy on Fire new version/_work/level_weapon_cleanup_before_20261017.json`
and
`A:/Galaxy on Fire new version/_work/level_weapon_cleanup_after_20261017.json`.

The full ARM corpus compares **4,323/4,524** originals in **4,723**
comparisons, with **201 missing** and **123 extras**. Rounded average
source-shape/strict fuzzy is **93.74%/82.88%**. The exact-pair audit
against `filread_weapon_positions_lifetime_full_20261017.json` finds
zero losses and two new linked-exact specializations:
`ArrayReleaseClasses<Vector*>` and
`ArrayReleaseClasses<Array<Vector*>*>` in the Level unit. Totals are
**2,520 linked-exact** and **1,076 byte-exact**. The report is at
`A:/Galaxy on Fire new version/_work/level_weapon_cleanup_full_20261017.json`.

Unverified: an asset-backed running-game path and non-market ship
changes. The original caller dereferences the loader result before
this cleanup, so the observed null check is not evidence that a
missing `weapons_hd.bin` is safe at the earlier use sites.
