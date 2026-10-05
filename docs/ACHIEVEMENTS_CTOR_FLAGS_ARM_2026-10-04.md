# Achievement constructor flag initialization (2026-10-04)

The Android 2.0.16 constructor at `libgof2hdaa.so:0x1568d0..0x1568f2`
allocates two 180-byte medal arrays, then clears the three consecutive
flag bytes at object offsets `0x20`, `0x21` and `0x22`. The ARM stores are
`strh` at `0x20` and `strb` at `0x22`.

The recovered constructor already cleared `gotAllMedals_` (`0x20`) and
`gotAllSupernovaMedals_` (`0x22`), but left `gotAllGoldMedals_` (`0x21`)
uninitialized. That flag is read through `gotAllGoldMedals()` by mission
generation and the missions window. The one-line initialization restores
the Android constructor contract without changing object layout.

Focused ARM verification moves both constructor aliases C1/C2 from 97.1%
source-shape and 92.9% strict fuzzy, neither exact, to 100% source-shape,
100% strict fuzzy and linked-exact. They are two symbol rows for one unique
constructor body. Native `gof2` and 206/206 ARM translation units build.

The complete `--fail-on-wrong-type` corpus compares 4,322/4,524 unique
originals across 4,675 rows: 202 absent, zero wrong-signature and zero
skipped. Mean source-shape and strict fuzzy stay 90.09% and 77.22%; linked-
exact rises from 2,162 to 2,164, while raw-byte-exact stays 977. The exact
`(unit, symbol)` sets show no losses; the only gains are C1 and C2. The
focused and complete local reports are
`_work/blueprint-isunlocked/achievements_ctor_after.json` and
`achievements_ctor_full.json`.

No game runtime is claimed. A standalone native smoke for the complete
`Achievements.cpp` object could not link without other game-method shims, so
the runnable evidence here is the focused ARM comparison plus green native
and ARM builds. The next priority remains exercising representative medal
states in an integrated runtime fixture.
