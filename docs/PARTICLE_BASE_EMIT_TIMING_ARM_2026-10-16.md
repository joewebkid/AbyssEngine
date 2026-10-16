# Base particle emission position and timing, Android ARM (2026-10-16)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA analysis of `_ZN15IParticleSystem4emitEi` at `0x181dc4` identifies
the speed-squared input in `S28` and the Newton inverse-square-root
factor initially in `S17` (`0x181fc4`–`0x181fd0`). The inverse-speed
estimate is `S26 = S17 * S2`. The inverse *travel* length is stored at
`0x181f18` and later passed as the second vector multiplier at
`0x1822e2`–`0x1822ee`, which places particles along the frame's path.
The C++ body had incorrectly used the inverse-speed estimate for this
position multiplier.

Crucially, `S17` is reused at `0x1822c4` or `0x1822cc` for the
emission step: traveled distance divided by count, or the configured
step for flag `0x10`. At `0x18268c`–`0x182698`, the original update
delay is `S26 * (S17 * (count - phase) * 1000)`, then capped by frame
delta. The first timing audit wrongly treated `S17` as the unchanged
Newton factor. This follow-up corrects that interpretation and uses
the retained emission step. It does not claim recovery of the whole
emitter.

The extracted-production-body smoke in `tools/run_particle_emit_smoke.py`
calls `IParticleSystem::emit` for 150 ms and 200 ms frames with the
same 10-unit/s speed and two emissions. It checks midpoint positions
and frame-proportional first-particle update times, then checks the
configured-step branch independently. These scenarios distinguish
both errors from the ARM behavior.

Validation: `python tools/run_particle_emit_smoke.py --cxx
C:/msys64/ucrt64/bin/g++.exe`, native `gof2` build, 206/206 ARM units,
and 40 verifier tests (one existing skip) pass. Focused ARM comparison
of `IParticleSystem::emit` is 88.5% source-shape and 16.7% strict fuzzy;
it remains nonexact. The strict fuzzy figure is down from 17.5% in the
prior report; semantic evidence takes precedence over that score.
Full ARM corpus: **4,323/4,524** original unique functions compared,
**201 missing**, **124 extras**, zero skipped, **93.62%** average
source-shape, **82.69%** average strict fuzzy, **2,507 linked-exact**
and **1,074 byte-exact**. Exact `(unit, symbol)` pair-set comparison
against `particle_emit_timing_full_20261016.json` shows zero losses
or gains. Focused and full reports are
`A:/Galaxy on Fire new version/_work/particle_emit_step_focused_20261016.json`
and `A:/Galaxy on Fire new version/_work/particle_emit_step_full_20261016.json`.

Further REA/ARM inspection confirms the random-call order in two branches.
Velocity spread draws three values at `0x1820da`–`0x182106`, followed by
the random phase at `0x18225a`. Flag `0x80` then draws three position
components at `0x182448`–`0x182468`; the ordinary branch instead draws
direction spread, X/Z spread and Y spread at `0x18239c`–`0x18240c`.
When lifetime randomization is enabled, three more draws occur at
`0x18249a`, `0x18251a` and `0x182530` for life and the two sizes.
The extracted production-body smoke now checks both branches against
an independently seeded `AERandom` sequence, including the final seed,
position, velocity and lifetime/size values. No production C++ changed
in this follow-up; the full corpus report above remains the report for
the same compiled source. A fresh focused report is
`A:/Galaxy on Fire new version/_work/particle_emit_random_focused_20261016.json`
and still reads 88.5% source-shape / 16.7% strict fuzzy. Native `gof2`,
206/206 ARM units and 40 verifier tests (one skip) remain green.

At that stage, the tests covered selected random cases, timing and ring
advancement, but not UV dispatch, repeated emission over many frames,
all particle-lifetime branches, GPU output or live ARM32 behavior.

