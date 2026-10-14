# PlayerEgo engine-sound dispatch (2026-10-14)

Primary evidence is Android Thumb `PlayerEgo::PlayEngineSound` at
`0x9a550–0x9a5d0` and `PlayerEgo::StopEngineSound` at
`0x9a5ec–0x9a676` in the 2.0.16 `libgof2hdaa.so`. Android IDA
lines 80940–81023 independently show the same calls and conditions.
Both methods test equipment sort 38, non-alien orbit, positive
gamma-ray damage interpreted from the soft-float return bits, and
a valid sound ID at `PlayerEgo+0xb4`. `StopEngineSound` skips this
branch when docked at docking point 1. When enabled, both call
`FModSound::play(Globals::sound, engineSoundId, null, null, 0.0f)`.
The `PlayEngineSound` tail additionally passes the ship's sound
resource at `PlayerEgo+0x1c` and a null vector to
`Player::PlayEngineSound`; `StopEngineSound` calls the corresponding
player stop method. The sound resource is populated during ego
construction and consumed by `Player::PlayEngineSound`.

The previous C++ called FMOD through an unconnected null
`g_engine_fmod` pointer in both methods, so the positive-gamma
branch could crash. It also passed zero instead of the `+0x1c`
resource to the player's engine start. The code now uses
`Globals::status` and `Globals::sound` from the ARM call path,
passes the original resource, and keeps the original gates and
tail calls. No sound-stop substitution was made: the ARM stop
method itself calls `FModSound::play` before stopping the player
event.

An extracted-production-method smoke covers absent equipment,
alien orbit, zero damage, invalid sound ID, successful play,
docking-point suppression, and both player tail calls. Native
`gof2`, all 206 ARM translation units, the new and adjacent
docking/emergency smokes, and 37 verifier tests (one skipped) pass.

| Method | Source-shape before → after | Strict fuzzy before → after | Instructions generated/original after |
| --- | ---: | ---: | ---: |
| `PlayEngineSound` | 94.0% → 98.5% | 75.5% → 87.4% | 51/52 |
| `StopEngineSound` | 94.7% → 98.2% | 76.9% → 86.2% | 54/55 |

Neither method is linked- or byte-exact. Full report
`_work/player_engine_sound_full_20261014.json` compares
**4,323/4,524** original functions with **201 missing**,
**123 extras** and zero skipped. Corpus averages are **93.39%**
source-shape and **82.29%** strict fuzzy. The **2,483 linked-exact**
and **1,065 byte-exact** pair sets gain and lose no members versus
`_work/player_emergency_start_full_20261014.json`.

The smoke executes the extracted production methods, not a live
ARM32 audio backend. Correct FMOD resource availability and audible
behavior remain unverified. The unresolved emergency-geometry
ownership and remaining constructor state are next bounded
priorities; no unsupported destructor cleanup was added.
