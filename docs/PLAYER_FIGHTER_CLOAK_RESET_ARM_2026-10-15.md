# PlayerFighter cloak/reset ABI recovery — 2026-10-15

## Android evidence

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA reads of `reset` (`0xdc934–0xdca22`) show three
`Vector::operator=` calls with a zero Vector at Fighter `+0x90`,
`+0x164`, and `+0x170`. The prior source called a null function
pointer for all three. ARM also clears the mission-crate flag pair
at `+0x68/+0x69`, the crate-lost flag at `+0x6a`, and the cloak
timer/duration pair at `+0x2c8/+0x2cc`. It does not clear `+0x2c0`
in this method.

The independent `setCloakingPossible` at `0xdcd9c–0xdcdb8`
reads duration from `+0x2cc` and writes duration+1 to timer
`+0x2c8`. `handleCloaking` at `0xdcdbc` reads the same pair.
`cloak(int,bool)` at `0xdcd58–0xdcd94` stores active at `+0x2d0`,
duration at `+0x2cc` and its bool at `+0x2d9`; nonpositive durations
draw from `Globals::rnd->nextInt(5000)` before adding 5,000 and
4,000. The former C++ layout placed `cloakTimer` at `+0x2c0` and
`cloakDuration` at `+0x2c8`, so several methods addressed the wrong
words despite compiling.

## Correction and checks

The Fighter header now asserts ARM offsets for timer, duration,
active and cooldown. Constructor/reset assignments follow the
observed offsets; `reset` uses real Vector assignments and clears
both mission-crate bytes. `cloak` uses the observed bounded RNG.
`python tools/run_player_fighter_cloak_reset_smoke.py` compiles the
production `reset`, `cloak`, and `setCloakingPossible` methods with
recording dependencies. It checks reset state and vector effects,
the state-5 exception, positive and random durations, RNG bound,
and forced cloak termination.

Native `gof2`, all Fighter smokes and 37 verifier tests (one skip)
pass; ARM compilation is 206/206 translation units. The focused
source-shape/strict fuzzy pairs change as follows:

| Method | Before | After |
| --- | ---: | ---: |
| `reset` | 43.6% / 16.8% | 93.2% / 75.0% |
| `cloak` | 91.8% / 60.5% | 100% / 91.3% |
| `setCloakingPossible` | 96.0% / 80.0% | 100% / 100% |
| `handleCloaking` | 87.0% / 50.7% | 87.5% / 50.1% |

The full corpus compares 4,323/4,524 original functions at 93.54%
average source-shape and 82.57% strict fuzzy, with 201 missing, 123
extras and zero skipped. Its 2,499 linked-exact and 1,074 byte-exact
`(unit, symbol)` sets have zero losses or gains against the preceding
full report. None of the four methods above becomes exact. Local
reports in the parent `_work` directory are
`player_fighter_cloak_reset_focused_20261015.json` and
`player_fighter_cloak_reset_full_20261015.json`.

## Limits

The smoke verifies these extracted methods with mock dependencies,
not an original ARM32 run. `handleCloaking` itself retains other
unresolved material/animation behavior. Neither the opaque word at
`+0x2c0` nor all cloak visual effects are classified here. Next,
validate the material transition and emitter behavior on an
executable ARM32 path before claiming complete cloaking recovery.