The subsequent UV dispatch audit found a concrete flag mismatch in the
same production body. Android ARM loads the flags byte at `+0x37`,
shifts it left by 30 and calls `rotateUVs` at `0x1820bc` only when bit 1
is set (`0x02000000` in the full flags word). The base-emitter C++ had
instead tested `0x80000000`; the neighboring `emitManual` body already
used `0x02000000`. A production-body smoke failed on the intended bit
before the correction and now checks both the intended bit and the
unrelated high bit, including all four UV outputs. It also checks that
flags `0x80` and `0x40` suppress a second emission as indicated by ARM
at `0x181e22`–`0x181e2a` and `0x181f28`–`0x181f2c`.

Native `gof2`, 206/206 ARM units and 40 verifier tests (one skip) pass.
Focused `IParticleSystem::emit` moves from 88.5%/16.7% to
**88.8% source-shape / 19.9% strict fuzzy**, still nonexact.
`rotateUVs` itself remains **95.2% / 72.0%**, nonexact; this package
confirms its dispatch bit, not the independent equivalence of its
algorithm. Focused report:
`A:/Galaxy on Fire new version/_work/particle_emit_uv_focused_20261016.json`.
The corrected global corpus report is
`A:/Galaxy on Fire new version/_work/particle_emit_uv_full_20261016.json`.
It compares **4,323/4,524** original unique functions, with **201 missing**,
**124 extras**, zero skipped, **93.62%/82.69%** average
source-shape/strict fuzzy, **2,507 linked-exact** and **1,074 byte-exact**.
The exact `(unit, symbol)` pair sets have zero losses or gains against
the preceding full report.

Direct disassembly of the original `rotateUVs` at `0x182704`–`0x182778`
confirms signed extension of the integer seed (`0x182712`), one
`AERandom::nextInt(40000)` call (`0x182724`–`0x182728`), and UV selection
from the result's two low bits (`0x18272c`–`0x182760`). The source uses
the same four selectors and returns the destination pointer. A focused
host check derives those selectors independently from the ARM sequence
for 256 negative and nonnegative seeds; all outputs match. Native
`gof2`, 206/206 ARM units and 40 verifier tests (one skip) remain green.
No production C++ changed, so the full corpus and exact-pair counts
above still apply. The fresh focused report for `rotateUVs` is
`A:/Galaxy on Fire new version/_work/particle_rotate_uv_focused_20261016.json`:
**95.2% source-shape / 72.0% strict fuzzy**, nonexact. This confirms
the selector mapping, not an independent equivalence proof for
`AERandom::nextInt(int)` or live GPU texture coordinates. The bounded
RNG method is the next targeted audit.

## Repeated-frame rate and ring check

Original ARM at `0x181f62`–`0x181f92` computes the rate-mode emission
count from `rate * (emitTimer + delta) * 0.001` and stores the leftover
time; `0x1826c4`–`0x1826cc` advances the particle index and wraps at
`maxParticles`. The extracted production `IParticleSystem::emit` body
now has a four-frame host check with rate 5/s and deltas 100, 200,
100 and 200 ms. It verifies zero/one/one/one emissions, timer values
100/100/0/0 ms, and index progression 0/1/2/0 in a three-slot ring.
The test passes at host `-O0` and `-O2`. Native `gof2` and 40 verifier
tests (one existing skip) pass. The focused ARM comparison remains
**88.8% source-shape / 19.9% strict fuzzy**, nonexact; report:
`A:/Galaxy on Fire new version/_work/particle_emit_repeated_frame_focused_20261016.json`.
No production source changed, so the most recent full corpus report
`particle_emit_direct_split_full_20261016.json` still applies:
**4,323/4,524** originals compared, **201 missing**, **123 extras**,
**93.67%/82.77%** average source-shape/strict fuzzy, **2,513
linked-exact / 1,074 byte-exact**. This confirms these specific
static-ARM-derived state transitions in the host fixture, not general
frame scheduling, every particle lifetime case, GPU output or live
ARM32 behavior. Next audit: lifecycle and output branches not yet
covered by this fixture.
