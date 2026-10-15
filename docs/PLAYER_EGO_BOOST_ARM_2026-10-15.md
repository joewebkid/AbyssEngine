# PlayerEgo boost setup and activation (2026-10-15)

Primary evidence is Android Thumb in the 2.0.16
`libgof2hdaa.so`: the `PlayerEgo` constructor clears the boost
availability byte at `0x999b8` and the control-gun gate at
`0x99a6e`, then derives boost delay/speed/time, availability and
sound resource at `0x99b20–0x99bbc`. `PlayerEgo::boost` at
`0x9aea4–0x9aefa` checks the active flag, availability, control-gun
gate and nonnegative timer before writing the boosted speed and
calling `FModSound::play(Globals::sound, boostSoundId, null, null,
0.0f)`. Android IDA independently confirms both paths.

The constructor chooses sound 38–41 for equipment sort 14 with
item indexes 71–74, or sound 1102 for index 195. The sound ID is
a 32-bit integer at `PlayerEgo+0xd4`, not an owned pointer. Unknown
item indexes retain the initial ID zero, which the original still
registers. Availability is exactly `getBoostDelay() > 0`.

The former C++ left the availability gate, control-gun gate and
boost sound ID uninitialized. Its `boost()` then dereferenced an
unconnected FMOD pointer and passed boost speed as the audio gain.
The recovered code initializes the confirmed defaults, registers
the item-specific sound, and dispatches through `Globals::sound`
at gain zero. An ARM-only layout assertion protects the sound-ID
offset. The extracted-production smoke checks defaults, disabled
boost, all five known sound mappings, the unknown-index fallback,
the blocking gates, and successful activation. Native `gof2`, all
206 ARM translation units, the new and adjacent docking/cloak/
supernova smokes, and 37 verifier tests (one skipped) pass.

Focused constructor source-shape/strict fuzzy rises **68.2%/41.3%
→ 73.9%/41.6%** for both C1/C2 aliases; generated/original
instruction counts change **408/604 → 450/604**. The constructor
remains non-exact. Focused `boost()` rises **90.1%/67.8% →
100.0%/100.0%**, with **32/32** instructions and a new
linked-exact, but not byte-exact, match.

Full report `_work/player_boost_full_20261015.json` compares
**4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages rise **93.40% →
93.41%** source-shape and **82.29% → 82.30%** strict fuzzy.
Linked-exact rises **2,483 → 2,484**, gaining only `boost()` and
losing none; byte-exact remains **1,065** with no gained or lost
members versus `_work/player_cloak_ctor_full_20261014.json`.

The smoke extracts the constructor segment and complete `boost()`
method; it does not exercise a live ARM32 game or audio backend.
Sound resources and boost timing in an actual flight remain
unverified. Jump-drive charge/equipment initialization is the next
bounded priority and was not folded into this boost package.
