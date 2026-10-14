# PlayerEgo emergency-system activation (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::tryToStartEmergencySystem`
at `0xa106c–0xa10f0` in the 2.0.16 `libgof2hdaa.so`, independently
cross-checked against Android IDA lines 86377–86403. The original
returns zero when emergency geometry at `+0xac` is absent, the timer at
`+0x30c` is nonzero, or player hitpoints exceed one. On activation it
sets hitpoints to one, copies duration `+0x310` into timer `+0x30c`,
clears vulnerability, obtains the current ship twice through
`Globals::status`, removes its first equipment of sort 27, and plays
sound 1115 through `Globals::sound` with zero gain and null vectors.
It then returns one. This is the transition called by
`MGame::gameOverCheck` before game-over handling.

The previous C++ dispatched the sound through an unconnected null
`g_emerg_fmod` pointer and passed gain `1.0f`; either differed from the
ARM behavior, and the pointer could crash on successful activation.
The production code now uses `Globals::sound` and gain `0.0f`, while
retaining the original gate and equipment-removal order. The
extracted-production-method smoke covers all three refusal gates,
successful activation, equipment removal, sound ID/gain, and refusal
to reactivate. Native `gof2`, all 206 ARM translation units, the
docking/emergency, cloak and setShip smokes, and 37 verifier tests
(one skipped) pass.

Focused source-shape and strict fuzzy both improve **90.4%/79.2% →
100.0%/100.0%**; generated/original instruction counts change
**48/53 → 53/53**. The function becomes **linked-exact**, but not
byte-exact. Full report `_work/player_emergency_start_full_20261014.json`
compares **4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages remain **93.39%**
source-shape and **82.28%** strict fuzzy. Linked-exact rises
**2,482 → 2,483** with this function as the sole gained member and
no losses; byte-exact remains **1,065** with no gained or lost
members, relative to `_work/player_emergency_ctor_full_20261014.json`.

The smoke executes the extracted production method, not the live
ARM32 game. Runtime availability and ownership of the emergency
geometry across ship replacement or ego destruction remain
unverified. Further constructor/ownership recovery is a next bounded
priority; no cleanup unsupported by the original destructor was added.
