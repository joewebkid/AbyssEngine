# Project Status

Last reviewed: 2026-10-16 (CutScene update dispatch; older subsystem
snapshots retain their dates)

## CutScene update dispatch (2026-10-16)

Both original Android ARM `CutScene::update` overloads tail-forward
to `process(int)`. The recovered no-argument wrapper instead called
`Level::update` and skipped cut-scene processing; both wrappers now
dispatch correctly. Focused `update()` source-shape/strict fuzzy rises
from **1.7%/0.0% to 100%/100%**; `update(int)` remains **100%/100%**.
Neither is linked- or byte-exact. Native `gof2`, **208/208** ARM units
and 40 verifier tests (one existing skip) pass. The fresh full corpus
is **4,323/4,524** compared, **201 missing**, **123 extras**,
**93.72%/82.82%** average source-shape/strict fuzzy, **2,515
linked-exact / 1,075 byte-exact**, with zero exact-pair losses.
Live ARM32 cut-scene timing remains untested. Next priority is a
focused `render3D` audit. See
[the dispatch audit](CUTSCENE_UPDATE_DISPATCH_ARM_2026-10-16.md).

## PlayerEgo position overload (2026-10-16)

Android ARM `PlayerEgo::setPosition(Vector)` tail-forwards to its
three-float overload; the recovered direct geometry call bypassed that
route. The one-line correction raises focused source-shape/strict
fuzzy from **1.1%/0.0% to 100%/100%** at one instruction, though it
is not linked- or byte-exact. Native `gof2`, **208/208** ARM units and
40 verifier tests (one existing skip) pass. The fresh full corpus is
**4,323/4,524** compared, **201 missing**, **123 extras**,
**93.70%/82.80%** average source-shape/strict fuzzy, **2,515
linked-exact / 1,075 byte-exact**, with zero exact-pair losses.
Live ARM32 gameplay remains untested. See
[the overload audit](PLAYER_EGO_POSITION_OVERLOAD_ARM_2026-10-16.md).
Next priority: source-backed audit of the separate `CutScene::update()`
one-instruction call route and its timing semantics.

## Particle emitter argument ABI (2026-10-16)

Android ARM `emit` and both `emitManual` call paths pass lifetime as
the second `setParticle` argument and separate start/end sizes as the
ninth/tenth arguments. The extracted production callers pass host
smoke tests; two fixture overrides were updated to the already-corrected
`void init` interface. No production code changed. Native `gof2` and
40 verifier tests (one existing skip) pass. The prior **208/208** ARM
build and full corpus remain applicable: **4,323/4,524** compared,
**201 missing**, **123 extras**, **93.68%/82.78%** average
source-shape/strict fuzzy and **2,515 linked-exact / 1,075 byte-exact**.
The unusual original sprite use of lifetime as initial SetSize is
confirmed at the call boundary but not explained by live ARM32/GPU
behavior. See [the argument audit](PARTICLE_EMIT_ARGUMENT_ABI_ARM_2026-10-16.md).

## Sprite spawn size argument (2026-10-16)

Android ARM `ParticleSystemSprite::setParticle` converts its `r2`
second argument (`p2`) for SetSize, not later stack argument `p9`.
The old source used `p9`; an extracted-body `-O0` test failed with
different values and now passes at `-O0/-O2` using `p2`. Position,
UV, RGBA, clear-color and 16-bit id output are also checked. Native
`gof2`, **208/208** ARM units and 40 verifier tests (one existing
skip) pass. Focused source-shape/strict fuzzy declines from
**90.8%/43.5% → 89.1%/38.4%**, nonexact; the semantic correction
is kept rather than inflating a code-shape metric with the wrong
input. Live ARM32/GPU output remains unverified; the caller argument
mapping is now covered by the
[argument audit](PARTICLE_EMIT_ARGUMENT_ABI_ARM_2026-10-16.md). The full
corpus remains **4,323/4,524** compared,
**201 missing**, **123 extras**, **93.68%/82.78%** averages,
**2,515 linked-exact / 1,075 byte-exact**, with zero exact-pair
losses against the previous report. See
[the spawn audit](PARTICLE_SPRITE_SPAWN_SIZE_ARM_2026-10-16.md).

## Sprite particle area-exit behavior (2026-10-16)

The extracted production `updateAreaExitParticle` and `setAlpha`
bodies now have `-O0/-O2` recording-canvas checks for every normal
radius band, exact outer/middle boundaries, rejected and accepted
rebirth candidates, position and base-velocity output. These match the
Android ARM branch/formula evidence; no production C++ changed. The
fixture, native `gof2` and 40 verifier tests (one existing skip) pass.
Focused source-shape/strict fuzzy remains **94.6%/66.7%**, nonexact.
The unchanged full corpus is **4,323/4,524** compared, **201
missing**, **123 extras**, **93.68%/82.78%** averages, **2,515
linked-exact / 1,075 byte-exact**; the prior **208/208** ARM build
still applies. Real RNG behavior and live ARM32/GPU output remain
unverified. See [the area-exit audit](PARTICLE_SPRITE_AREA_EXIT_ARM_2026-10-16.md).

## Sprite particle update output (2026-10-16)

The extracted production `ParticleSystemSprite::updateSingle` body now
has host `-O0/-O2` recording-canvas checks for exact/expired lifetime,
clear-position and size, size/color/UV/movement call order, frame
transition and area-exit dispatch. These agree with the Android ARM
branches; no production C++ changed. The fixture, native `gof2` and
40 verifier tests (one existing skip) pass. Focused source-shape/
strict fuzzy stays **92.9%/49.0%**, nonexact. The unchanged full
corpus remains **4,323/4,524** compared, **201 missing**, **123
extras**, **93.68%/82.78%** averages, **2,515 linked-exact / 1,075
byte-exact**; the prior **208/208** ARM build still applies. Actual
area-exit behavior and live ARM32/GPU output remain unverified. See
[the update audit](PARTICLE_SPRITE_UPDATE_OUTPUT_ARM_2026-10-16.md).

## Particle init return ABI and sprite alpha output (2026-10-16)

Both Android ARM particle `init` bodies end in a tail-call to virtual
`reset`, without producing an integer result; their manager callers
ignore any result. The shared declaration and both overrides now use
`void`. ARM **208/208** units, native `gof2`, 40 verifier tests (one
existing skip), and `-O0/-O2` sprite/mesh output fixtures pass.
Sprite init improves from **57.0%/62.5%** source-shape/strict fuzzy
to **100%/100% byte-exact**; mesh init improves from **97.6%/97.9%**
to **100%/100% linked-exact**. `ParticleSystemSprite::setAlpha` has a
new extracted-body RGBA/index test, remains **80.6%/46.9%** nonexact,
and needs no evidenced behavior change. The full corpus is now
**4,323/4,524** compared, **201 missing**, **123 extras**,
**93.68%/82.78%** average source-shape/strict fuzzy, **2,515
linked-exact / 1,075 byte-exact**. Exact-pair comparison against the
previous full report shows two linked gains, one byte gain and zero
losses. Live ARM32/GPU output and an
independent iOS/PC return-type declaration remain unverified. See
[the init/alpha audit](PARTICLE_INIT_VOID_SPRITE_ALPHA_ARM_2026-10-16.md).

## Base particle color interpolation (2026-10-16)

Android ARM `IParticleSystem::interpolateColor` confirms the `0xa0`
set stride, color/fade offsets, upper-only age-fraction cap and the
distinct `alphaFade` channel groups. The mesh-trail smoke fixture now
executes the real production interpolator at host `-O0/-O2` for
half-life, over-life, both fades, second-set selection and adjacent
trail propagation. No production C++ changed. Native `gof2` and 40
verifier tests (one existing skip) pass. Focused source-shape/strict
fuzzy is **91.4%/42.8%**, nonexact. A fresh full ARM comparison is
**4,323/4,524** compared, **93.67%/82.77%** averages, **2,513
linked-exact / 1,074 byte-exact**, with zero exact-pair changes from
the previous report; the prior **208/208** ARM build remains valid.
Live ARM32/GPU output and sprite
render propagation remain unverified. See
[the interpolation audit](PARTICLE_INTERPOLATE_COLOR_ARM_2026-10-16.md).

## ParticleSystemMesh color and expiry output (2026-10-16)

The extracted production `updateSingleColor` and `updateSingle` bodies
now have host `-O0/-O2` canvas-output checks for trail start masks,
neighbor/wrap coloring, color-before-clear order and lifetime boundary.
The test, native `gof2` build and 40 verifier tests (one existing skip)
pass; no production C++ changed. Fresh focused ARM scores remain
**97.2%/70.5%** for `updateSingle` and **99.8%/74.6%** for
`updateSingleColor` (source-shape/strict fuzzy), neither exact. The
previously verified **208/208** ARM build and full **4,323/4,524**
corpus remain applicable: **93.67%/82.77%** averages, **2,513
linked-exact / 1,074 byte-exact**. Interpolation arithmetic and live
ARM32/GPU output are still unverified. See
[the output audit](PARTICLE_MESH_COLOR_EXPIRY_OUTPUT_ARM_2026-10-16.md).

## ParticleSystemMesh trail-edge Y sign (2026-10-16)

Android ARM negates X/Y/Z for each negative trail-edge movement;
the source had left Y positive in both calls. A production-body
recording-canvas test failed before the fix and passes at host
`-O0/-O2` afterward. Native `gof2`, **208/208** ARM units and
**40** verifier tests (one existing skip) pass. Focused
`updateTrailEdges` source-shape/strict fuzzy improves
**96.1%/67.2% → 96.8%/73.4%**, nonexact. Full corpus remains
**4,323/4,524** compared, **201 missing**, **123 extras**,
**93.67%/82.77%** average source-shape/strict fuzzy, **2,513
linked-exact / 1,074 byte-exact** with no exact-set losses.
Live GPU/ARM32 output remains unverified. See
[the edge audit](PARTICLE_MESH_TRAIL_EDGE_SIGN_ARM_2026-10-16.md).

## ParticleSystemMesh dormant trail sentinel (2026-10-16)

Android ARM `updateSingle` returns after trail-edge handling when age
is `-2` and no new section has begun. The prior source incorrectly
advanced age and color. An extracted-production test failed before
the fix and now passes at host `-O0/-O2`, including the resumed and
ongoing branches. Native `gof2`, **208/208** ARM units and **40**
verifier tests (one existing skip) pass. Focused source-shape stays
**97.2%**; strict fuzzy rises
**69.2% → 70.5%**, nonexact. Full ARM corpus remains **4,323/4,524**
compared, **201 missing**, **123 extras**, **93.67%/82.77%** average
source-shape/strict fuzzy, **2,513 linked-exact / 1,074 byte-exact**,
with no exact-set losses. Other trail lifetime/output branches and
live ARM32/GPU behavior remain unverified. See
[the sentinel audit](PARTICLE_MESH_SENTINEL_UPDATE_ARM_2026-10-16.md).

## Base particle repeated-frame rate and ring check (2026-10-16)

Android ARM rate-mode arithmetic and ring-wrap instructions now have
a four-frame check against the extracted production `IParticleSystem::emit`
body. The expected zero/one/one/one emissions, retained timer values
and 0/1/2/0 ring indices pass at host `-O0/-O2`. No production C++
changed; native `gof2` and **40** verifier tests (one existing skip)
pass. Focused `emit` remains **88.8%/19.9%** source-shape/strict fuzzy,
nonexact. The unchanged full report covers **4,323/4,524** originals,
**201 missing**, **123 extras**, **93.67%/82.77%** average
source-shape/strict fuzzy, **2,513 linked-exact / 1,074 byte-exact**.
Other lifetime/output branches and live ARM32/GPU behavior remain
unverified. See [the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## ParticleSystemMesh direct trail dispatch (2026-10-16)

The trail branch of `ParticleSystemMesh::emit` now targets the original
`emitTrail` method directly. The method body lives in a separate ARM
translation unit to preserve the caller's linked-exact relocation; the
extra forwarding symbol was removed. Native `gof2`, **208/208** ARM
units, the particle trail fixture and **40** verifier tests (one existing
skip) pass. Full corpus remains **4,323/4,524** compared, **201 missing**,
**93.67%/82.77%** average source-shape/strict fuzzy and **2,513
linked-exact / 1,074 byte-exact** with zero exact-set losses. Extra
symbols decline **124 → 123**. Trail and base emitter behavior remain
partially unverified; next audit their state and lifetime branches. See
[the dispatch audit](PARTICLE_MESH_DIRECT_TRAIL_DISPATCH_ARM_2026-10-16.md).

## ObjectGun geometry offsets (2026-10-16)

Android ARM `ObjectGun::update` subtracts 100 from the owner gun's
Z offset and selects turret Z offsets of 205 (default), 260, 190,
200, 172, 170, 147 or 150 by item index. Its X/Y branches use
the ARM literals ±45, ±35 and ±16.5, plus the existing ±20 and
±13 cases; the X-enable condition reads only the byte at
`Gun+0xa4`. The previous C++ used smaller, incorrect offsets and
tested the four-byte flag word. Extracted production fragments
pass at host `-O0/-O2`. Native `gof2`, **207/207** ARM units and
**40** verifier tests (one existing skip) pass. Focused `update`
source-shape/strict fuzzy changes **87.9%/31.0% -> 90.8%/30.4%**;
it remains nonexact. Full corpus: **4,323/4,524** compared,
**201 missing**, **124 extras**, no skipped or wrong signatures,
**93.67%/82.77%** average source-shape/strict fuzzy, **2,513
linked-exact / 1,074 byte-exact**. Both exact sets are unchanged.
Live ARM32 placement remains unverified. Next priority is other
first-party behavior/ABI gaps, including the extra-symbol set. See
[the offset audit](OBJECTGUN_UPDATE_OFFSETS_ARM_2026-10-16.md).

## Per-function missing-symbol triage (2026-10-16)

All **201** names listed as missing by the current per-function ARM
verifier are external library/runtime symbols: **74 ZIP**, **47
zlib**, **23 OpenSSL/crypto** and **57 C++ ABI/runtime**. Each name
is defined exactly in the existing corresponding ARM archive; none
is an absent first-party game/engine name in this list. These
archives are excluded from per-function comparison and belong to
the separate whole-library relink gate. The published metrics remain
**4,323/4,524** compared, **201 missing**, **124 extras**, no skipped
or wrong signatures, **93.67%/82.77%** average source-shape/strict
fuzzy, **2,513 linked-exact / 1,074 byte-exact**. Archive symbol
presence does not prove link success, byte identity or behavior.
Next priority is first-party semantics/ABI and the extra-symbol
audit, not placeholder bodies for dependency functions. See
[the missing-symbol triage](MISSING_SYMBOLS_ARCHIVE_TRIAGE_2026-10-16.md).

## ObjectGun render gating and animation (2026-10-16)

Android ARM `ObjectGun::render` uses `50000.0f` as the inactive
position sentinel, divides two lifetime scaling paths by `1000.0f`,
passes `(0, PI, spinAngle)` to turret rotation, crosses direction
with side in that order, and advances mine wobble by
`deltaTime * 0.0005f`. The previous C++ used a different sentinel,
divisor, rotation axis, cross order and wobble rate; these are now
restored. Extracted production fragments pass under host `-O0/-O2`.
Native `gof2`, **207/207** ARM units and **40** verifier tests (one
existing skip) pass. Focused `render` source-shape/strict fuzzy
changes **89.6%/39.7% -> 89.7%/41.5%**, still nonexact. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**, no skipped
or wrong signatures, **93.67%/82.77%** average source-shape/strict
fuzzy, **2,513 linked-exact / 1,074 byte-exact**. Both exact sets
are unchanged. Live ARM32 frame/GPU behavior remains unverified;
next priority is remaining render effects and missing-function triage.
See [the render audit](OBJECTGUN_RENDER_SEMANTICS_ARM_2026-10-16.md).

## ObjectGun visible orientation axes (2026-10-16)

Android ARM confirms that the visible-gun branch copies camera
matrix indices `0,1,4,5,8,9` into the corresponding orientation
entries. The C++ incorrectly copied camera `10,11` into orientation
`8,9`; it now uses `8,9`. An extracted production-fragment fixture
checks distinct camera values and negated muzzle components under
host `-O0/-O2`. Native `gof2`, **207/207** ARM units and **40**
verifier tests (one existing skip) pass. Focused `render` remains
**89.6%/39.7%** source-shape/strict fuzzy and nonexact. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**, no skipped
or wrong signatures, **93.67%/82.77%** average source-shape/strict
fuzzy, **2,513 linked-exact / 1,074 byte-exact**. The exact sets
show zero gains and zero losses. Whole-frame and live ARM32 GPU
behavior remain unverified. See
[the orientation audit](OBJECTGUN_VISIBLE_ORIENTATION_ARM_2026-10-16.md).

## ObjectGun scatter explosion flow (2026-10-16)

Android ARM and extracted production fragments confirm the
scatter-gun hit/ready gating, one-time explosion start with a zero
direction, per-frame update with a null camera, completed-hit flag
reset, and hit-gated rendering. The host fixture passes under
`-O0/-O2`. No production C++ or ARM object changed. Focused
`update` and `render` remain **87.9%/31.0%** and **89.6%/39.7%**
source-shape/strict fuzzy, both nonexact. The last full report at
this source revision remains **4,323/4,524** compared, **201
missing**, **124 extras**, no skipped or wrong signatures,
**93.67%/82.77%** average source-shape/strict fuzzy, **2,513
linked-exact / 1,074 byte-exact**. Timing, audio, GPU output and
live ARM32 behavior remain unverified. See
[the explosion flow audit](OBJECTGUN_EXPLOSION_FLOW_ARM_2026-10-16.md).

## ObjectGun destructor ownership (2026-10-16)

Android ARM confirms that the destructor releases owned geometry,
explosion objects and their array, then the readiness byte buffer.
The previous C++ called `ArrayRemoveAll` after
`ArrayReleaseClasses`, unnecessarily reallocating a buffer just
before deleting it; the ARM code has no such call. Removing it makes
both `D1`/`D2` aliases **100%/100%** source-shape/strict fuzzy and
linked-exact (from **89.2%/95.1%**). The neighboring `setScaling`
ignores its third argument in ARM and remains byte-exact without
change. Native `gof2`, **207/207** ARM units, the ObjectGun host
fixture, and **40** verifier tests (one skip) pass. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**, no skipped
or wrong signatures, **93.67%/82.77%** average source-shape/strict
fuzzy, **2,513 linked-exact / 1,074 byte-exact**. Exact sets show
two linked gains and zero losses. Live ARM32 teardown remains
unverified. See [the destructor audit](OBJECTGUN_DESTRUCTOR_ARM_2026-10-16.md).

## ObjectGun enemy delegation (2026-10-16)

Android ARM confirms that `ObjectGun::setEnemies` forwards its
`Array<Player*>*` unchanged to `Gun::setEnemies`; the previous
declared-only helper dereferenced the array and would fail on
`nullptr`. An extracted production-body fixture checks nonnull and
null forwarding at host `-O0/-O2`. `setEnemy` and `translate` are
genuine ARM no-ops and remain byte-exact. Native `gof2`, **207/207**
ARM units, and **40** verifier tests (one skip) pass. Focused
`setEnemies` source-shape/strict fuzzy **90.0%/50.0% ->
100%/100%**, now linked-exact. Full corpus: **4,323/4,524**
compared, **201 missing**, **124 extras**, no skipped or wrong
signatures, **93.67%/82.77%** average source-shape/strict fuzzy,
**2,511 linked-exact / 1,074 byte-exact**. Exact sets show one
linked gain and zero losses. Live game AI remains unverified. See
[the enemy delegation audit](OBJECTGUN_ENEMY_DELEGATION_ARM_2026-10-16.md).

## ObjectGun update math (2026-10-16)

Android ARM confirms typed vector-returning math calls in
`ObjectGun::update`: the first rotated offset moves the geometry,
the non-turret direction uses the player matrix while up uses the
camera matrix, and the turret uses the ordered product of two
`PlayerEgo` geometry matrices. The previous free shims had wrong
signatures and the previous matrix helper discarded its product.
Extracted production fragments check the direction inputs and
turret matrix order under host `-O0/-O2`. Native `gof2`, **207/207**
ARM units, and **40** verifier tests (one skip) pass. Focused update
source-shape/strict fuzzy **87.8%/29.0% -> 87.9%/31.0%**, still
nonexact. Full corpus: **4,323/4,524** compared, **201 missing**,
**124 extras**, no skipped or wrong signatures, **93.66%/82.75%**
average source-shape/strict fuzzy, **2,510 linked-exact / 1,074
byte-exact**, with zero exact gains/losses. Explosion timing and
live ARM32 behavior remain unverified. See
[the update math audit](OBJECTGUN_UPDATE_MATH_ARM_2026-10-16.md).

## ObjectGun render math (2026-10-16)

Android ARM confirms by-value math results and mutable matrix inputs
in `ObjectGun::render`, including `Matrix::operator*=` for turret and
ego orientation. The previous `void` free-function shims omitted those
inputs or discarded matrix products. Typed calls now update the
orientation and normalize/cross the vectors. Extracted production
fragments check iPad scaling and ego rotation under host `-O0/-O2`.
Native `gof2`, **207/207** ARM units, and **40** verifier tests (one
skip) pass. Focused render source-shape **90.4% -> 89.6%**; strict
fuzzy **34.6% -> 39.7%**; no exact status. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**, no skipped
or wrong signatures, **93.66%/82.75%** average source-shape/strict
fuzzy, **2,510 linked-exact / 1,074 byte-exact**, with zero exact
gains/losses. Full render behavior and live ARM32 GPU output remain
unverified. See [the math audit](OBJECTGUN_RENDER_MATH_ARM_2026-10-16.md).

## ObjectGun render bindings (2026-10-16)

Android ARM confirms that the render path reads
`Globals::Canvas` for camera, local-transform and draw calls,
and `Globals::iPad` for conditional scaling. The former null
file-static placeholders and their declared-only canvas call shims
in `ObjectGun::render` have been replaced with typed calls.
An extracted production-fragment fixture checks iPad scaling
and re-reading the canvas after `TransformSetLocal` at
host `-O0/-O2`. Native `gof2`, **207/207** ARM units and
**40** verifier tests (one skip) pass. Focused render
source-shape/strict fuzzy improves **86.8%/30.9% ->
90.4%/34.6%**, without exact status. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**,
zero skipped, **93.66%/82.75%** average source-shape/strict
fuzzy, **2,510 linked-exact / 1,074 byte-exact**; exact
sets have zero gains/losses. Full rendering behavior and
live ARM32 GPU output remain untested. See
[the render audit](OBJECTGUN_RENDER_BINDINGS_ARM_2026-10-16.md).

## ObjectGun update bindings (2026-10-16)

Android ARM confirms `Globals::Canvas`, typed transform update,
the `0 -> 3 -> 1` inactive animation sequence, and typed camera
lookups in `ObjectGun::update`. These replace a null canvas and
two null function pointers. `AnimationMode` now has fixed `int`
representation for observed modes `0-3`. A production-branch
host fixture checks call order and canvas re-reads at `-O0/-O2`.
Native `gof2`, **207/207** ARM units, and **40** verifier tests
(one skip) pass. Focused update source-shape/strict fuzzy improves
**83.8%/20.5% -> 87.8%/29.0%**; no exact match. Full corpus:
**4,323/4,524** compared, **201 missing**, **124 extras**, zero
skipped, **93.66%/82.75%** average source-shape/strict fuzzy,
**2,510 linked-exact / 1,074 byte-exact**, with zero exact
gains/losses. Full update behavior remains unverified; the
later render binding pass is documented separately.
See [the update audit](OBJECTGUN_UPDATE_BINDINGS_ARM_2026-10-16.md).

## ObjectGun constructor bindings (2026-10-16)

Android ARM confirms `Globals::Canvas`, `Globals::iPad`, and one
232-entry signed mesh-ID table shared by the constructor and
`ObjectGun::update`. The former null placeholders and two distinct
zero arrays have been replaced; all 232 entries match the original
`.rodata`. Native `gof2`, **207/207** ARM units, and **40**
verifier tests (one skip) pass. Both constructor aliases improve
**80.2%/41.1% -> 87.4%/65.5%** source-shape/strict fuzzy, but are
not exact. Full corpus: **4,323/4,524** compared, **201 missing**,
**124 extras**, zero skipped, **93.66%/82.75%** average
source-shape/strict fuzzy, **2,510 linked-exact / 1,074 byte-exact**.
Exact sets show zero gains and zero losses. `update` and `render`
still have unresolved call bindings, and live ARM32 rendering was
not tested. See [the constructor audit](OBJECTGUN_CONSTRUCTOR_BINDINGS_ARM_2026-10-16.md).

## ObjectGun mesh replacement (2026-10-16)

Android ARM `ObjectGun::replaceGun(unsigned,int)` uses the GOT binding
for `Globals::Canvas`, re-reads it after removing the old mesh, stores
the full new mesh ID, then adds its 16-bit ID via typed `PaintCanvas`
calls. The previous body dereferenced a null placeholder and called
declared-only helpers. A production-body fixture verifies old/new
16-bit ABI truncation, the full stored ID and a canvas swap inside the
first call. Native `gof2`, **207/207** ARM units and **40** verifier
tests (one skip) pass. Focused source-shape/strict fuzzy rises
**83.0%/41.2% -> 100%/100%**, linked-exact but not byte-exact.
Full corpus: **4,323/4,524** compared, **201 missing**, **124 extras**,
zero skipped, **93.66%/82.74%** average source-shape/strict fuzzy,
**2,510 linked-exact / 1,074 byte-exact**. Exact `(unit,symbol)`
comparison shows one linked gain and zero linked/byte losses.
The constructor and update have since received bounded binding
recoveries; render still contains unresolved bindings. Live ARM32
rendering remains untested. See
[the mesh replacement audit](OBJECTGUN_REPLACE_MESH_ARM_2026-10-16.md).

## Trail constructor behavior (2026-10-16)

An extracted production-body fixture now checks `Trail::Trail(int,int)`
against Android ARM for `(type=9, segments=2)` and the zero-segment
boundary `(type=4, segments=0)`. It verifies mesh counts, triangle and
UV call order, allocations, transform attachment and color. The
original's zero UV multiplier is confirmed, not corrected. Source C++
was unchanged; host `-O0`/`-O2`, native `gof2`, **207/207** ARM units
and **40** verifier tests (one skip) pass. Focused Trail remains
**98.76%/88.72%** source-shape/strict fuzzy, **7 linked-exact /
1 byte-exact**. The unchanged full corpus remains **4,323/4,524**
compared, **201 missing**, **124 extras**, zero skipped,
**93.66%/82.73%** average source-shape/strict fuzzy, **2,509
linked-exact / 1,074 byte-exact**. Live ARM32 rendering, larger
segment counts and allocation failure remain untested. See
[the constructor audit](TRAIL_CONSTRUCTOR_SEMANTICS_ARM_2026-10-16.md).

## Trail update and width behavior (2026-10-16)

An ARM-derived production-body fixture now checks `Trail::update` and
`setWidth` under host `-O0` and `-O2`. It covers the observed endpoint
write-then-shift order, fractional/negative coordinate truncation,
six mesh vertex updates, both signs of width change, and the local
translation. Source C++ was unchanged; native `gof2`, **207/207** ARM
units and **40** verifier tests (one skip) pass. Focused Trail metrics
remain **98.76%/88.72%** source-shape/strict fuzzy with **7
linked-exact / 1 byte-exact**. The unchanged full corpus remains
**4,323/4,524** compared, **201 missing**, **124 extras**, zero skipped,
**93.66%/82.73%** average source-shape/strict fuzzy, **2,509
linked-exact / 1,074 byte-exact**. The constructor now has a bounded
behavior fixture; live ARM32/GPU output remains unverified. See
[the semantics audit](TRAIL_UPDATE_WIDTH_SEMANTICS_ARM_2026-10-16.md).

## Trail canvas routing (2026-10-16)

Original Android ARM Trail methods use the GOT slot relocated to
`Globals::Canvas`. The previous C++ used ten null file-static canvas
pointers and declared-only render/color hooks. The constructor,
`update`, `setWidth`, `changeType`, and `render` now use the confirmed
global canvas and typed `PaintCanvas` calls. A production-body fixture
tests `render`, 13 type/color selections, and switching canvas pointers;
native `gof2`, **207/207** ARM units and **40** verifier tests (one skip)
pass. Focused 12-function source-shape/strict fuzzy rises
**93.65%/83.60% -> 98.76%/88.72%**; linked-exact rises **6 -> 7**.
Full corpus: **4,323/4,524** compared, **201 missing**, **124 extras**,
zero skipped, **93.66%/82.73%** average source-shape/strict fuzzy,
**2,509 linked-exact / 1,074 byte-exact**. Exact `(unit,symbol)` sets
show one linked gain and zero linked/byte losses. The constructor,
float `update`, and `setWidth` remain nonexact; the latter two now have
bounded behavior fixtures, while live rendering remains untested. See
[the Trail audit](TRAIL_CANVAS_ROUTING_ARM_2026-10-16.md).

## AEGeometry render routing (2026-10-16)

Original Android ARM `AEGeometry::render()` tail-branches through a
veneer to `PaintCanvas::DrawTransform(unsigned, const Matrix*)`. The
previous C++ used an undefined `_ae_geom_render` hook with the same
register arguments; linked-exact masked that wrong call destination.
The method now calls the typed canvas entry with its transform index
and null view matrix. An extracted-body fixture checks the low
visibility byte and call arguments; native `gof2`, **207/207** ARM units
and **40** verifier tests (one skip) pass. The focused method stays
**100%/100%** source-shape/strict fuzzy and linked-exact, not byte-exact.
Full corpus stays **4,323/4,524** compared, **201 missing**, **124
extras**, zero skipped, **93.64%/82.72%** average source-shape/strict
fuzzy, **2,508 linked-exact / 1,074 byte-exact**. Exact `(unit,symbol)`
sets show zero gains and zero losses. This fixes routing, not yet live
ARM32 rendering. See [the render audit](AEGEOMETRY_RENDER_ARM_2026-10-16.md).

## AEGeometry position setter (2026-10-16)

Android ARM confirms that `AEGeometry::setPosition(float,float,float)`
gets the local matrix from `PaintCanvas` and calls
`AEMath::MatrixSetTranslation` directly. The declared-only
`_ae_setPosition3` hook has been replaced. The numeric setter is in a
separate translation unit to preserve the original external branch shape
of the `Vector` overload; both are linked-exact. The extracted-body
canvas test passes, as do native `gof2`, **207/207** ARM units and
**40** verifier tests (one skip). Focused numeric source-shape/strict
fuzzy improves **0.7%/0.0% -> 100%/100%**, linked-exact but not
byte-exact. Full corpus: **4,323/4,524** compared, **201 missing**,
**124 extras**, zero skipped, **93.64%/82.72%** average source-shape/
strict fuzzy, **2,508 linked-exact / 1,074 byte-exact**. Exact-symbol
set comparison against the prior full report shows one linked gain and
zero linked or byte losses. Live ARM32 rendering remains unverified.
See [the position audit](AEGEOMETRY_SET_POSITION_ARM_2026-10-16.md).

## Bounded AERandom semantics (2026-10-16)

Android ARM `AERandom::nextInt(int)` wraps negation for its power-of-two
test. The C++ signed `-bound` was undefined for `INT_MIN`; the test now
uses unsigned 32-bit arithmetic without changing ordinary bounds.
An ARM-derived 48-bit oracle checks 2,560 return/seed transitions,
including rejection retries and `INT_MIN`. Optimized and overflow-trap
host builds pass after the change; the trap build failed before it.
Native `gof2`, **206/206** ARM units and **40** verifier tests (one skip)
pass. Focused source-shape/strict fuzzy stay **96.4%/50.5%**, nonexact.
Full corpus remains **4,323/4,524** compared, **201 missing**, **124
extras**, zero skipped, **93.62%/82.69%** average source-shape/strict
fuzzy, **2,507 linked-exact / 1,074 byte-exact**, with no exact-pair
losses. Negative nonpower bounds and live ARM32 execution remain
unverified. See [the RNG audit](AERANDOM_BOUND_ARM_2026-10-16.md).

## Particle UV selector mapping (2026-10-16)

Direct Android ARM disassembly of `IParticleSystem::rotateUVs` confirms
the signed seed, one bounded random draw and the four UV selectors.
An independent selector check against the production method passes for
256 negative/nonnegative seeds. No production C++ changed; native
`gof2`, **206/206** ARM units and **40** verifier tests (one skip) pass.
The helper remains **95.2%** source-shape / **72.0%** strict fuzzy,
nonexact. The unchanged full corpus remains **4,323/4,524** compared,
**201 missing**, **124 extras**, zero skipped, **93.62%/82.69%**
average source-shape/strict fuzzy, **2,507 linked-exact / 1,074
byte-exact**. Bounded `AERandom::nextInt(int)` and live GPU output
still require separate validation. See
[the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle UV dispatch and second-frame guard (2026-10-16)

Android ARM checks flags byte `+0x37`, bit 1, before calling
`IParticleSystem::rotateUVs`; the base emitter tested the wrong full-word
bit (`0x80000000` instead of `0x02000000`). The source now uses the
confirmed flag. An extracted-production-body smoke first reproduced the
failure, then verified both flag values, UV output mapping, and suppression
of a second emission for flags `0x80` and `0x40`. Native `gof2`, **206/206**
ARM units and **40** verifier tests (one skip) pass. Focused `emit`
improves **88.5%/16.7% → 88.8%/19.9%** source-shape/strict fuzzy;
it is not exact. Full corpus remains **4,323/4,524** compared,
**201 missing**, **124 extras**, zero skipped, **93.62%/82.69%**
average source-shape/strict fuzzy, **2,507 linked-exact / 1,074
byte-exact**, with no exact-pair losses. The UV helper itself and live
ARM32/GPU output are not independently validated. See
[the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle random-emission checks (2026-10-16)

REA/ARM call sequences in `IParticleSystem::emit` now have focused
production-body host coverage for the `0x80` random-position branch
and the ordinary directional/XZ/Y spread branch. The probe checks
draw order, final RNG state, particle position and velocity, and
randomized lifetime/sizes. No production C++ changed; native `gof2`,
**206/206** ARM units and **40** verifier tests (one skip) pass.
Focused `emit` remains **88.5%** source-shape / **16.7%** strict fuzzy,
nonexact. The unchanged full-source corpus report remains **4,323/4,524**
compared, **201 missing**, **124 extras**, **93.62%/82.69%** average
source-shape/strict fuzzy, **2,507 linked-exact / 1,074 byte-exact**.
UV rotation, repeated-frame state and live ARM32/GPU behavior remain
unverified. See [the emitter audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## Base particle-emission position/timing (2026-10-16)

REA/Android ARM analysis of `IParticleSystem::emit` confirms that
particle position uses inverse frame-travel length, while update time
uses inverse emitter speed times a retained emission step. ARM reuses
the step register; the previous timing audit missed this overwrite and
has been corrected. An extracted-production-body smoke checks both
position and time across two frame lengths and the configured-step
branch. Native `gof2`, **206/206** ARM units, and **40** verifier tests
(one skip) pass. Focused `emit` remains **88.5%** source-shape and
nonexact; strict fuzzy moves **17.5% → 16.7%**.
Full corpus remains **4,323/4,524** compared, **201 missing**, **124
extras**, zero skipped, **93.62%/82.69%** average source-shape/strict
fuzzy, and **2,507 linked-exact / 1,074 byte-exact**. Exact pair sets
have no losses. Random emission branches and live ARM32/GPU behavior
remain unverified. See [the timing audit](PARTICLE_BASE_EMIT_TIMING_ARM_2026-10-16.md).

## ParticleSystemMesh ordinary-emission dispatch (2026-10-16)

Original Android ARM `ParticleSystemMesh::emit` tail-branches to the
existing `IParticleSystem::emit(int)` when flag `0x8000` is clear;
source instead called an undefined ordinary-emission shim. The direct
ARM relocation now names the original base method. A production-body
host fixture checks ordinary/trail selection, suppression and section
reset. Native `gof2`, **206/206** ARM units and **40** verifier tests
(one skip) pass. `emit` remains **100%/100%** strict fuzzy/source-shape
and linked-exact. Full corpus metrics and exact pair sets are unchanged:
**4,323/4,524** compared, **201 missing**, **124 extras**, zero skipped,
**93.62%/82.69%** average source-shape/strict fuzzy, **2,507
linked-exact / 1,074 byte-exact**. The base emitter algorithm and live
ARM32 output still need independent validation. See
[the dispatch audit](PARTICLE_MESH_EMIT_DISPATCH_ARM_2026-10-16.md).

## ParticleSystemMesh trail emission (2026-10-16)

Android ARM at `0x18564c` anchors the recovered `emitTrail` instance
method: emitter/edge vectors, optional cap, UV and color writes, section
turnover and ring-buffer state now replace a one-instruction stub. An
extracted-body host smoke, native `gof2`, **206/206** ARM units and **40**
verifier tests (one skip) pass. Focused strict fuzzy/source-shape rises
**0%/0% -> 37.5%/94.9%** over a 605-instruction original body; no exact
match is claimed. Full coverage remains **4,323/4,524**, **201 missing**,
zero skipped, **93.62%/82.69%** average source-shape/strict fuzzy,
**2,507 linked-exact / 1,074 byte-exact** with no exact-pair loss or gain.
One forwarding dispatch adds an extra symbol (**123 -> 124**), while the
linked-function call audit stays **2,392/53/647** match/mismatch-candidate/
unresolved. Ordinary emission, real GPU output and live ARM32 trail
continuity remain unverified. See
[the trail audit](PARTICLE_MESH_EMIT_TRAIL_ARM_2026-10-16.md).

## AEGeometry LOD selection and transform transfer (2026-10-16)

Android ARM `updateLod` confirms an alternative position transform,
position-minus-camera delta, `0.33`/`0.66` detail thresholds, target
matrix-copy destinations and transfer of the active transform's time
to selected/base transforms. The old body used four unresolved
`AEGeomCanvas` routes, 32-bit pointer narrowing, different thresholds
and zero animation time; these are corrected. A 20-body host smoke
covers visibility, boundary and `NaN` factors, unchanged/changed/base
LOD branches and animation handoff. Native `gof2`, **206/206** ARM
units and **40** verifier tests (one skip) pass. Focused strict
fuzzy/source-shape rises **82.8%/96.4% -> 97.6%/100%**; this
function is not exact. Full coverage stays **4,323/4,524**, with
**201 missing**, **123 extras**, zero skipped, **93.60%/82.69%**
average source-shape/strict fuzzy and **2,507 linked-exact / 1,074
byte-exact**, with zero exact-pair losses/gains. The linked-function
call audit remains **2,392 matches / 53 mismatch candidates / 647
unresolved**; `updateLod`'s 19 direct calls were checked separately.
No `AEGeomCanvas` calls remain in `AEGeometry.cpp`, but live ARM32 LOD
and animation behavior remains unverified. See
[the LOD audit](AEGEOMETRY_UPDATE_LOD_ARM_2026-10-16.md).

## AEGeometry direction basis (2026-10-16)

Original Android ARM confirms `setDirection` writes right/up/direction
as the three matrix columns, preserving the translation entries; the
old body wrote them consecutively and used unresolved hooks with
32-bit pointer narrowing. The source now uses the original vector
cross/normalize order and typed canvas/math calls. The 19-body host
smoke, production native math smoke, native `gof2`, **206/206** ARM
units and **40** verifier tests (one skip) pass. Focused strict
fuzzy/source-shape rises **45.1%/95.6% -> 85.4%/100%**, but this
function is not exact. Full coverage stays **4,323/4,524**, with
**201 missing**, **123 extras**, zero skipped and **93.60%/82.68%**
average source-shape/strict fuzzy. **2,507 linked-exact / 1,074
byte-exact** remain, with zero exact-pair losses/gains. The
linked-function call audit remains **2,392 matches / 53 mismatch
candidates / 647 unresolved**; `setDirection`'s ten direct calls were
checked separately against the Android disassembly. Four
`AEGeomCanvas` routes remain in `updateLod`; live ARM32 behavior is
unverified. See [the direction audit](AEGEOMETRY_DIRECTION_ARM_2026-10-16.md).

## AEGeometry translation axes (2026-10-16)

Original Android ARM confirms `translate(x,y,z)` adds `x`, `y` and `z`
to matrix entries `m[3]`, `m[7]` and `m[11]`, respectively. The old
body ignored `x`, misrouted `z` and narrowed matrix pointers; these
errors are corrected. The 18-body host smoke, native `gof2`, **206/206**
ARM units and **40** verifier tests (one skip) pass. The float overload
becomes **100%/100%** source-shape/strict fuzzy and linked-exact, not
byte-exact. Full coverage stays **4,323/4,524**, with **201 missing**,
**123 extras** and zero skipped; average source-shape/strict fuzzy is
**93.60%/82.67%**. Linked-exact rises **2,506 -> 2,507**;
byte-exact stays **1,074**, with zero exact-pair losses. Named
direct-call matches rise **2,387 -> 2,392**; **53 mismatch candidates /
647 unresolved** remain. Seven `AEGeomCanvas` routes and live ARM32
transform behavior remain open. See
[the translation audit](AEGEOMETRY_TRANSLATE_ARM_2026-10-16.md).

## AEGeometry rotation and scaling calls (2026-10-16)

Original Android ARM confirms the call order and arguments in
`setRotation(float,float,float)`, `rotate(float,float,float)` and
`setScaling(float,float,float)`. Those bodies now call the real typed
`PaintCanvas::TransformGetLocal` and `AEMath::MatrixSetRotation/Scaling`
methods instead of declared-only hooks with 32-bit pointer narrowing.
The 16-body host smoke, native `gof2`, **206/206** ARM units and **40**
verifier tests (one skip) pass. All three become **100%/100%**
source-shape/strict fuzzy and linked-exact, not byte-exact. Full corpus
coverage stays **4,323/4,524**, with **201 missing**, **123 extras** and
zero skipped; average source-shape/strict fuzzy is **93.59%/82.65%**.
Linked-exact rises **2,503 -> 2,506**, byte-exact stays **1,074**, with
zero exact-pair losses. Named direct-call matches rise **2,372 -> 2,387**;
**53 mismatch candidates / 647 unresolved** remain. Eight
`AEGeomCanvas` routes and live ARM32 transform behavior remain open.
See [the rotation/scaling audit](AEGEOMETRY_ROTATION_SCALING_ARM_2026-10-16.md).

## AEGeometry construction and LOD calls (2026-10-16)

Original Android ARM confirms eight more canvas call routes in the two
constructors, `setMatrix` and two LOD-child methods. The mesh constructor
now reads its reference matrix from the newly created transform instead of
index zero and no longer narrows that matrix pointer on a 64-bit host. A
13-body production-source smoke, native `gof2`, **206/206** ARM units and
**40** verifier tests (one skip) pass. The mesh constructor C1/C2 strict
fuzzy score rises **45.6% -> 52.7%**; no constructor exact match is claimed.
Full coverage stays **4,323/4,524**, **201 missing**, **123 extras**, zero
skipped, **93.59%/82.64%** average source-shape/strict fuzzy, **2,503
linked-exact / 1,074 byte-exact** with zero exact-pair losses/gains.
The linked-function direct-call audit remains **2,372 matches / 53
mismatch candidates / 647 unresolved**. Fourteen `AEGeomCanvas` routes
remain; live ARM32 transform behavior is unverified. See
[the construction/LOD audit](AEGEOMETRY_CTOR_LOD_CALLS_ARM_2026-10-16.md).

## AEGeometry canvas call contracts (2026-10-16)

Ten `AEGeometry` transform/LOD methods now call existing
`PaintCanvas::TransformGetLocal`, `TransformCreate` and
`TransformAddChild` methods instead of declared-only `AEGeomCanvas`
routes. Android ARM confirms the 12 corrected direct-call targets.
A host smoke exercises eight production method bodies, including both
parent-transform choices and child bookkeeping; the two LOD bodies
are covered by ARM call-target comparison, not host behavior. Native
`gof2`, **206/206** ARM units and **40** verifier tests (one skip)
pass. All ten changed methods remain **100%/100%** source-shape/strict
fuzzy and linked-exact, not byte-exact. Full coverage stays
**4,323/4,524** unique original functions, **201 missing**,
**123 extras**, zero skipped, **93.59%/82.64%** average
source-shape/strict fuzzy, **2,503 linked-exact / 1,074 byte-exact**,
with zero exact-pair losses/gains. Named direct-call matches rise
**2,360 -> 2,372** and mismatch candidates fall **65 -> 53**;
**647** sites remain unresolved. Another **22** `AEGeomCanvas` calls
in other methods and live ARM32 transform/GL behavior remain open.
See [the canvas call audit](AEGEOMETRY_CANVAS_CALLS_ARM_2026-10-16.md).

## PaintCanvas text-line call contracts (2026-10-16)

The two `DrawTextLines` alignment overloads now call their existing
`GetTextWidth`, `DrawString` and `GetTextHeight` methods directly,
matching six Android ARM call targets instead of unresolved transition
shims. A production-body host smoke checks both alignment branches,
line spacing and an empty array. Native `gof2`, **206/206** ARM units
and **40** verifier tests (one skip) pass. Both changed overloads
remain **100%/100%** source-shape/strict fuzzy and linked-exact, not
byte-exact. Full coverage stays **4,323/4,524** unique original
functions, **201 missing**, **123 extras**, zero skipped,
**93.59%/82.64%** average source-shape/strict fuzzy, **2,503
linked-exact / 1,074 byte-exact**, with zero exact-pair losses/gains.
Named direct-call matches rise **2,354 -> 2,360** and mismatch
candidates fall **71 -> 65**; **647** calls remain unresolved by the
audit. Real fonts, rendering and live ARM32 UI output remain
unverified. See [the text-line call audit](PAINTCANVAS_TEXT_LINES_CALLS_ARM_2026-10-16.md).

## Camera view-frustum flag (2026-10-16)

Android ARM `CameraIsPointinViewFrustum` and
`CameraIsSphereinViewFrustum` read `Engine::vfc` before frustum work.
The recovered functions dereferenced a never-assigned placeholder
pointer; both now read the original static byte. Native `gof2`,
**206/206** ARM units, **40** verifier tests (one skip) and the
existing canvas wrapper smoke pass. Focused point source-shape/strict
fuzzy changes **81.1%/53.8% -> 82.3%/54.0%**; sphere changes
**82.5%/34.9% -> 82.4%/46.3%**. Neither is exact. Full coverage is
**4,323/4,524** unique functions, **201 missing**, **123 extras**,
zero skipped, **93.59%/82.64%** average source-shape/strict fuzzy,
**2,503 linked-exact / 1,074 byte-exact** with zero exact-pair
losses/gains. The source-tree path that enables `Engine::vfc` and
live ARM32 frustum behavior remain unverified. See
[the camera flag audit](CAMERA_VFC_FLAG_ARM_2026-10-16.md).

## MeshIntersect shader flag (2026-10-16)

Android ARM `MeshIntersect` at `0x6d818` reads
`Engine::enableShader` before optionally inverting a successful
triangle hit's average V coordinate. The recovered body dereferenced
an uninitialized placeholder pointer instead; it now reads the
original static byte. A host test of the production body covers
hit/miss, empty index data and both shader modes. Native `gof2`,
**206/206** ARM units and **40** verifier tests (one skip) pass.
Focused source-shape/strict fuzzy rises **84.7%/43.0% →
86.2%/43.7%** without exact equality. Full coverage remains
**4,323/4,524** unique functions, **201 missing**, **123 extras**,
zero skipped, **93.59%/82.63%** average source-shape/strict fuzzy,
**2,503 linked-exact / 1,074 byte-exact**, with zero exact-pair
losses/gains. Live Android ARM32 and nonrepresentative geometry
remain unverified; next priority is another independently anchored
flag placeholder. See [the MeshIntersect audit](MESH_INTERSECT_SHADER_FLAG_ARM_2026-10-16.md).

## SpriteSystemCreate shader flag (2026-10-16)

Android ARM `SpriteSystemCreate` at `0x86adc` reads
`Engine::enableShader` before conditionally filling mesh tangents
and binormals. The recovered body dereferenced an uninitialized
placeholder pointer instead; it now reads the original static byte.
Native `gof2`, **206/206** ARM units and **40** verifier tests
(one skip) pass. Focused strict fuzzy changes **66.9% → 67.1%**;
source-shape slightly falls **95.2% → 95.0%**, so no exact match is
claimed. Full coverage stays **4,323/4,524** unique functions,
**201 missing**, **123 extras**, zero skipped, **93.59%/82.63%**
average source-shape/strict fuzzy, **2,503 linked-exact /
1,074 byte-exact**, with no exact-pair losses or gains. A full
Android ARM32 sprite-creation runtime test remains open. Next
priority: audit remaining flag placeholders one original target at
a time. See [the shader-flag audit](SPRITE_CREATE_SHADER_FLAG_ARM_2026-10-16.md).

## Sprite UV/RGBA data helpers (2026-10-16)

Android ARM `AbyssEngine::SpriteSystemSetUv` at `0x86cd4` tests
`Engine::enableShader` before deciding whether to invert V coordinates.
The recovered function dereferenced an uninitialized placeholder flag
pointer instead; it now reads the original static byte. A host test of
the production bodies covers selected-sprite writes, both shader-flag
branches, bounds/null cases and RGBA replication. Native `gof2`,
**206/206** ARM units and **40** verifier tests (one skip) pass.
Focused `SetUv` source-shape/strict fuzzy rises **87.9%/46.6% →
88.9%/52.8%** without becoming exact. The full corpus remains
**4,323/4,524** unique original functions, **201 missing**,
**123 extras**, zero skipped, **93.59%/82.63%** average
source-shape/strict fuzzy, **2,503 linked-exact / 1,074 byte-exact**
with zero exact-pair losses/gains. Live ARM32/GL sprite output remains
untested; next priority is the neighboring sprite-creation flag.
See [the helper audit](SPRITE_UV_RGBA_FLAG_ARM_2026-10-16.md).

## PaintCanvas sprite-system call contracts (2026-10-16)

Android ARM `PaintCanvas::SpriteSystemSetAllUv` (`0x7a7bc`),
`SpriteSystemSetUv` (`0x7a7f8`) and `SpriteSystemSetRGBA` (`0x7a840`)
perform the existing bounds/null checks, then call their corresponding
`AbyssEngine::SpriteSystemSet*` functions. The recovered wrappers passed
the same arguments to three undefined `paintcanvas_ext_sprite_*` symbols;
they now call the implemented, original-named helpers.

Native `gof2`, **206/206** ARM units and **40** verifier tests (one skip)
pass. All three focused functions remain **100%/100%** source-shape/
strict fuzzy and linked-exact, not byte-exact. Full coverage remains
**4,323/4,524** unique original functions, **201 missing**, **123 extras**,
zero skipped, **93.59%/82.63%** average source-shape/strict fuzzy,
**2,503 linked-exact / 1,074 byte-exact** with no exact-pair losses or
gains. Direct-call-target matches improve **2,351 → 2,354** and mismatch
candidates fall **74 → 71**; 647 unresolved remain. A real GL/ARM32
sprite-output test is still missing. Next priority: controlled sprite
data/UV/RGBA behavior checks or the next call-contract cluster. See
[the sprite call audit](PAINTCANVAS_SPRITE_CALLS_ARM_2026-10-16.md).

## PaintCanvas StopDraw2FBO call contract (2026-10-15)

Android ARM `PaintCanvas::StopDraw2FBO` at `0x7d08e` calls
`Engine::DeactivateRender2TextureFBO`, `PaintCanvas::SetBlendMode(0)`,
`Engine::DoPostEffect`, then tail-branches to
`Engine::ActivateViewBuffer`. Four undefined `paintcanvas_ext_fbo_*`
calls in the recovered source have been replaced by these existing
methods. REA/IDA identifies the original tail veneer; the rebuilt ARM
object has the expected `R_ARM_THM_JUMP24` relocation.

Native `gof2` and **206/206** ARM units pass. Focused source-shape/strict
fuzzy remains **100%/100%**, linked-exact but not byte-exact. The full
corpus remains **4,323/4,524** unique original functions, **201 missing**,
**123 extras**, zero skipped, **93.59%/82.63%** average source-shape/
strict fuzzy, **2,503 linked-exact / 1,074 byte-exact**. Exact-pair sets
show zero losses or gains. Direct-call audit matches improve **2,348 →
2,351**, candidates **77 → 74**; 647 unresolved remain. Runtime FBO/GL
behavior is not yet tested; next priority is a bounded PaintCanvas shim
cluster with a behavior check where feasible. See
[the FBO call audit](PAINTCANVAS_STOP_FBO_CALL_ARM_2026-10-15.md).

## ImageFactory portrait RNG call contract (2026-10-15)

Android ARM `ImageFactory::createChar(int)` at `0x11c6cc` and
`createChar(bool,int)` at `0x11c6f4` pass `Globals::rnd` as the object
pointer to `AERandom::nextInt(int)`. The recovered source instead called
an undefined free-function symbol with `(void*, int)`. Both overloads now
invoke the actual member method at all three sites. REA/IDA assembly and
the Android ELF's call annotations agree on the target; the portrait host
smoke exercises the object call contract.

Native `gof2`, portrait smoke and **206/206** ARM units pass. The two focused
functions remain **100%/100%** source-shape/strict fuzzy and linked-exact,
not byte-exact. Full corpus remains **4,323/4,524** unique original
functions, **201 missing**, **123 extras**, zero skipped,
**93.59%/82.63%** average source-shape/strict fuzzy, **2,503
linked-exact / 1,074 byte-exact**, with zero exact-pair losses or gains.
The direct-call audit improves **2,345 → 2,348** matching targets and
**80 → 77** mismatch candidates; 647 sites remain unresolved. Original
ARM32 runtime behavior and random-sequence parity remain untested.
Next priority: audit the PaintCanvas shim/call-contract cluster. See
[the focused contract audit](IMAGE_FACTORY_RNG_CALL_ARM_2026-10-15.md).

## ARM direct call-target audit (2026-10-15)

At the String numeric-constructor baseline, 2,503 `linked-exact` function-pairs were audited for direct ARM
call relocations against the original Android 2.0.16 ELF. Of 3,072 call sites,
2,345 have the same named callee, 80 are mismatch candidates across 59
function-pairs, and 647 could not be resolved by the original disassembly
header/label parser. In particular, several PaintCanvas functions call local
`paintcanvas_ext_*` shims where the original calls named engine methods;
`ImageFactory::createChar` calls a different `AERandom::nextInt` signature.
Some candidates may be aliases or forwarding thunks; none is automatically a
proven behavior defect. This historical tool-only audit changed no C++ or
corpus metrics. The ImageFactory correction above resolves three candidates;
the largest shim cluster and data relocations remain. See
[the detailed audit](ARM_CALL_TARGET_AUDIT_2026-10-15.md).

## String numeric constructors and append audit (2026-10-15)

Android ARM C2 constructors for `String(char)` (`0x6e484`), `String(int)`
(`0x737b4`) and `String(long long)` (`0x72904`) initialize the vtable and
null data pointer, then call `Set(long long)` without first writing
`length`. The recovered constructors now follow that order. The `char`
path explicitly uses an unsigned byte on native hosts, matching the
observed zero high word. Native smoke checks `0xff`, `INT_MIN`,
`INT64_MIN`, `INT64_MAX` and zero.

Native `gof2`, ABI/lifetime and standalone String checks, **206/206** ARM
units and **37** verifier tests (one skip) pass. Focused source-shape/
strict fuzzy reaches **100%/100%** for all three constructors, from
**97.6%/97.0%**, **97.6%/97.0%** and **97.3%/96.6%** respectively.
Full corpus compares **4,323/4,524** unique original functions with
**201 missing**, **123 extras**, zero skipped and **93.59%/82.63%**
average source-shape/strict fuzzy. Linked-exact gains exactly three
constructor pairs, **2,500 → 2,503**, with zero losses; byte-exact
remains **1,074** with zero losses.

The adjacent `operator+=(long long)` ARM audit confirms a direct
`realloc`/copy path without an explicit final terminator write. Safe
direct-append experiments lost source shape, so the production method
was left unchanged and native terminator/empty/zero tests were added.
Live Android ARM32 and allocator-dependent bytes remain unverified;
next priority is a controlled ARMv7 append-boundary observation. See
[the constructor and append audit](STRING_NUMERIC_CTORS_ARM_2026-10-15.md).

## String signed-int append delegation (2026-10-15)

Android ARM `String::operator+=(int const&)` (`0x72928–0x72950`)
sign-extends the input to a stack-local 64-bit value and calls
`operator+=(long long const&)` directly. The recovered source previously
constructed an intermediate `String` and used the string overload; it now
follows the observed call contract. The native ABI/lifetime smoke checks
`INT_MIN` through this path.

Native `gof2`, the ABI smoke, standalone String reverse check, **206/206**
ARM units and **37** verifier tests (one skip) pass. Focused
source-shape/strict fuzzy improves **53.0%/14.3% → 100%/100%** and gains
linked-exact, not byte-exact equality. Full corpus compares
**4,323/4,524** unique original functions with **201 missing**, **123
extras**, zero skipped, **93.59%/82.63%** average source-shape/strict
fuzzy, **2,500 linked-exact / 1,074 byte-exact**. Exact-pair comparison
against the preceding report shows one linked gain and zero losses in
either set. Live ARM32 behavior and downstream `operator+=(long long)`
allocator/terminator semantics remain unverified. Next priority: audit
that downstream append path. See
[the signed-int append audit](STRING_APPEND_INT_ARM_2026-10-15.md).

## String 64-bit integer conversion (2026-10-15)

Android ARM `String::Set(long long)` (`0x72a0c–0x72ad4`) writes decimal
digits directly to its owned UTF-16 buffer, using unsigned 64-bit division
and in-place reversal. The recovered method no longer invokes undefined
signed negation for `INT64_MIN` or copies through a temporary buffer.
The original requests 40 bytes, which static analysis shows is too small
for a negative 19-digit value plus terminator; our deliberate 42-byte
allocation prevents that overflow and is not an exact-code claim.

Native `gof2`, the ABI/lifetime smoke, standalone String reverse check,
**206/206** ARM units and **37** verifier tests (one skip) pass. Focused
source-shape/strict fuzzy improves **65.1%/18.9% → 89.4%/21.9%**;
the function remains non-exact. Full corpus compares **4,323/4,524**
unique original functions, with **201 missing**, **123 extras**, zero
skipped, **93.58%/82.61%** average source-shape/strict fuzzy, and no
losses or gains in the **2,499 linked-exact / 1,074 byte-exact**
`(unit, symbol)` sets. Live original ARM32 behavior and allocator effects
remain unverified. Next priority: adjacent integer-constructor/append ARM
shapes and a controlled ARMv7 boundary check if available. See
[the integer-conversion audit](STRING_SET_I64_ARM_2026-10-15.md).

## String float object composition (2026-10-15)

Android ARM `String::Set(float)` constructs a local `String`, appends the
helper's UTF-16 digits, adds leading `"0."` and zeros for nonpositive
exponents, inserts `"."` through two `SubString` calls and `operator+`
for positive exponents, and prefixes `"-"` when needed. The recovered
method previously used a separate raw UTF-16 buffer; it now follows
the observed temporary-object and destruction sequence.

Native `gof2`, the ABI/lifetime smoke (including `String(float)` and
`operator+=(float)`), standalone String reverse check, **206/206** ARM
units and **37** verifier tests (one skip) pass. Focused source-shape/
strict fuzzy improves **43.4%/12.3% → 96.6%/65.6%**; `Set(float)`
remains non-exact. Full corpus compares **4,323/4,524** unique original
functions, **201 missing**, **123 extras**, zero skipped and zero
wrong-type, at **93.57%/82.61%** average source-shape/strict fuzzy.
The **2,499 linked-exact / 1,074 byte-exact** `(unit, symbol)` sets
have zero losses or gains. Live ARM32 float formatting, exceptional
values, all rounding boundaries and remaining codegen differences
remain unverified. See [the object-composition audit](STRING_FLOAT_SET_OBJECT_ARM_2026-10-15.md).

## String float digit helper and call contract (2026-10-15)

Android ARM `AbyssEngine::computeFloatString` (`0x72af8–0x72ca4`)
returns a 33-unit UTF-16 allocation and reports exponent/sign through
output pointers. `String::Set(float)` calls it with precision 10 and
mode 0. The recovered helper previously returned `void` and the caller
used an undefined four-argument symbol; both now use the observed
five-argument contract. The fractional exponent initialization was
corrected from the ARM instructions.

Native `gof2`, the production-body ABI smoke, standalone String reverse
check, **206/206** ARM units and **37** verifier tests (one skip) pass.
Focused source-shape/strict fuzzy improves **70.5%/20.5% →
86.8%/26.0%** for the helper and **42.3%/11.7% → 43.4%/12.3%**
for `Set(float)`; neither is exact. Full corpus remains **4,323/4,524**
original functions, **201 missing**, **123 extras**, zero skipped,
**93.56%/82.60%** average source-shape/strict fuzzy, and **2,499
linked-exact / 1,074 byte-exact** with zero exact-symbol losses or gains.
Live ARM32 float formatting, exceptional values and all rounding
boundaries remain unverified; see [the helper audit](STRING_FLOAT_HELPER_ARM_2026-10-15.md).

## String float sign (2026-10-15 baseline)

Android ARM `String::Set(float)` (`0x72cb0–0x72dbe`) loads `"-"` at
`0x72d6e` when the digit helper reports a negative value. The recovered
method previously used `"E"`; it now uses `"-"`. A controlled-helper ABI
smoke checks `1.5` and `-1.5`. Native `gof2`, the smoke, **206/206** ARM
units and **37** verifier tests (one skip) pass. Focused source-shape/strict
fuzzy stays **42.3%/11.7%**; this is a behavioral fix, not an exact match.
Full ARM corpus remains **4,323/4,524** original functions, **201 missing**,
**123 extras**, zero skipped and **93.56%/82.60%** average
source-shape/strict fuzzy, with no losses or gains in the **2,499
linked-exact** and **1,074 byte-exact** `(unit, symbol)` sets.

At this baseline, the digit helper remained an unresolved link/behavior
gap; the newer recovery above closes that static contract. The original
sign-only smoke used controlled helper output. See
[the sign audit](STRING_FLOAT_SIGN_ARM_2026-10-15.md) for that earlier gate.

## String append temporary-copy contract (2026-10-15)

REA/IDA Android ARM `String::operator+=(String const&)` (`0x727d8–0x72848`)
copies the source through `String(other, false)` before appending. Its copy
constructor calls zero-terminated `Set`, so a source with an embedded zero
contributes only the prefix, even when its stored length is larger. The
recovered method now follows that behavior; the native ABI smoke covers the
embedded-zero and self-append cases. `operator=` at `0x727a8–0x727ba` already
matches the original gate and was not changed.

Native `gof2`, the ABI smoke, **206/206** ARM units and **37** verifier tests
(one skip) pass. Focused source-shape/strict fuzzy improves
**50.5%/9.4% -> 66.7%/30.5%**; the method remains non-exact. Full ARM
corpus compares **4,323/4,524** original functions, **201 missing**,
**123 extras**, zero skipped and **93.56%/82.60%** average
source-shape/strict fuzzy. The **2,499 linked-exact** and **1,074 byte-exact**
`(unit, symbol)` sets have zero gains or losses versus the preceding report.
Original live ARM32 behavior and the original `realloc` versus recovered
`new[]` allocator distinction remain unverified. See
[the append audit](STRING_APPEND_COPY_ARM_2026-10-15.md).

## Recovery objective and REA/IDA workflow (2026-10-15)

Continue improving native recovery one bounded function group at a time.
For each selected family, use targeted REA/IDA reads of the original Android
ARM binary to map the function, relevant callers/callees, field accesses,
branches and remaining unknowns before editing. Use these reads to identify
addresses, data flow and plausible source structure; cross-check claims
against instruction evidence, other available platform sources and focused
behavior tests. Accept C++ changes only with native and ARM build gates,
focused source-shape verification and a full-corpus exact-pair comparison.
REA output is evidence, not an automatic semantic proof or a replacement for
the verifier. Its call relationships may be partial; record this rather than
assuming the graph is complete. Where an executable original is available,
prefer controlled runtime observations for unresolved behavior. The local
GOF2 HD 1.1.4 iOS IPA is ARMv7 and runs in SuperDuperHyperHle, which
provides a guest GDB server; use it as a supplementary behavioral oracle,
not as an exact binary reference for Android 2.0.16. Aim for at least
98% source-shape in a selected family when the evidence and toolchain
permit it; do not alter correct behavior solely to reach that number.
Report unattained targets and untested live behavior
explicitly. Large whole-binary REA overviews may exceed its IDA response
limit; prefer named-function queries.

## String::Set empty-buffer contract (2026-10-15)

REA/IDA Android ARM `Set(unsigned short const *)` (`0x723dc–0x72482`)
and `Set(char const *)` (`0x724b4–0x7255c`) confirm that null and
non-null empty inputs leave an owned two-byte zero buffer, not
`data == nullptr`. Both C++ overloads now follow that contract;
the UTF-16 nonempty path also uses the original pointer scan and
buffer copy. This closes the empty-buffer dependency noted in the
`SubString` audit, including a slice beginning with an embedded zero.

Native `gof2`, the production-code ABI smoke, **206/206** ARM units
and **37** verifier tests (one skip) pass. Focused source-shape/strict
fuzzy changes from **50.9%/32.0% → 73.0%/61.3%** for UTF-16 and
**52.0%/35.0% → 69.2%/35.4%** for char input. Neither method is exact
and the 98% family target is not reached. Full ARM corpus stays at
**4,323/4,524** original functions, **201 missing**, **123 extras**,
zero skipped and **93.56%/82.59%** average source-shape/strict fuzzy.
The **2,499 linked-exact** and **1,074 byte-exact** `(unit, symbol)`
sets have zero losses or gains versus the preceding full report.
Identical-input original ARM32 execution is untested; constructor and
assignment ownership paths remain the next String priority. See
[the Set audit](STRING_SET_EMPTY_ARM_2026-10-15.md).

## String::SubString ARM behavior and source shape (2026-10-15)

REA/IDA Android ARM `0x72ed8–0x72f64` confirms a hidden result slot,
exact `end-start` UTF-16 copy for `end > start`, temporary-copy
construction and destruction, and no clamp to `this->length`. The
recovered C++ now follows this lifecycle. Its ABI smoke covers a normal
slice, an empty slice and truncation when the selected range contains a
zero. Native `gof2`, the smoke, **206/206** ARM units and **37** verifier
tests (one skip) pass. Focused source-shape/strict fuzzy changes
**58.4%/23.4% → 99.3%/94.2%**; the function is not exact.

The full ARM corpus compares **4,323/4,524** original functions,
with **201 missing**, **123 extras**, zero skipped and **93.55%/82.59%**
average source-shape/strict fuzzy. Both exact `(unit, symbol)` sets
have zero gains or losses versus the preceding full report: **2,499
linked-exact** and **1,074 byte-exact**. Original live behavior on
identical inputs is untested. The adjacent `Set(unsigned short const *)`
empty-buffer difference recorded at this stage is resolved in the Set
audit above. See [the SubString audit](STRING_SUBSTRING_ARM_2026-10-15.md).

## PlayerFighter cloak/reset ABI (2026-10-15)

REA/IDA Android ARM `reset` (`0xdc934–0xdca22`), `cloak`
(`0xdcd58–0xdcd94`), `setCloakingPossible` (`0xdcd9c–0xdcdb8`) and
`handleCloaking` confirm the cloak timer/duration at `+0x2c8/+0x2cc`,
not the former C++ offsets. The source now uses those offsets, a bounded
`nextInt(5000)` draw, real zero-Vector assignments instead of a null
function pointer, and clears both mission-crate flag bytes in `reset`.

The extracted-production cloak/reset smoke, other Fighter smokes, native
`gof2`, **206/206** ARM units and **37** verifier tests (one skip) pass.
Focused source-shape/strict fuzzy changes are `reset` **43.6%/16.8% →
93.2%/75.0%**, `cloak` **91.8%/60.5% → 100%/91.3%**, and
`setCloakingPossible` **96.0%/80.0% → 100%/100%**. `handleCloaking`
changes **87.0%/50.7% → 87.5%/50.1%**; its other behavior is not
closed. No exact match is claimed for these four functions.

Full ARM corpus remains **4,323/4,524** original functions,
**201 missing**, **123 extras**, zero skipped, **93.54%/82.57%**
average source-shape/strict fuzzy, **2,499 linked-exact** and
**1,074 byte-exact**. Both exact `(unit, symbol)` sets have zero gains
or losses against the preceding full report. This smoke uses mock
dependencies; original ARM32 cloak effects and the opaque `+0x2c0`
word remain unverified. See [the cloak/reset audit](PLAYER_FIGHTER_CLOAK_RESET_ARM_2026-10-15.md).

## PlayerFighter update prelude (2026-10-15)

REA/IDA Android ARM `0xdcfd8–0xdd116` confirms the initial death,
position and allegiance paths of `PlayerFighter::update`. The source now
calls `KIPlayer::setActive(false)` instead of an undefined helper on
the terminal death branch, reads position from the real geometry, and
updates both Player enemy/friend bytes including wingman, special-group
and damage-threshold branches. The source-level null stack guard was
removed; it is compiler instrumentation in the original. An extracted
production-method smoke covers these branches and signed `dt`.

Native `gof2`, all three Fighter smokes, **206/206** ARM units and
**37** verifier tests (one skip) pass. Focused `update` source-shape/
strict fuzzy improves **3.6%/2.2% → 5.3%/3.7%**; no exact match is
claimed. Full corpus remains **4,323/4,524** original functions,
**201 missing**, **123 extras**, zero skipped, **93.52%/82.55%**
average source-shape/strict fuzzy, **2,499 linked-exact** and **1,074
byte-exact** with zero exact-symbol losses. The original method has
about 3,556 instructions while the recovered ARM body now has 118;
the remaining `PF_update_body` is still an unresolved placeholder.
Do not treat full Fighter update or live ARM32 behavior as verified.
See [the prelude audit](PLAYER_FIGHTER_UPDATE_PRELUDE_ARM_2026-10-15.md).

## PlayerFighter constructor initial values (2026-10-15)

REA/IDA confirms the original constructor writes float bits
`0x3bf9fff6` to `shootError` at `+0x1a8` and words `0x443b8000` and
`0x41723ace` at `+0x294/+0x298`. These were incorrectly zero in C++.
The already-correct `setShootError(int)` was left untouched; the roles
of the two latter fields remain unclassified. Native `gof2`, both
PlayerFighter fragment smokes, **206/206** ARM units and **37** verifier
tests (one skip) pass. Constructor source-shape remains **94.5%**;
strict fuzzy improves **37.9% → 38.6%**, without exact equality.

Full corpus is unchanged at **4,323/4,524** original functions,
**201 missing**, **123 extras**, zero skipped and **93.52%/82.55%**
average source-shape/strict fuzzy. Exact `(unit, symbol)` sets have
zero losses or gains: **2,499 linked-exact**, **1,074 byte-exact**.
Live ARM32 construction remains untested; see
[the initial-values audit](PLAYER_FIGHTER_INITIAL_VALUES_ARM_2026-10-15.md).

## PlayerFighter constructor routes (2026-10-15)

REA/IDA and original ELF bytes confirm the constructor's bounded RNG
draws, 2–4 distinct route waypoints, the four-point shared station route,
looping and x/y/z position. The prior unbounded indices and null shared
pointer could corrupt memory or crash; both are removed. A production-code
route smoke covers all route lengths, duplicate-index retry, draw bounds,
shared-route reuse, coordinates and loop flags. The earlier `setLevel`
smoke also passes after updating its extraction boundary. Native `gof2`,
**206/206** ARM units and **37** verifier tests (one skip) pass.

Constructor source-shape improves **85.2% → 94.5%**; strict fuzzy is
**37.9%**, with no exact match. Full corpus remains **4,323/4,524**
original functions, **201 missing**, **123 extras**, zero skipped, and
**93.52%/82.55%** average source-shape/strict fuzzy. Both exact-symbol
sets are unchanged, with zero losses: **2,499 linked-exact** and
**1,074 byte-exact**. The full constructor and object lifecycle have not
run against the original ARM32 game; other dependent initialization
effects remain open. See [the route audit](PLAYER_FIGHTER_ROUTE_ARM_2026-10-15.md).

## PlayerFighter ABI and particle registration (2026-10-15)

REA/IDA on Android 2.0.16 confirms a **0x2f0-byte** PlayerFighter
allocation, its KIPlayer base fields, two Matrix subobjects and five
particle-system registrations in `setLevel`. The C++ layout now has ARM
size/offset assertions and the five calls use typed managers instead of
null function pointers. A production-method smoke records the selected
managers, particle sets, returned handles, reference matrix and disabled
emission. Native `gof2`, **206/206** ARM units, the smoke and **37** verifier
tests (one skip) pass.

The focused `setLevel` source-shape/strict fuzzy pair changes from
**6.6%/0.0% to 94.8%/82.0%**. The full corpus compares **4,323/4,524**
original functions, at **93.52%** average source-shape and **82.55%** strict
fuzzy; **201 missing**, **123 extras**, zero skipped. It has **2,499
linked-exact** and **1,074 byte-exact**, gains of 13 and 9 Fighter symbols
respectively, with no exact-symbol losses against the prior full report.
The four trailing bytes of the allocation remain unclassified. At this
stage the constructor still contained null placeholders; the route audit
above records their subsequent removal. Its live ARM32 path has not been
executed, so a working `Level::createShip` route is not yet established. See
[the Fighter ABI audit](PLAYER_FIGHTER_SET_LEVEL_ABI_ARM_2026-10-15.md).

## String reversal and historical fighter ABI gap (2026-10-15)

Android ARM `String::Reverse` copies the zero-terminated source before
writing characters in reverse by the stored length. The former in-place
swap disagreed for embedded zeros. The recovered body and a standalone
production-source smoke cover ordinary, locale-gated, embedded-zero and
empty inputs. Native `gof2`, 206/206 ARM units and 37 verifier tests
(one skip) pass. Focused source-shape/strict fuzzy improves
**36.3%/7.8% → 91.0%/62.3%**; no exact match is claimed. Full corpus
is **4,323/4,524** at **93.47%/82.37%**, with **201 missing**,
**123 extras**, zero skipped and unchanged exact sets of **2,486
linked-exact** and **1,065 byte-exact**. Live original ARM behavior is
not tested. At the time of that audit, `PlayerFighter::setLevel` still
called null placeholders and the derived type was **0x368** bytes despite
Android allocating **0x2f0**. The PlayerFighter section above supersedes
that historical gap; no fighter code was changed in the String audit. See
[the evidence and layout audit](STRING_REVERSE_ARM_2026-10-15.md).

## PaintCanvas camera creation and matrix ABI (2026-10-15)

Android ARM constructor/getter/setter instructions and the typed `Camera`
layout confirm that a constructed camera owns a `Matrix` subobject at
`+0x0c`; the `float[15]` camera view is only a raw overlay. The two
accessors are already linked-exact, so they were not rewritten.
`CameraCreate` now replaces two undefined helper calls with the real
five-argument `Camera` constructor and `ArrayAdd<Camera*>`, preserving
the original constants and width/height order. Native `gof2`, 206/206
ARM units and the extracted-production creation smoke pass. Focused
source-shape/strict fuzzy improves **76.8%/69.6% → 84.7%/75.7%**;
the function remains non-exact. Full corpus stays **4,323/4,524** at
**93.46%/82.36%**, with **201 missing**, **123 extras**, zero skipped,
**2,486 linked-exact** and **1,065 byte-exact**; both exact-symbol sets
are unchanged. Live Android/iOS rendering is still
unverified. See [the camera audit](PAINTCANVAS_CAMERA_CREATE_ARM_2026-10-15.md).

## ParticleSystemSprite camera-matrix render (2026-10-15)

REA/IDA Android ARM `0x18347c` shows two direct 15-float camera-matrix
copies before `DrawSpriteSystem`. Replacing default construction and
element loops with direct `Matrix` copies makes the two-argument render
overload **100% source-shape / 100% strict fuzzy** (previously
71.0%/29.8%) and adds one linked-exact match; it is not byte-exact.
Native `gof2` and 206/206 ARM translation units pass. The nearby
`calcEmitterVelocity` low source-shape was investigated but its existing
math and writes match ARM evidence, so it was not rewritten for the metric.
Full corpus remains **4,323/4,524** compared, **201 missing**, **123 extras**,
zero skipped; average source-shape/strict fuzzy improves **93.45%/82.35% →
93.46%/82.36%**. Linked-exact rises **2,485 → 2,486** with exactly this
symbol added and no losses; byte-exact stays **1,065** with no losses.
The `float[15]`/`Matrix` camera-buffer ABI contract has since been
confirmed for constructed cameras; a controlled Android/iOS render
observation remains open.
See [the render audit](PARTICLE_SPRITE_RENDER_ARM_2026-10-15.md).

## ParticleSystemSprite ordinary update (2026-10-15)

REA/IDA Android ARM `0x1830b4–0x183348` confirms a three-part correction
to `updateSingle`: expired sprites move to the `2^32` sentinel and size
zero; the previous UV frame uses integer `dt` and a `1/1024` wrap bias;
and position velocity
is scaled by `0.001`. The extracted-production host smoke now checks these
cases plus the previously recovered area-exit path. Native `gof2`, 206/206
ARM units and 37 verifier tests (one skip) pass. Focused source-shape/
strict fuzzy improves **87.3%/39.8% → 92.9%/49.0%**; no exact match
is claimed. Live renderer/timing and runtime setting overrides remain
unverified. Layout assertions confirm `colorFlag` at `+0x40`, `posBase`
at `+0x44` (the already-correct size input) and `posSpread` at `+0x48`.
Full ARM corpus compares **4,323/4,524** at **93.45%** average
source-shape and **82.35%** strict fuzzy, with **201 missing**, **123 extras**
and zero skipped. Both exact-symbol sets are unchanged: **2,485
linked-exact** and **1,065 byte-exact**, with zero losses.
See [the update audit](PARTICLE_SPRITE_UPDATE_ARM_2026-10-15.md).

## ParticleSystemSprite area-exit recovery (2026-10-15)

The former `updateAreaExitParticle` call to an undefined helper has been
replaced by the Android ARM-backed movement, squared-radius alpha bands,
four-draw random rebirth and velocity reset. An extracted-production host
smoke covers all branches and RNG state. Native `gof2`, 206/206 ARM units
and 37 verifier tests (one expected skip) pass. Focused source-shape/strict
fuzzy improves **1.8%/0.9% → 94.6%/66.7%** after correcting a prior
mistaken `+0x40` field identification; the method is still
non-exact. Full corpus remains **4,323/4,524** compared, with average
source-shape/strict fuzzy **93.43%/82.33% → 93.45%/82.34%**,
**2,485 linked-exact**, **1,065 byte-exact**, zero exact-symbol losses,
**201 missing**, **123 extras** and zero skipped in the historical report
before that correction. The new ordinary-update report supersedes these
corpus averages without changing either exact-symbol set. The ARM method
reads the float `colorFlag` at
`+0x40`; configured overrides and live renderer behavior remain
unverified. See
[the area-exit audit](PARTICLE_SPRITE_AREA_EXIT_ARM_2026-10-15.md).

## AERandom ARM-derived sequence check (2026-10-15)

REA/IDA Android ARM instructions for `next(int)`, `nextInt()` and
`nextInt(int)` now anchor a separate 32-bit limb oracle. An extracted
production smoke passed **2,304 draws**, **112 rejection iterations** and
state comparison after every draw across six seeds and representative bit
widths/bounds. The rejection condition now uses defined unsigned 32-bit
wraparound, matching the observed ARM signed-branch rule instead of relying
on C++ signed overflow. Native `gof2` and 206/206 ARM units are green. Focused
source-shape is **99.7%** across 11 symbols, with two linked-exact and two
byte-exact destructor symbols; `nextInt(int)` remains 96.4% source-shape.
This check does not execute the original `.so` or prove live particle
behavior. Full corpus remains **4,323/4,524** at **93.43%** source-shape,
**82.33%** strict fuzzy, **2,485 linked-exact** and **1,065 byte-exact**;
neither exact-symbol set lost a member. See
[the oracle audit](AERANDOM_ARM_ORACLE_2026-10-15.md).

## ARM32 ABI/lifetime device smoke (2026-10-15)

The existing isolated `gof2_abi_runtime_smoke` was rebuilt as an ELF32
ARM `armeabi-v7a` executable from this checkout and ran **5/5** times
on an Android 12 M2007J20CG. Each run passed 100 String/Array lifetime
iterations plus Vector by-value return, virtual dispatch and base-pointer
destruction. The temporary device file and drive mapping were removed;
UCRT64 host smoke and native `gof2` remain green. This is a real ARM32
execution gate for the reconstructed probe, **not** the original game,
not a sanitizer run and not a death-to-revival gameplay test. The latest
unchanged full corpus is **4,323/4,524** at **93.43%** source-shape and
**82.33%** strict fuzzy, with **2,485 linked-exact** and **1,065
byte-exact**. Next: original-derived ABI cases and a controlled
`explode`/`revive` runtime path. See [the ABI gate](ABI_RUNTIME_GATE_2026-09-27.md).

## PlayerEgo endExplosion (2026-10-15)

Android Thumb `0xa15c6–0xa15d2`, inspected through REA/IDA, confirms
a null check followed by `Explosion::reset()`; the former code called
`update(0, nullptr)`. An extracted-production smoke covers both paths.
Native `gof2`, 206/206 ARM units, adjacent explode/revive smokes and
37 verifier tests (one skipped) pass. Focused source-shape/strict fuzzy
improves **59.2%/36.4% → 100%/100%**, gaining linked-exact but not
byte-exact. Full coverage remains **4,323/4,524**; average
source-shape/strict fuzzy changes **93.42%/82.32% → 93.43%/82.33%**,
with **2,485 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. No direct source
caller was found; live Android/iOS animation reset remains unverified.
Next: controlled runtime/ABI death-to-revival test. See
[the explosion audit](PLAYER_EGO_EXPLODE_ARM_2026-10-15.md).

## PlayerEgo explode (2026-10-15)

Android Thumb `0xa13e0–0xa1488`, inspected through REA/IDA instructions,
restores the death path: always enable particle emission; on the first
call deactivate camera/player, allocate `Explosion(0)`, stop ten FMOD
events in original order and hide exhaust. The former code called
through null function/object globals and updated the new explosion
prematurely. An ARM32 size assertion guards the 0x68-byte Explosion
object. Extracted-production first/repeated-call smoke, adjacent revive
smoke, native `gof2`, 206/206 ARM units and 37 verifier tests (one
skipped) pass. Focused source-shape/strict fuzzy improves
**61.7%/49.5% → 100%/100%**; `explode` is still non-exact.
Full coverage remains **4,323/4,524**, with average source-shape/
strict fuzzy **93.41%/82.31% → 93.42%/82.32%**. Linked-exact
**2,484**, byte-exact **1,065**, missing **201**, extras **123**, zero
skipped and both exact symbol sets unchanged. Live Android ARM32 and
iOS death-flow behavior remain unverified. Next: controlled runtime/
ABI test of `explode`/`revive`. See
[the explode audit](PLAYER_EGO_EXPLODE_ARM_2026-10-15.md).

## PlayerEgo revive (2026-10-15)

Android Thumb `0xa14a4–0xa1586`, inspected through REA/IDA instructions,
confirms unconditional particle disablement, explosion cleanup, camera/player
reactivation, both `Globals::sound` events, full hull/armor restoration,
position `(0,0,10000)`, direction reset and timer clear. The previous
code could dereference an uninitialized sound pointer and did not relocate
the ship. ARM32 field-offset assertions cover the sound ID, explosion and
particle system. Extracted-production smoke (with and without explosion),
adjacent tests, native `gof2`, 206/206 ARM units and 37 verifier tests
(one skipped) pass. Focused source-shape/strict fuzzy improves
**88.4%/64.9% → 93.0%/70.3%**; the function remains non-exact.
The full corpus stays at **4,323/4,524** compared, **93.41%** average
source-shape, **82.31%** strict fuzzy, **2,484 linked-exact** and
**1,065 byte-exact**, with **201 missing**, **123 extras**, zero skipped
and no exact-pair losses. Live Android ARM32 and iOS revive behavior is
untested. Next: an ARM32 runtime/ABI probe, then a controlled iOS guest
trace. See [the revive audit](PLAYER_EGO_REVIVE_ARM_2026-10-15.md).

## PlayerEgo autopilot waypoint distance (2026-10-15)

Android Thumb `0x9e344–0x9e3e8`, checked with REA/IDA pseudocode and
instructions, restores the target Vector at `+0xec`, the difference
from the actual player position, and the truncated Euclidean distance
test (`<=19999`). The previous code read an uninitialized array,
shifted the stored waypoint by one field, and compared squared distance
against 20000. An extracted-production test covers the waypoint,
near/far and truncation boundaries. Native `gof2`, 206/206 ARM units,
adjacent smokes and 37 verifier tests (one skipped) pass. Focused
`update` source-shape/strict fuzzy changes **80.1%/30.8% →
80.6%/30.8%**; it remains non-exact. The full corpus is
**4,323/4,524** at **93.41%** average source-shape and **82.31%**
strict fuzzy, with **2,484 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, zero skipped and no exact-pair loss.
Live ARM32 waypoint behavior and the analogous iOS path are untested.
Next: controlled iOS guest trace and the remaining PlayerEgo update
branches. See [the autopilot audit](PLAYER_EGO_AUTOPILOT_DISTANCE_ARM_2026-10-15.md).

## PlayerEgo jump-drive charge in update (2026-10-15)

Android Thumb `0x9d8a6–0x9d8f6`, inspected through REA/IDA, restores
per-frame charge growth, threshold HUD event 26 and volatile-cargo shake.
The ARM field offsets `+0x1fc/+0x200/+0x204` are asserted. The low-byte
store to `+0x38` is preserved exactly, but its higher-level purpose is not
yet established. Native `gof2`, 206/206 ARM units, extracted and adjacent
smokes and 37 verifier tests (one skipped) pass. Focused `update`
source-shape/strict fuzzy changes **79.5%/32.3% → 80.1%/30.8%**;
it remains non-exact. The full corpus is **4,323/4,524** compared at
**93.41%** average source-shape and **82.31%** strict fuzzy, with
**2,484 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. The iOS ARMv7
emulator may allow controlled guest breakpoints on the analogous path;
that cross-version behavior has not yet been measured. See
[the charge audit](PLAYER_EGO_DRIVE_CHARGE_ARM_2026-10-15.md).

## PlayerEgo jump-drive initialization and activation (2026-10-15)

Android Thumb constructor `0x99a30–0x99a38`, `0x99d56–0x99dcc` and
`startJumpDrive` `0x9a6c4–0x9a700`, inspected through REA/IDA, confirm
the initial drive state, equipment sort 18 / attribute 37, integrated
item 85 fallback, sounds 32/33, and charging-event order. Extracted
production and adjacent smokes, native `gof2`, 206/206 ARM units and
37 verifier tests (one skipped) pass. Focused constructor source-shape/
strict fuzzy rises **73.9%/41.6% → 77.8%/47.3%** for both aliases;
`startJumpDrive` rises **91.9%/85.1% → 100%/96.2%**. None becomes
exact. Full coverage remains **4,323/4,524** at **93.41%** average
source-shape and **82.31%** strict fuzzy, with **2,484 linked-exact**,
**1,065 byte-exact**, **201 missing**, **123 extras**, zero skipped
and no exact-pair losses. Live ARM32 jump/audio remains unverified.
Next: jump-charge update and flight transition. See
[the jump-drive audit](PLAYER_EGO_JUMP_DRIVE_ARM_2026-10-15.md).

## PlayerEgo boost setup and activation (2026-10-15)

Android Thumb `0x999b8`, `0x99a6e`, `0x99b20–0x99bbc` and
`0x9aea4–0x9aefa` restores boost availability, sound selection and
activation through `Globals::sound` at zero gain. The former code
left gates uninitialized and dereferenced an unconnected FMOD
pointer. Native `gof2`, 206/206 ARM units, extracted-method and
adjacent smokes, and 37 verifier tests (one skipped) pass.
Focused constructor source-shape/strict fuzzy rises **68.2%/41.3%
→ 73.9%/41.6%** for both aliases; `boost()` rises
**90.1%/67.8% → 100%/100%** and becomes linked-exact. Full
coverage remains **4,323/4,524** at **93.41%** average
source-shape and **82.30%** strict fuzzy, with **2,484 linked-exact**,
**1,065 byte-exact**, **201 missing**, **123 extras**, zero skipped
and no exact-pair losses. Live ARM32 boost/audio is unverified.
Next: jump-drive initialization. See
[the boost audit](PLAYER_EGO_BOOST_ARM_2026-10-15.md).

## PlayerEgo cloak-state constructor (2026-10-15)

Android Thumb `0x99a12`, `0x99cdc–0x99d4e` and
`0x99f56–0x99f94` restore inactive cloak state, integrated/ship
equipment selection, attributes 35/36, sound 30 and a
difficulty-dependent recharge maximum. Native `gof2`, 206/206 ARM
units, extracted-fragment and adjacent smokes, and 37 verifier
tests (one skipped) pass. Focused constructor source-shape/strict
fuzzy rises **61.0%/35.2% → 68.2%/41.3%** for both aliases;
neither is exact. Full coverage remains **4,323/4,524** at
**93.40%** average source-shape and **82.29%** strict fuzzy, with
**2,483 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. Live ARM32
cloak/material behavior and other equipment state remain
unverified. Next: jump-drive/boost initialization. See
[the cloak-constructor audit](PLAYER_EGO_CLOAK_CONSTRUCTOR_ARM_2026-10-15.md).

## PlayerEgo supernova-protection constructor (2026-10-14)

Android Thumb `0x99a02–0x99a0a` and `0x99f4a–0x9a048`
restores inactive sound/geometry state and the conditional
supernova-protection sound and two-child effect. Native `gof2`,
206/206 ARM units, extracted-tail and related smokes, and 37
verifier tests (one skipped) pass. Focused constructor source-shape/
strict fuzzy rises **45.1%/28.3% → 61.0%/35.2%** for both aliases;
neither is exact. Full coverage remains **4,323/4,524** at
**93.40%** average source-shape and **82.29%** strict fuzzy, with
**2,483 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. Live ARM32
render/audio and the constructor's other missing state remain
unverified. See [the constructor audit](PLAYER_EGO_SUPERNOVA_CONSTRUCTOR_ARM_2026-10-14.md).

## PlayerEgo engine-sound dispatch (2026-10-14)

Android Thumb `0x9a550–0x9a5d0` and `0x9a5ec–0x9a676` confirms
the gamma-protection sound is played through `Globals::sound`, and
`PlayEngineSound` passes the ship resource at `+0x1c` to `Player`.
The former unconnected FMOD pointer and zero resource could break
the live engine-audio path. Native `gof2`, 206/206 ARM units,
extracted-method and adjacent smokes, and 37 verifier tests (one
skipped) pass. Focused `PlayEngineSound` source-shape/strict fuzzy
rises **94.0%/75.5% → 98.5%/87.4%**; `StopEngineSound` rises
**94.7%/76.9% → 98.2%/86.2%**. Neither is exact. Full coverage
remains **4,323/4,524** at **93.39%** average source-shape and
**82.29%** strict fuzzy, with **2,483 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras**, zero skipped and no
exact-pair losses. Audible behavior on live ARM32 remains
unverified. See [the engine-sound audit](PLAYER_EGO_ENGINE_SOUND_ARM_2026-10-14.md).

## PlayerEgo emergency-system activation (2026-10-14)

Android Thumb `0xa106c–0xa10f0` confirms the geometry/timer/hitpoint
gates and the transition that consumes equipment sort 27 and plays
sound 1115 via `Globals::sound` with zero gain. The previous sound
receiver was disconnected and its gain was wrong. Native `gof2`,
206/206 ARM units, extracted-method and related smokes, and 37
verifier tests (one skipped) pass. Focused source-shape/strict fuzzy
rises **90.4%/79.2% → 100%/100%**, gaining one linked-exact pair.
Full coverage remains **4,323/4,524** at **93.39%** average
source-shape and **82.28%** strict fuzzy, with **2,483 linked-exact**,
**1,065 byte-exact**, **201 missing**, **123 extras**, zero skipped and
no exact-pair losses. Live ARM32 behavior and emergency-geometry
ownership across ship replacement or destruction remain unverified;
the latter is a next bounded priority. See
[the activation audit](PLAYER_EGO_EMERGENCY_START_ARM_2026-10-14.md).

## PlayerEgo emergency-state constructor (2026-10-14)

Android Thumb `0x99946–0x9994c` and `0x99a82–0x99a86` confirm
zero-initialization of the emergency offset vector, timer, duration
and geometry pointer before `setShip`. The extracted-constructor
smoke, native `gof2`, 206/206 ARM units, related smokes and 37
verifier tests (one skipped) pass. Focused constructor source-shape/
strict fuzzy rises **44.8%/28.2% → 45.1%/28.3%** for both aliases;
neither is exact. Full coverage remains **4,323/4,524** at
**93.39%** average source-shape and **82.28%** strict fuzzy, with
**2,482 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. The ARM
destructor has no `+0xac` cleanup, so none was invented; ownership
outside active expiry remains uncertain. Next: the emergency-start
sound and equipment transition. See
[the constructor audit](PLAYER_EGO_EMERGENCY_CONSTRUCTOR_ARM_2026-10-14.md).

## PlayerEgo emergency visual lifecycle (2026-10-14)

Android Thumb `0x9e60c–0x9e748` restores the late emergency-system
timer, invulnerability, effect transform/rotation/scale, and expiry
cleanup. The earlier standalone timer decrement is removed. Native
`gof2`, 206/206 ARM units, production-fragment smoke, related cloak/
setShip/render smokes and 37 verifier tests (one skipped) pass.
Focused `update` source-shape/strict fuzzy rises **77.0%/31.1% →
79.5%/32.3%**; the function remains non-exact. Full coverage stays
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, zero skipped and no exact-pair losses.
Live ARM32 rendering and unused-geometry ownership on ship replacement
or destruction remain unverified; the ownership path is next. See
[the emergency-lifecycle audit](PLAYER_EGO_EMERGENCY_LIFECYCLE_ARM_2026-10-14.md).

## PlayerEgo active cloak lifecycle (2026-10-14)

Android Thumb `0x9cfe2–0x9d002` and `0x9d310–0x9d8a6` restore
the cloak recharge timer, active fade, child visibility, Status
duration counter, material restoration and recharge transition.
The previous event-46 timer binding to the emergency system was
wrong; the old checkpoint notes are corrected. Native `gof2`,
206/206 ARM units, two production-fragment smokes and 37 verifier
tests (one skipped) pass. Focused `update` source-shape/strict
fuzzy rises **68.2%/20.4% → 77.0%/31.1%**; the function remains
non-exact. Full coverage stays **4,323/4,524** at **93.39%** average
source-shape and **82.28%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **201 missing**, **123 extras**, zero skipped
and no exact-pair losses. Live ARM32 rendering and the later
emergency visual teardown are unverified; the latter is next.
See [the cloak-lifecycle audit](PLAYER_EGO_CLOAK_LIFECYCLE_ARM_2026-10-14.md).

## PlayerEgo original material mode (2026-10-14)

Android Thumb `0x9a29a–0x9a2a6` confirms `setShip` saves the integer
`Material::materialMode` to `PlayerEgo+0x394`; the previous code
treated that value as a pointer. The production-fragment smoke,
native `gof2`, 206/206 ARM units and 37 verifier tests (one skipped)
pass. Focused `setShip` source-shape/strict fuzzy changes
**98.2%/91.3% → 97.8%/79.2%** because this is a behavior-backed
correction, not an exact-match gain. Full coverage remains
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, zero skipped and no exact-pair
losses. Live ARM32 material restoration remains unverified. Next:
cloak fade, visibility and restoration in `PlayerEgo::update`. See
[the material-mode audit](PLAYER_EGO_ORIGINAL_MATERIAL_MODE_ARM_2026-10-14.md).

## PlayerEgo cloak-charge transition (2026-10-14)

Android Thumb `0x9d492–0x9d4c8` restores the missing per-frame
charge and automatic activation of the cloak after a strict
`cloakDischargeMax` threshold, preserving HUD-event and flag order.
The extended production-fragment smoke, native `gof2`, 206/206 ARM
units and 37 verifier tests (one skipped) pass. Focused `update`
source-shape rises **67.5% → 68.2%**; strict fuzzy changes
**20.5% → 20.4%**, with no exact match. Full coverage stays
**4,323/4,524** at **93.39%** average source-shape and **82.28%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, zero skipped and no exact-pair
losses. Live ARM32 behavior remains unverified. Next: cloak fade,
visibility and material restoration. See
[the cloak-charge audit](PLAYER_EGO_CLOAK_CHARGE_ARM_2026-10-14.md).

## PlayerEgo beam updates (2026-10-14)

Android Thumb `0x9d8fe–0x9d93c` restores the missing per-frame
`TractorBeam::update` and `RepairBeam::update` dispatch in
`PlayerEgo::update`, after the constructor and render paths were
already present. The production-fragment smoke, native `gof2`,
206/206 ARM units and 37 verifier tests (one skipped) pass.
Focused source-shape improves **66.1% → 67.5%**, while strict fuzzy
falls **28.6% → 20.5%**; the full method is still incomplete and
non-exact. Full coverage stays **4,323/4,524** at **93.39%**
average source-shape and **82.28%** strict fuzzy, with **2,482
linked-exact**, **1,065 byte-exact**, **201 missing**, **123 extras**,
zero skipped and no exact-pair losses. Live ARM32 effects and
ownership of the emergency geometry at `+0xac` remain unverified.
Next: adjacent cloak/material and transform updates. See
[the beam-update audit](PLAYER_EGO_BEAM_UPDATE_ARM_2026-10-14.md).

## PlayerEgo setShip emergency and cloak tail (2026-10-14)

Android Thumb `0x9a3f6–0x9a498` restores sort-27 attribute 41
for emergency-system setup and clones the hull material into the
cloak handle at `+0x388`, replacing a vector copy over material
slots. The production-fragment smoke, native `gof2`, 206/206 ARM
units and 37 verifier tests (one skipped) pass. Focused `setShip`
source-shape/strict fuzzy rises **97.4%/90.6% → 98.2%/91.3%**;
it is non-exact. Full coverage stays **4,323/4,524** at
**93.39%** average source-shape and **82.28%** strict fuzzy, with
**2,482 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras**, zero skipped and no exact-pair losses. Whole-ship
runtime behavior remains unverified. Next: repair-beam/emergency
geometry lifecycle. See
[the setShip-tail audit](PLAYER_EGO_SET_SHIP_TAIL_ARM_2026-10-14.md).

## PlayerEgo cloak activation (2026-10-14)

Android Thumb `0x9a708–0x9a9b0` restores cloak attribute 38,
amount-limited cargo removal, HUD event 30's amount, sound 30 via
`Globals::sound`, and shader channels 1/2 for hull and turret
materials. The production-method smoke, native `gof2`, 206/206 ARM
units and 37 verifier tests (one skipped) pass. Focused source-shape
rises **92.5% → 94.2%**, but strict fuzzy falls **56.5% → 23.0%**;
the function remains non-exact. Full coverage stays **4,323/4,524**
at **93.39%** average source-shape and **82.28%** strict fuzzy
(down 0.01 point), with **2,482 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, zero skipped and no exact-pair
losses. Live ARM32 cloak behavior and visual output remain
unverified. Next: `setShip` cloak-material lifecycle. See
[the cloak audit](PLAYER_EGO_CLOAK_ARM_2026-10-14.md).

## PlayerEgo render dispatch (2026-10-14)

Android Thumb `0xa1b78–0xa1c56` restores the dead/live render
branches, the `+0x309` visibility gate, emergency and turret
geometry, repair-beam order, and the frozen/HUD moving-stars rule.
The production-method smoke, native `gof2`, 206/206 ARM units and
37 verifier tests (one skipped) pass. Focused `render` source-shape/
strict fuzzy improves **86.8%/41.4% → 97.2%/60.4%** without an
exact match. Full coverage remains **4,323/4,524** at **93.39%**
average source-shape and **82.29%** strict fuzzy, with **2,482
linked-exact**, **1,065 byte-exact**, **201 missing**, **123 extras**,
zero skipped and no exact-pair losses. Live ARM32 rendering and
ownership of the `+0xac` geometry remain unverified. Next:
`toggleCloaking` material/state branches. See
[the render audit](PLAYER_EGO_RENDER_ARM_2026-10-14.md).

## PlayerEgo turret construction and stream return (2026-10-14)

Android Thumb `0x9b340–0x9b7a4` restores turret equipment queries,
mesh offsets, transform-handle parenting, animation/visibility,
rotation and cloak material slots; both `addGun` overloads again call
`checkForTurret`. The base geometry is at `PlayerEgo+0xdc`, not the
old `+0xd8` alias. The adjacent `dockToStream` fix uses the original
`+0xe0/+0xe4/+0xe8` return position and clears autopilot. The
production-method smoke, native `gof2`, 206/206 ARM units, and 37
verifier tests (one skipped) pass. Focused source-shape rises from
86.5% to 95.2% for `checkForTurret`, 81.4% to 100% for
`dockToStream`, and 90.7% to 100% for each `addGun` overload.
Full coverage remains **4,323/4,524** at **93.38%** average
source-shape and **82.28%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **201 missing**, **123 extras**, zero skipped,
and no exact-pair losses. Live ARM32 behavior and unidentified
`+0xd8` semantics remain unverified. Next: audit turret render and
cloak consumers against the corrected geometry addresses. See
[the turret audit](PLAYER_EGO_TURRET_CONSTRUCTION_ARM_2026-10-14.md).

## PlayerEgo pitch/yaw ramps (2026-10-14)

Android Thumb `0xa08a2–0xa08f2`, `0xa0d58–0xa0da4` and
`0xa0fc6–0xa1012` replace declared-only ramp calculations in
`right`, `left`, `down` and `up`. Cursor steering, the two option
sensitivity slots and direction-specific pitch factors are restored;
`right` also stores base handling into `yawRamp` as ARM does. The
production-helper smoke, native `gof2`, 206/206 ARM units and 37
verifier tests (one skipped) pass. Focused average source-shape/strict
fuzzy rises **80.3%/58.9% → 88.0%/69.8%**; none is exact. Full
coverage remains **4,323/4,524** at **93.37%** average source-shape
and **82.26%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped and no exact-pair
losses. Live input feel and ARM32 runtime remain unverified. Next
priority: the turret placement/material helpers in `checkForTurret`.
See [the axis-ramp audit](PLAYER_EGO_AXIS_RAMP_ARM_2026-10-14.md).

## PlayerEgo asteroid approach stage zero (2026-10-14)

Android Thumb `0x9f5ac–0x9f8de` replaces the declared-only approach
step with distance, steering speed, near-target flag, camera/exhaust
entry and alignment rotation. The target byte at `+0x3c` is still
accessed by offset because its semantic field name is unresolved.
The expanded production-body smoke, native `gof2`, 206/206 ARM units
and 37 verifier tests (one skipped) pass. Focused `approachAsteroid`
source-shape/strict fuzzy rises **40.8%/21.6% → 91.9%/47.3%**; it is
not exact. Full coverage remains **4,323/4,524** at **93.37%**
average source-shape and **82.26%** strict fuzzy, with **2,482
linked-exact**, **1,065 byte-exact**, **123 extras**, zero skipped and
no exact-pair losses. Live approach/mining and ARM32 runtime remain
unverified. The declared-only pitch/yaw ramp helpers used by ship
controls are recovered in the section above. See
[the asteroid-approach audit](PLAYER_EGO_ASTEROID_APPROACH_STAGE0_ARM_2026-10-14.md).

## PlayerEgo docking-point ease (2026-10-14)

Android Thumb `0x9f9f4–0x9fcb4` confirms the navigation-point matrix
translation and 3,000 ms `EaseInOutMatrix` construction. The
declared-only ease helper is replaced; the same branch now uses the
ARM-backed global canvas, level argument and look-at camera mode.
The production-body smoke, native `gof2`, 206/206 ARM translation units
and 37 verifier tests (one skipped) pass. Focused
`dockToDockingPoint` source-shape/strict fuzzy rises
**77.7%/54.3% → 97.7%/66.4%**; it is not exact. Full coverage remains
**4,323/4,524** at **93.36%** average source-shape and **82.25%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. Live docking
and ARM32 runtime remain unverified. Next priority:
`approachAsteroid`'s declared-only `PE_aa_approachStep` path, recovered
in the section above. See
[the docking-point ease audit](PLAYER_EGO_DOCK_POINT_EASE_ARM_2026-10-14.md).

## PlayerEgo maneuver update (2026-10-14)

Android Thumb `0x9c1f8–0x9c684` replaces the declared-only dodge and
strafe shims with movement, rotation, sway, camera and completion
behavior. The timer at `+0x350` now advances by frame time from
`+0x134`, and `initManeuver` initializes that same field. Production-body
smokes, native `gof2`, 206/206 ARM translation units and 37 verifier
tests (one skipped) pass. Focused `updateManeuver` source-shape/strict
fuzzy rises **30.1%/20.0% → 93.0%/51.7%**; it is not exact. Full
coverage remains **4,323/4,524** at **93.35%** average source-shape
and **82.25%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped and no exact-pair
losses. Live maneuver animation and ARM32 runtime remain unverified.
The adjacent docking-point ease-matrix helper is recovered in the
section above.
See [the maneuver audit](PLAYER_EGO_UPDATE_MANEUVER_ARM_2026-10-14.md).

## PlayerEgo move-to-position (2026-10-14)

Android Thumb `0x9c6d4–0x9ca52` establishes the destination/current
direction blend, heading, optional signed steering and five-slot
history, forward travel, rotated strafe, and Player/position-cache
updates. The old body delegated steering to a declared-only shim.
The constructor now initializes the history and steering parameters;
`setAutoPilot(nullptr)` resets the index at `+0x2a4` rather than the
limit at `+0x284`. The production-body smoke, native `gof2`, 206/206
ARM translation units and 37 verifier tests (one skipped) pass.
Focused `moveToPosition` source-shape/strict fuzzy rises
**43.8%/22.2% → 92.9%/48.0%**; no exact match. Full coverage stays
**4,323/4,524** at **93.34%** average source-shape and **82.24%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. Live flight and
ARM32 runtime remain unverified. See
[the move-to-position audit](PLAYER_EGO_MOVE_TO_POSITION_ARM_2026-10-14.md).

## PlayerEgo ship control (2026-10-14)

Android Thumb `0x9bdf0–0x9c12c` establishes engine sound parameters,
pitch/yaw and roll matrix composition, forward travel, normalized
basis, strafe and follow-camera translation, and accumulator resets.
The previous body delegated orientation to a declared-only helper and
used an uninitialized sound pointer. The adjacent `handleTurretView`
also incorrectly cleared direction flags instead of angle accumulators;
that offset is corrected. Production-body host smokes, native `gof2`,
206/206 ARM translation units and 37 verifier tests (one skipped) pass.
Focused `handleShip` source-shape/strict fuzzy rises **40.8%/23.7% →
92.1%/60.4%**; `handleTurretView` improves to **97.9%/80.8%**. Neither
is exact. Full coverage remains **4,323/4,524**, with average
source-shape **93.33%**, strict fuzzy **82.23%**, **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped, and no exact-pair
losses. Live ship motion and ARM32 runtime remain unverified. See
[the ship-control audit](PLAYER_EGO_HANDLE_SHIP_ARM_2026-10-14.md).

## PlayerEgo roll (2026-10-14)

Android Thumb `0x9bbf8–0x9bd94` establishes the bank-rate table,
direction reversal and autolevel transitions, exact-level reset and
Matrix rotation contract. The previous body hid most behavior behind
a declared-only helper and accepted an incorrect zero-component
boundary. The recovered production-body smoke, native `gof2`,
206/206 ARM translation units and verifier tests pass. Focused
source-shape/strict fuzzy rises **60.8%/38.6% → 92.2%/67.5%**;
no exact match. Live roll animation and ARM32 runtime remain
unverified. Full coverage remains **4,323/4,524** at **93.32%**
average source-shape and **82.23%** strict fuzzy, with **2,482
linked-exact**, **1,065 byte-exact**, **123 extras**, zero skipped
and no exact-pair losses. See
[the roll audit](PLAYER_EGO_ROLL_ARM_2026-10-14.md).

## PlayerEgo turret-view camera (2026-10-14)

Android Thumb `0x9b96c–0x9bbb0` confirms the two movement gates,
three geometry matrices, hit/boost shake, camera handle at `+0x174`
and Player transform assignment. The previous body mis-gated movement
and passed a canvas pointer as the camera handle. The recovered body
and production-body smoke, native `gof2`, 206/206 ARM units and
verifier tests pass. Focused source-shape/strict fuzzy rises
**80.8%/59.3% → 97.8%/80.4%**; no exact match. Live camera feel and
ARM32 runtime remain unverified. Full coverage remains
**4,323/4,524** at **93.31%** average source-shape and **82.22%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. See
[the turret-view audit](PLAYER_EGO_TURRET_VIEW_ARM_2026-10-14.md).

## PlayerEgo auto-turret aim/fire (2026-10-14)

The Android post-selection path now replaces the declared-only shim:
target lead, local aim, horizontal/vertical turret movement, aligned
weapon-2 firing, transform updates and timeout stop. The adjacent
`KIPlayer::noTargetFlag` overlay is corrected from `+0x6c` to Android's
`+0x74`. Production-body host smoke, native `gof2`, 206/206 ARM
translation units and verifier tests pass. Focused source-shape/strict
fuzzy rises **45.7%/24.0% → 93.8%/54.1%**. Full coverage remains
**4,323/4,524** at **93.31%** average source-shape and **82.22%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. Live aiming,
animation side effects and ARM32 runtime remain unverified. See
[the aim/fire audit](PLAYER_EGO_AUTO_TURRET_AIM_ARM_2026-10-14.md).

## PlayerEgo auto-turret selection (2026-10-14)

Android `handleAutoTurret` at `0x9ca78` obtains each eligible enemy's
virtual position, subtracts it from the PlayerEgo position cache and
compares integer distance with a 60,000-unit limit. The source had
discarded that position and used an uninitialized array. The
no-target/timeout path now stops weapon `2` as ARM specifies. A
production-body host smoke, native `gof2`, 206/206 ARM units and
verifier tests pass. Focused source-shape stays **45.7%**; strict
fuzzy rises **23.5% → 24.0%**. Full coverage remains
**4,323/4,524** at **93.30%** average source-shape and **82.21%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. The aim/fire
tail still uses an undefined shim; live aiming and ARM32 runtime are
unverified. See [the selection audit](PLAYER_EGO_AUTO_TURRET_SELECTION_ARM_2026-10-14.md).

## PlayerEgo dock-vector reset (2026-10-14)

Android `dockToAsteroid` and `dockToDockingPoint` each zero a
12-byte vector at PlayerEgo `+0x1c8` through `Vector::operator=`.
The header now exposes that overlapping view while preserving its
three integer fields and ARM offsets; both paths no longer call an
undefined `Vec_assign`. The existing docking smoke, native `gof2`,
206/206 ARM units and verifier tests pass. Focused source-shape and
strict fuzzy stay **99.4%/88.2%** and **77.7%/54.3%** respectively;
neither function is exact. Full coverage stays **4,323/4,524** at
**93.30%** average source-shape and **82.21%** strict fuzzy, with
**2,482 linked-exact**, **1,065 byte-exact**, **123 extras**, zero
skipped and no exact-pair losses. Live docking and ARM32 runtime are
unverified. See [the vector audit](PLAYER_EGO_DOCK_VECTOR_ARM_2026-10-14.md).

## PlayerEgo maneuver target (2026-10-14)

The Android `initManeuver` literal at `0xa1b70` is `10000.0f`, not the
zero previously used for the type-3 strafe target. This function now
uses the existing vector operators instead of declared-only `Vec_*`
wrappers. Its production-body host smoke, native `gof2`, 206/206 ARM
units and verifier tests pass. Focused source-shape/strict fuzzy rises
**93.1%/78.5% → 99.4%/89.4%**; no exact match. Full coverage is
**4,323/4,524** at **93.30%** average source-shape and **82.21%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras** and zero skipped. Exact-pair sets have no losses.
Other `PlayerEgo` vector wrappers and live ARM32 maneuver behavior
remain unverified. See [the maneuver audit](PLAYER_EGO_MANEUVER_ARM_2026-10-14.md).

## StarSystem flare handoff (2026-10-14)

Android `StarSystem::render2D` at `0x1354e8` uses `Globals::Canvas`
for the camera and draws only after successful screen projection.
Both dependencies are restored; a production-body host smoke covers
absent flare, failed projection and successful projected draw. Native
`gof2`, 206/206 ARM units and verifier tests pass. Focused source-shape
improves **95.6% → 98.6%**; strict fuzzy falls **67.2% → 64.4%**,
and the function is not exact. Full coverage remains **4,323/4,524**
at **93.29%** average source-shape and **82.21%** strict fuzzy, with
**2,482 linked-exact**, **1,065 byte-exact**, **123 extras** and zero
skipped; no exact-pair losses. A live graphics frame and ARM32 runtime
remain unverified. See [the handoff audit](STARSYSTEM_FLARE_HANDOFF_ARM_2026-10-14.md).

## LensFlare render2D recovery (2026-10-14)

Android `render2D` at `0x11d088` confirms seven image passes, their
signed offsets/scales, per-pass colors, ordered screen/depth guards,
shared image canvas, and input-canvas color save/fill/restore. The
null canvas alias and declared-only `LensFlare_*` calls are removed.
A production-body host smoke covers centered/displaced flares and
no-draw guards; native `gof2`, 206/206 ARM units and verifier tests
pass. Focused source-shape/strict fuzzy rises **64.2%/15.5% →
94.8%/48.7%**; the function is not exact. Full coverage is
**4,323/4,524** at **93.29%** average source-shape and **82.21%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. Live GL and
ARM32 behavior remain unverified. Next: validate a real
StarSystem-to-LensFlare frame. See
[the render audit](LENS_FLARE_RENDER_ARM_2026-10-14.md).

## LensFlare constructor canvas binding (2026-10-14)

Android `LensFlare::LensFlare` at `0x11d00c` loads image resources
1288–1290 through `Globals::Canvas` and reads dimensions from its
input canvas. The null local pointer and declared-only constructor
helpers are replaced by real `PaintCanvas` calls. A production-body
host smoke checks both canvas roles and the three outputs; native
`gof2`, 206/206 ARM units and verifier tests pass. Both constructor
aliases rise **91.0%/46.2% → 100%/100%** source-shape/strict fuzzy
and become linked-exact, not byte-exact. Full coverage is
**4,323/4,524** at **93.29%** average source-shape and **82.20%**
strict fuzzy, with **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, zero skipped and no exact-pair losses. `render2D`
and live GL behavior remain unverified; next recover that body. See
[the constructor audit](LENS_FLARE_CONSTRUCTOR_ARM_2026-10-14.md).

## SystemPathFinder route state and node release (2026-10-14)

Android `getSystemPath` at `0x11b7a8` uses `Globals::status` for system
visibility and releases the complete node array after constructing a
successful path. The source's null local status pointer and partial
node cleanup are corrected. A production-body host smoke checks a
two-jump path, jump distance and an invisible destination; native
`gof2`, 206/206 ARM units and verifier tests pass. Focused source-shape
improves **95.4% → 96.0%** while strict fuzzy falls **67.6% → 59.1%**;
the function is not exact. Full coverage is **4,323/4,524** at
**93.28%** average source-shape and **82.18%** strict fuzzy, with
**2,480 linked-exact**, **1,065 byte-exact**, **123 extras**, zero
skipped and no lost exact pair. The newly emitted node-array release
specialization adds one linked-exact pair. Repeated no-route queries,
live StarMap behavior and ARM32 runtime remain unverified. See
[the pathfinder audit](SYSTEM_PATH_FINDER_ROUTE_ARM_2026-10-14.md).

## Save-preview level label (2026-10-14)

`MenuTouchWindow::createRecordButtons` now renders GameText ID 321,
`: ` and the saved integer level. The old source misread `GameRecord+0x20`
as a `String`; Android IDA and Thumb, plus record serialization and
`Status::setLevel`, establish its actual type. Extracted production
expression smoke, native `gof2`, 206/206 ARM units and verifier tests
pass. Focused source-shape/strict fuzzy improves **89.5%/43.6% →
89.9%/44.4%**; no exact match. The full report compares
**4,323/4,524** functions at **93.28%** average source-shape and
**82.18%** strict fuzzy, with **2,479 linked-exact**, **1,065
byte-exact**, **123 extras**, zero skipped, and no exact-pair losses.
Full save/load interaction, asset-backed display and ARM32 runtime
remain unverified. Next: preview rank and row ownership. See
[the label audit](MENUTOUCH_PREVIEW_LEVEL_ARM_2026-10-14.md).

## Layout save-menu button and row metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes the previously zero-filled
`+0x294..+0x2ac` group copied by `MenuTouchWindow` for button widths,
row gaps and save-list placement. IDA platform branches and Thumb
scalar/vector stores confirm ordinary low/Retina, N9 and iPad HD
values. The production constructor smoke checks seven profiles.
Native `gof2`, 206/206 ARM units, three Layout/TouchButton smokes and
37 verifier tests (one skipped) pass. Focused C1/C2 source-shape rises
**41.0% → 43.2%**, while strict fuzzy falls **9.5% → 9.0%**; neither
alias is exact. Full report
`_work/layout_save_menu_metrics_full_20261014.json` compares
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact pair
sets have no losses or gains against the previous Layout checkpoint.
Live save-menu interaction, asset-backed drawing and ARM32 runtime
remain unverified. Next: audit `MenuTouchWindow` save/load interaction
and exercise the populated metrics. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout Wanted-window width metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes the previously zero-filled
`+0xa0..+0xac` words and `+0xcc` width used by `WantedWindow`'s
detail button. IDA platform branches and Thumb scalar/vector stores
confirm seven tested profiles. Native `gof2`, 206/206 ARM units, three
Layout/TouchButton smokes and 37 verifier tests (one skipped) pass.
Focused C1/C2 source-shape/strict fuzzy improves **39.9%/9.1% →
41.0%/9.5%**; neither alias is exact. Full report
`_work/layout_wanted_width_metrics_full_20261014.json` compares
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact pair
sets have no losses or gains against the previous Layout checkpoint.
Asset-backed `WantedWindow` interaction and ARM32 runtime remain
unverified. Next: consumer-backed `Layout+0x2a4..+0x2ac` save-menu
metrics. See [the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout late window placement metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes six previously missing
`+0x2c0/+0x2c4/+0x2cc..+0x2d8` words used by the save list, Hangar,
dialogue, wanted and mission windows. IDA branch values are checked
against Thumb stores at `0xd1122–0xd1128` and `0xd1786–0xd18b6`.
The production constructor smoke checks seven profiles, including
iPad + Retina. Native `gof2`, 206/206 ARM units, three Layout/
TouchButton smokes and 37 verifier tests (one skipped) pass. Focused
C1/C2 source-shape rises **38.0% → 39.9%**; strict fuzzy falls
**9.7% → 9.1%**. Neither alias is exact. Full report
`_work/layout_late_window_metrics_full_20261014.json` compares
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact pair
sets have no losses or gains against the previous Layout checkpoint.
Visual UI interactions with assets and ARM32 runtime remain unverified.
Next: audit consumer-backed `Layout+0xcc` in `WantedWindow`. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout Hangar and choice-window metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes the previously zero-filled
`+0x254..+0x278` group. `HangarWindow` consumes the first four slots for
row/blueprint placement; `ChoiceWindow` consumes the remaining six for
width, padding and height. IDA branch values are cross-checked against
Thumb stores at `0xd23cc–0xd23fe`. The production constructor smoke
checks all ten values for six profiles. Native `gof2`, 206/206 ARM units,
three Layout/TouchButton smokes and 37 verifier tests (one skipped) pass.
Focused C1/C2 source-shape rises **35.8% → 38.0%** while strict fuzzy
moves **9.8% → 9.7%**; neither alias is exact. Full report
`_work/layout_hangar_choice_metrics_full_20261014.json` compares
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Exact pair
sets have no losses or gains against the previous Layout checkpoint.
Hangar/Choice interaction, asset-backed rendering and ARM32 runtime
remain unverified. Next: audit remaining zero-filled middle Layout
fields for confirmed consumers. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout save-list metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes the previously zero-filled
`+0x108..+0x120` group and `+0x128`. The first four slots control
`MenuTouchWindow` save-list row and button placement; the remaining
slots are direct adjacent constructor writes with no confirmed
consumer in this package. IDA's ordinary/Retina vector selection and
N9/iPad HD literals are cross-checked with raw Thumb stores at
`0xd119a–0xd11ac`, `0xd1a9c–0xd1aac`, and `0xd1c48–0xd1c8c`.
The production constructor smoke checks all eight values for six
platform profiles. Native `gof2`, 206/206 ARM units, three Layout/
TouchButton smokes and 37 verifier tests (one skipped) pass. Focused
constructor C1/C2 source-shape/strict fuzzy rises **34.3%/9.5% →
35.8%/9.8%**; neither alias is exact. Full report
`_work/layout_save_metrics_full_20261014.json` covers
**4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have no losses or gains against the prior Layout checkpoint.
Save/load interaction, asset-backed rendering and ARM32 runtime remain
unverified. Next: consumer-backed `Layout+0x254..+0x278` Hangar
metrics. See [the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout window and touch metrics (2026-10-14)

Android `Layout::Layout` at `0xd1028` writes the previously zero-filled
`+0x5c..+0x6c` window/button dimensions and `+0x74..+0x88`
content, touch, slider and medal metrics. IDA platform branches and
raw Thumb stores confirm the six ordinary low, Retina, iPad, iPad
Large, N9 and iPad HD profiles. These fields feed `ListItemWindow`,
`WantedWindow`, `HangarWindow`, `TouchButton`, `TouchSlider`,
`StatusWindow` and Layout's own box/string drawing; the three
formerly padded 32-bit slots are now named without changing offsets.
The existing production-constructor smoke checks all eleven values
for each profile. Native `gof2`, 206/206 ARM units, three Layout/
TouchButton smokes and 37 verifier tests (one skipped) pass. Focused
constructor C1/C2 source-shape/strict fuzzy rises **30.9%/7.6% →
34.3%/9.5%**; neither alias is exact. Full report
`_work/layout_window_touch_metrics_full_20261014.json` covers
**4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Pairwise
exact sets have no losses or gains against the previous docking
checkpoint. Other constructor slots and asset-backed visual/ARM32
runtime behavior remain unverified. Next: a consumer-backed group
around `Layout+0x108..+0x128`. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## PlayerEgo docking glide and departure (2026-10-14)

Android ARM `0xa0050–0xa039c`, cross-checked with the IDA and Ghidra
`approachDockingPoint` branches, resolves the remaining state-2/3
distance and ease-update shims. State 2 selects the nearest docking
point from the held navigation point, converts it to world space,
stores it at `+0xec`, and compares integer-truncated distance with
the ship-specific radius. State 3 repeats the world-space calculation
with the nearest navigation point and a 200-unit exit threshold. On
non-arrival, both states advance the EaseInOutMatrix by frame time,
replace its translation with current position plus the target delta
scaled by frame time, reset the local body matrix, apply the eased
ship matrix, copy it to Player, and cache the new position at `+0x148`.
The ARM literals give follow rates `0.0006f` (approach) and
`0.0012f` (departure).

The extracted whole-method smoke covers both integer-distance
boundaries, follow rates, matrix writes and arrival/exit behavior.
Native `gof2`, 206/206 ARM units, 37 verifier tests (one skipped),
and four PlayerEgo smokes pass. Focused `approachDockingPoint`
source-shape/strict fuzzy improves **52.2%/27.1% → 69.9%/30.7%**;
it is not exact. Full report `_work/player_dock_glide_full_20261014.json`
covers **4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have no losses or gains against the ease-entry checkpoint.
Linked ARM gameplay remains unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking ease-matrix entry (2026-10-14)

Android ARM `0x9fdf4–0xa001a` confirms the transition into eased
docking: it releases the previous EaseInOutMatrix at `+0x358`,
constructs a 2000 ms matrix from current ship geometry to a basis
formed from rotated navigation/docking points and docking direction,
stores the new object, plays sound `0x8de` at the player's position
with zero velocity, and stops the engine sound. The extracted
whole-method smoke checks basis axes, lifetime, duration and audio.
Native `gof2`, 206/206 ARM units, 37 verifier tests (one skipped),
and four PlayerEgo smokes pass. Focused `approachDockingPoint`
source-shape/strict fuzzy improves **31.2%/16.1% →
52.2%/27.1%**; still non-exact. Full report
`_work/player_dock_ease_entry_full_20261014.json` covers
**4,323/4,524** original functions at **93.27%** average
source-shape and **82.17%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets have no losses or gains against the previous navigation
checkpoint. State-2/3 glide and easing updates remain unresolved;
linked ARM gameplay is unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking navigation and approach speed (2026-10-14)

Android ARM `0x9fd56–0x9fdc4` and IDA/Ghidra cross-checks establish
the state-0 world navigation point: target position plus its geometry
matrix rotation of the held SpacePoint position, stored as a 12-byte
vector at PlayerEgo `+0xec`. This spans three legacy header fields;
new ARM32 offset assertions guard them. Integer-truncated distance
chooses between state-2 entry and `moveToPosition` with steering and
speed `min(ship handling + 2.7, 4.0)`; only distances at most 19,999
set the near-target flag. The opaque `PE_adp_approach` call is gone.
Extracted whole-method smoke checks coordinates, speed branches and
distance boundaries. Native `gof2`, 206/206 ARM units, 37 verifier
tests (one skipped), and four PlayerEgo smokes pass. Focused
`approachDockingPoint` source-shape/strict fuzzy improves
**22.2%/15.3% → 31.2%/16.1%**. Full-corpus exact comparison is
recorded in `_work/player_dock_nav_full_20261014.json`:
**4,323/4,524** original functions covered, **93.27%** average
source-shape, **82.17%** strict fuzzy, **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Pairwise
exact sets have no losses or gains against the previous approach
checkpoint. At that checkpoint the ease-matrix constructor and
state-2/3 glide/easing were unresolved; the following pass recovers
the constructor. Linked ARM gameplay is unverified.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking approach and exit transitions (2026-10-14)

Android `approachDockingPoint` at `0x9fcb8` uses the approach threshold
at `+0x1d8`, clears the target byte `+0x8c` on entering state 2,
and enables look-at camera. State 2 sets the near-target flag and
returns true immediately on arrival in state 1, after clearing byte
`+0xb2`; state 3 clears the docking target, flags, radar pointers,
approach vector, SpacePoint and hacking game, requests standard
camera, and returns false. The earlier source used the wrong
threshold and return values and cleared `dockStation` instead of the
docking target. An extracted whole-method smoke now covers the
transition boundaries and cleanup. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused source-shape/strict fuzzy rises **20.5%/12.1% →
22.2%/15.3%** for this non-exact function. Full report
`_work/player_approach_docking_full_20261014.json`: **4,323/4,524**
original functions covered, **93.27%** average source-shape,
**82.17%** strict fuzzy, **2,479 linked-exact**, **1,065 byte-exact**,
**123 extras**, and zero skipped units. Pairwise exact sets have no
losses or gains against the docking-delivery checkpoint. At this
checkpoint the next priority was docking navigation and easing;
linked ARM gameplay remains unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo docking delivery and transfer ABI (2026-10-14)

Android `PlayerEgo::update` at `0x9dda8` onward, cross-checked with
IDA lines 84161–84311, handles deliveries after
`approachDockingPoint` returns true. Mission types 15/184 at docking
type 1 unload goods or passengers; types 184/168 at docking type 2
load passengers; campaign types 167/174 unload goods. The 1000/1500 ms
intervals come from ARM literals at `0x9ded4/0x9ded8`. Each path
clamps transferred amount to its initialized total, emits the
appropriate HUD event, and updates cargo, passenger count, mission
status, or delivered-ore count. Docking type 3 starts a hacking game
for the last matching docking-target slot if one is not active.

The corresponding `approachDockingPoint` function at `0x9fcb8` reads
target `+0x1bc` and state `+0x1c4`; state 1 returns true every update.
Its arrival path initializes transfer timer/total/delivered at
`+0x35c/+0x360/+0x364`, not the previously used auto-turret fields,
and transitions to state 1. Extracted production-fragment smoke checks
the state gate, initializers, transfer boundaries and side effects,
passenger-capacity exception, and both hacking modes. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and four PlayerEgo
smokes pass. Focused source-shape/strict fuzzy changes are
`update` **57.1%/27.6% → 66.1%/30.3%** and
`approachDockingPoint` **20.4%/10.2% → 20.5%/12.1%**. Neither
function is exact. Full report `_work/player_delivery_full_20261014.json`
covers **4,323/4,524** original functions at **93.27%** average
source-shape and **82.17%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Pairwise
comparison with `_work/player_subsystems_full_20261014.json` finds no
linked-/byte-exact losses or gains. Linked ARM gameplay and the remaining docking
movement/easing helpers are unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cloak recharge timer and hacking update (2026-10-14)

Android Thumb `0x9cfe2–0x9d19e` completes the previously unresolved
`PE_upd_subsystems` segment. A positive cloak recharge timer at `+0x20c`
decrements by frame time and emits HUD event 46 when it reaches zero
or below. The original checkpoint mistakenly used the emergency-system
field; the later cloak-lifecycle package corrects it. The hacking game
at `+0x1e8` updates with the stored frame
time `+0x134`; on a win, the HUD closes the game. If the current
station has an undiscovered hidden blueprint, the ARM reward table at
`0x1fba34` selects item IDs `226, 221, 223, 225, 227` by station
slot. The game object is destroyed; the first level enemy with byte
`+0x70` set has that byte cleared, docking type reset, and its `+0x50`
cargo array released. A radio message of type `slot + 21` is emitted
and the status flag at `+0x58` is marked. If no blueprint is available,
the original leaves the hacking object in place after closing the HUD;
the source preserves that distinction. Extracted-fragment smoke tests
the timer edge, no-win and no-reward paths, reward/cargo cleanup,
first-match selection, and null enemy list. Native `gof2`, 206/206
ARM units, 37 verifier tests (one skipped), and four PlayerEgo smokes
pass. Focused `update` source-shape/strict fuzzy rises
**55.0%/19.0% → 57.1%/27.6%**. The function is still non-exact;
full report `_work/player_subsystems_full_20261014.json` covers
**4,323/4,524** original functions at **93.26%** average
source-shape and **82.17%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Pairwise
comparison with the preceding boost checkpoint finds zero exact losses
and one new linked-exact pair (`Array<int>::~Array()`) from the cargo
cleanup path. Linked ARM gameplay is unverified. The next unresolved `update`
block is delivery/docking completion (`PE_upd_docksFinishDelivery`).
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo boost update (2026-10-14)

Android Thumb `0x9ce92–0x9ceba` advances the boost timer after the
frame-time store, including the negative-cooldown crossing test
`timer + 3 × dt >= 1`. The active-boost path at `0x9d1d6–0x9d310`
adds cargo-induced shake at rate 0.13, or 0.17 for equipment 195;
updates particle life for each engine nozzle from its X scale and
`getBoostPercentage`; and, only when `boostTimer > duration`, disables
the post-effect, resets speed to 2, and starts the negative cooldown.
When byte `+0x146` is clear, it also resets the boost speed/duration/
cooldown fields. The unresolved `PE_upd_boost` call is replaced by
this source-backed branch. The extracted production-fragment smoke
checks the cooldown boundary, rates, nozzle scaling, exact duration
boundary, and both reset modes. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape rises **52.3% → 55.0%**; strict fuzzy
stays **19.0%**. Full report `_work/player_boost_full_20261014.json`
covers **4,323/4,524** original functions at **93.26%** average
source-shape and **82.16%** strict fuzzy, with **2,478 linked-exact**,
**1,065 byte-exact**, **123 extras**, and zero skipped units. Exact
pair sets lose or gain none against the preceding checkpoint. The
whole `update` function is not exact, and linked ARM gameplay remains
unverified. At this checkpoint, the next priority was the earlier
`PE_upd_subsystems` behavior, then the delivery/docking branch. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo death/explosion update tail (2026-10-14)

Android Thumb `PlayerEgo::update` at `0x9edf0–0x9ef92` completes the
post-crosshair path: it stores the visible previous crosshair after
camera handling, stops shooting and two conditional sounds when a dead
player has an explosion object, applies death rotation outside the
docking-point exception, starts the explosion at the 3000 ms crossing,
updates primary/secondary explosions on their original gates, and
advances the timer. At the crossing it disables the current particle
system and emits a manual particle using the cached camera vectors.
Turret mode then composes the two local geometry matrices in ARM call
order. The unresolved `PE_upd_post` call has been replaced by this
source-backed tail; raw ARM shows only the function epilogue after it.
The extracted production-fragment smoke covers the gates, threshold,
calls, and matrix order. Native `gof2`, 206/206 ARM units, 37 verifier
tests (one skipped), and four PlayerEgo smokes pass. Focused `update`
source-shape rises **48.2% → 52.3%**; strict fuzzy falls
**24.8% → 19.0%**, with no whole-function exact match. Full report
`_work/player_update_tail_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**.
Exact-pair sets lose or gain none against the active-cursor checkpoint.
Earlier unresolved helpers such as `PE_upd_subsystems` and linked ARM
gameplay remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo active-cursor aim and steering (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9eb84–0x9eda6` confirms the
active-mouse branch following passive projection. It clamps mouse
deltas to 70% of half the screen dimensions, writes the clamped
deltas back, bounds the visible crosshair, and calls the corresponding
left/right/up/down control with the normalized displacement. It then
projects a separate 22,000-unit shoot point and smooths negative
depth against the prior shoot position at `+0xa0` using 0.2/0.8.
The constructor's Android initialization zeros both previous-position
vectors at `+0x94/+0xa0`; the C++ now does so explicitly. The
production-fragment smoke checks the active gates, steering signs,
clamping, projection, smoothing, and turret source order. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and four
PlayerEgo smokes pass. Focused `update` source-shape/strict fuzzy
changes **42.4%/10.9% → 48.2%/24.8%**; constructor aliases change
**40.3%/24.8% → 41.9%/25.2%**. None is exact. Full report
`_work/player_crosshair_active_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.17%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**. Exact
pair sets lose or gain none against the passive-crosshair checkpoint.
At this checkpoint the C++ saved the visible previous position before
the camera handoff; the following tail pass corrected that order.
Linked ARM gameplay remains unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo passive crosshair projection (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9ea32–0x9eda6`, checked against
IDA, Ghidra, and raw Thumb, projects an aim point 22,000 units along
the normalized ship direction. With an active turret, it first combines
the geometry matrices at `+0x2c` and `+0x28` in that order. The passive
path runs when the mouse cursor is inactive, `arg6` is true, `arg7` is
3, or `arg7` is neither 1 nor 3 while autopilot is active. A point
behind the camera is blended with the prior crosshair position at
`+0x94` using 0.2/0.8. The production-fragment smoke covers all
gates, projection, smoothing, and turret matrix order. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and four PlayerEgo
smokes pass. Focused `update` source-shape rises **37.9% → 42.4%**;
strict fuzzy falls **12.7% → 10.9%**, with no whole-function exact
match. Full report `_work/player_crosshair_passive_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**.
Pair-by-pair comparison with the cursor/camera checkpoint finds zero
exact losses or gains. Cursor-driven steering, shoot-position handling,
the original later store order for the prior position, and linked ARM
gameplay remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cursor/camera handoff (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9edaa–0x9edf0`, checked against
IDA and Ghidra, consumes the cursor-change byte at `+0x84` after the
camera/crosshair path. Unless the target-follow camera is in fast-forward,
it applies the ego handling at `+0x154` when the mouse cursor is active,
or resets camera handling otherwise, then clears the change byte. The
stored cursor state at `+0x378` is refreshed to Boolean 0/1 even when
fast-forward defers handling. The extracted production-fragment smoke
checks deferral, application, reset, and normalization. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and four PlayerEgo
smokes pass. Focused `update` source-shape rises **37.0% → 37.9%**;
strict fuzzy falls **21.5% → 12.7%**, so this remains a non-exact
large-function match. Full report
`_work/player_cursor_camera_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**; no exact
pairs are lost or gained against the hit-projection checkpoint.
The intervening camera/crosshair calculations and linked ARM gameplay
remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo hit-direction projection (2026-10-14)

Original Android ARM `PlayerEgo::update` at `0x9e94c–0x9ea32`,
cross-checked with IDA and Ghidra, normalizes `Player::getHitVector`,
scales it by 5000, adds the cached position at `+0x148`, and projects
the point through `PaintCanvas::GetScreenPosition`. The projected x
coordinate and depth classify left/right and top/bottom HUD damage
flags, including the original ±15,000/50,000 x thresholds. The
existing `Hud::draw` reads those flags for its 300 ms indicators.
An extracted production-fragment smoke covers damage gating, the
world point, screen thresholds, and behind-camera cases. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and four
PlayerEgo smokes pass. Focused `update` source-shape/strict fuzzy
changes **34.0%/10.5% → 37.0%/21.5%**; the function is not exact.
Full report `_work/player_hit_projection_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**.
Against the cursor/damage checkpoint, no linked- or byte-exact pair
is lost or gained. The following camera/crosshair branch and linked
ARM gameplay remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cursor/damage state and tail ABI (2026-10-14)

Android ARM `update` at `0x9e86a–0x9e94c`, cross-checked with IDA and
Ghidra, resets the per-frame direction/flash state, detects cursor-state
changes, and gates HUD/camera hit feedback on reduced combined HP while
neither computer control nor freeze is active. Shield, armor, and hull hit
flags select sounds 25, 23, and 24 in priority order. Adjacent Android
ARM accessors establish the docking `SpacePoint` at `+0x36c`, strafe
acceleration/factor at `+0x37c/+0x380`, and cloak materials at
`+0x388/+0x38c/+0x390`; the header asserts these offsets. An extracted
production-fragment smoke covers the cursor and damage gates, sound
priority, flag clearing, and unchanged HP. Native `gof2`, 206/206 ARM
units, 37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape rises **31.4% → 34.0%**, while strict
fuzzy changes **10.8% → 10.5%**; the whole function is not exact. Full
report `_work/player_hit_cursor_layout_full_20261014.json`:
**4,323/4,524** covered, **93.26%** source-shape, **82.16%** strict
fuzzy, **2,478 linked-exact**, **1,065 byte-exact**, **123 extras**,
with zero exact-pair losses or gains against the matrix/particle
checkpoint. The following hit-vector projection and linked ARM gameplay
remain unverified. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo matrix/particle prelude (2026-10-14)

Original ARM `update` at `0x9e7ac–0x9e82c`, cross-checked with Android
IDA/Ghidra, obtains the ship transform's Up vector for static
`PlayerEgo::vec_up` before the route transition. While boosting, it
interpolates particle sets 4 and 6 into set 4 using
`getBoostPercentage`; otherwise it restores set 4 from the initial
table while preserving the live color field at slot offset `+0x34`.
An extracted production-fragment smoke checks both branches, the
vector, blend arguments and preserved color. Native `gof2`, 206/206
ARM units, 37 verifier tests (one skipped), four PlayerEgo smokes,
and the real ParticleSettingsRef smoke pass. Focused `update`
source-shape/strict fuzzy rises **30.0%/10.2% → 31.4%/10.8%**;
the whole function remains non-exact. Full report
`_work/player_visual_prelude_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**,
with zero exact-pair losses or gains. The iOS dump did not provide
a reliably identified corresponding function. Linked ARM gameplay
and the remaining update tail are unverified. Next: post-route
cursor/damage state. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo route/HUD transition and route ABI (2026-10-14)

Android ARM `PlayerEgo::update` at `0x9e82c–0x9e866`, checked against
IDA and Ghidra, reads `route` at `+0xfc`, updates it with the cached
position at `+0x148`, and emits HUD event 23 on a changed waypoint or
24 when a non-looping route returns to zero. The source and extracted
host smoke now cover no route, no transition, advancement, completion,
and loop wrap. The same ARM/IDA evidence fixes `route` at `+0xfc` and
the secondary-weapon index at `+0x10c`; neighboring legacy field names
are not yet independently confirmed. Native `gof2`, 206/206 ARM units,
37 verifier tests (one skipped), and four PlayerEgo smokes pass.
Focused `update` source-shape rises **28.7% → 30.0%**, while strict
fuzzy changes **10.6% → 10.2%**; the whole function is not exact. Full
report `_work/player_route_event_full_20261014.json`: **4,323/4,524**
covered, **93.26%** source-shape, **82.16%** strict fuzzy,
**2,478 linked-exact**, **1,065 byte-exact**, **123 extras**. Compared
with the player/sound checkpoint, exact-pair losses are zero and gains
are four linked / three byte. The matrix-up/particle-settings prelude
between sound and route, remaining post logic, and linked ARM gameplay
are still unverified. Next: recover that prelude in original order.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo player/sound handoff (2026-10-14)

Android IDA and independent Ghidra show that `update` calls
`Player::update` after passive recovery, passing `doSound` only
when alive and the event word at `+0x1c` is valid. Its separate
3D sound-position update requires a nonzero byte at `+0x38`,
options byte `+0x0f`, and valid event ID at `+0xb4`. The source
now uses that exact byte test rather than treating the overlapping
pointer as a Boolean. The constructor also restores the missing
producer of `+0x1c`: special ship IDs 42/43/40 or four base-handling
bands select and register its sound resource before agility scaling.
The production-fragment smoke tests these gates and selections.
Native `gof2`, 206/206 ARM units, 37 verifier tests (one
skipped) and four PlayerEgo smokes pass. `update` source-shape
rises **27.1% → 28.7%**, strict fuzzy **10.5% → 10.6%**;
constructor source-shape rises **33.4% → 40.3%**, strict fuzzy
**19.8% → 24.8%**. Neither full function is exact. Full corpus
remains **4,323/4,524** covered, **93.25%** source-shape,
**82.12%** strict fuzzy, **2,474 linked-exact**,
**1,062 byte-exact**, **123 extras**, with zero exact-pair
losses. Linked ARM gameplay and the rest of `PE_upd_post`
remain unverified. Next: route-change/HUD notification after
the matrix and particle update. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo passive shield and repair timers (2026-10-14)

Android ARM and independent Ghidra confirm three 64-bit timers
at `+0x118/+0x120/+0x128`, the shield recovery amount at
`+0x218`, and the constructor's amount formula. The original
literal pool gives strict repair thresholds of 420/700 ticks for
nonzero repair type and 600/1000 for type zero. The update now
advances timers only while alive, restores shield at 101 ticks,
repairs hull when damaged and armor only after hull reaches full
HP. A production-fragment smoke tests all threshold boundaries,
death gating and the constructor amount expression. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped) and four
PlayerEgo smokes pass. Focused source-shape rises **22.0% →
27.1%** for `update` and **30.4% → 33.4%** for its constructor;
neither is exact. Full corpus: **4,323/4,524** covered,
**93.24%** source-shape, **82.12%** strict fuzzy,
**2,474 linked-exact**, **1,062 byte-exact**, **123 extras**,
with zero exact-pair losses. Linked ARM gameplay and the rest
of `update` remain unverified. Next: the bounded player-update/
sound handoff within `PE_upd_post`. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo shield recharge branch (2026-10-14)

Confirmed Android IDA/Ghidra control flow now starts shield recharge
only with equipment sort 43, zero shield HP and enough cargo item
202, then spends its attribute-59 amount and emits the original HUD
and sound events. An already-active recharge heals at least 1 HP per
frame even without the equipment flag; full shield ends the effect.
The production-fragment smoke covers these boundaries. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and
four PlayerEgo host smokes pass. `update` source-shape rises
**14.3% → 22.0%** and strict fuzzy **4.1% → 8.3%**, without an
exact match. Full corpus remains **4,323/4,524** covered,
**93.24%** source-shape, **82.12%** strict fuzzy,
**2,474 linked-exact**, **1,062 byte-exact**, **123 extras**;
there are zero exact-pair losses. The rest of `update` and linked
ARM gameplay remain unverified. Next: its adjacent shield/repair
timer branch. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo shield/mining flag split (2026-10-14)

Android ARM shows that shield-regeneration state at `+0x399`,
equipment presence at `+0x39a`, and mining-loss state at `+0x39b`
are separate. The C++ had incorrectly placed mining loss at
`+0x399`. The header and constructor now match these bytes;
the shield branch in `update` remains to be reconstructed. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and
three PlayerEgo smokes pass. Full corpus: **4,323/4,524**
covered, **93.24%** source-shape, **82.12%** strict fuzzy,
**2,474 linked-exact**, **1,062 byte-exact**, **123 extras**.
No exact-pair losses; `lostMiningGame()` adds one linked/byte
match. Constructor is still only **30.4%** source-shape, so
the class is not behavior-complete. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo cargo-state refresh (2026-10-14)

`update` now follows Android ARM: when ship free space decreases,
it refreshes saved free space and the volatile-goods flag. A
production-fragment smoke covers decreasing, increasing and equal
free-space cases. Native `gof2`, 206/206 ARM units, 37 verifier
tests (one skipped), and three PlayerEgo smokes pass. Full corpus
remains **4,323/4,524** covered, **93.24%** source-shape,
**82.11%** strict fuzzy, **2,473 linked-exact**,
**1,061 byte-exact** and **123 extras**, with zero exact-pair
losses. `update` itself is only **14.3%** source-shape and not
behavior-complete. Next: the adjacent equipment/shield branch.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo turret position repair (2026-10-14)

Raw Android ARM passes `PlayerEgo+0x224` to both turret-geometry
`setPosition` calls. The C++ now uses `turretOffsetVec` instead of
reinterpreting HUD image handles as coordinates. Native `gof2`,
206/206 ARM units and 37 verifier tests (one skipped) pass.
Full corpus remains **4,323/4,524** covered, **93.24%**
source-shape, **82.11%** strict fuzzy, **2,473 linked-exact**,
**1,061 byte-exact** and **123 extras**, with zero exact-pair
losses. Turret construction as a whole is still non-exact and
linked gameplay is untested. Next: restore cargo-state refresh
in `update`. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo HUD/constructor group (2026-10-14)

Confirmed Android ARM fields for hard-core mode, agility, prior
free cargo space, crosshair/throttle images and the blink timer
no longer overlap. The constructor now scales ship handling by
agility, initializes the cargo flag, and creates five HUD images;
`draw` reads the matching handles. Native `gof2`, 206/206 ARM
units, 37 verifier tests (one skipped), and three PlayerEgo smokes
pass. Full corpus: **4,323/4,524** covered, **93.24%**
source-shape, **82.11%** strict fuzzy, **2,473 linked-exact**,
**1,061 byte-exact**, **123 extras** and zero lost exact pairs.
Constructor source-shape is **29.0%**, not a completion signal.
Next: restore cargo-state refresh in `update` and resolve the
unverified position argument in `checkForTurret`. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo late movement/visibility ABI repair (2026-10-14)

Android ARM confirms late movement, roll-Matrix, rotation,
explosion-timer, visibility, dock-total and material/volatile
offsets. The header now matches these addresses at the original
`0x3a0` class size, and `setVisible` again calls
`setExhaustVisible` to update particle emission. Native `gof2`,
all 206 ARM units, 37 verifier tests (one skipped) and three
PlayerEgo smokes pass. Full corpus: **4,323/4,524** covered,
**93.23%** source-shape, **82.09%** strict fuzzy, **2,472
linked-exact**, **1,060 byte-exact** and **123 extras**; exact-pair
losses are zero. Constructor shape remains only **19.6%** and
linked ARM gameplay is untested. Next: reconstruct constructor
initialization and validate the distinct visibility flags.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo HUD/turret ABI repair (2026-10-14)

Android ARM reads the HUD at `PlayerEgo+0x220`, turret-active byte
at `+0x1a0`, and cloak pointer at `+0x1b0`. The header and the
three affected uses now agree with those addresses; duplicated
late fields were removed. Native `gof2`, all 206 ARM units, 37
verifier tests (one skipped) and three PlayerEgo smokes pass. Full
corpus: **4,323/4,524** covered, **93.20%** source-shape,
**81.98%** strict fuzzy, **2,461 linked-exact**, **1,051 byte-exact**
and **123 extras**, with zero lost exact pairs. `getHUD` is now
byte-exact. The later movement/rotation fields still diverge from
ARM despite matching total class size; linked ARM gameplay is
untested. Next: map the roll Matrix and rotation/timer group.
See [the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo camera-handling semantic repair (2026-10-14)

`Ship::getHandling()` returns a float, but `PlayerEgo` stored it in
an integer at the now-correct `+0x154` offset, truncating the value.
The member and getter now use float. Android ARM
`setTargetFollowCamera` copies that value's bits from `+0x154` to
`TargetFollowCamera::setShipHandling`; the C++ had incorrectly
called `resetShipHandling`. The corrected setter is 100% source-
shape and linked-exact (from 82% source-shape), while
`getHandling` stays byte-exact. Native `gof2`, all 206 ARM units,
37 verifier tests (one skipped) and three PlayerEgo smokes pass.
The full corpus remains **4,323/4,524** covered, at **93.20%**
source-shape and **81.97%** strict fuzzy, with **2,459** linked-
exact, **1,050** byte-exact and **123** extras. There are no lost
exact pairs. `PlayerEgo` size still matches `0x3a0`, but HUD and
later members remain offset and linked ARM gameplay is untested.
Next: reconstruct the HUD/terminal-field group before asserting
full ABI parity. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo mining/docking ARM32 layout repair (2026-10-14)

Android ARM places `dockScaling`, its settle timer, station,
`MiningGame` and `HackingGame` at `+0x1d8..+0x1e8`, after an
unidentified four-byte slot at `+0x1d4`. The header now matches
these confirmed offsets. Native `gof2`, all 206 ARM units, 37
verifier tests (one skipped), and three PlayerEgo smokes pass.
The full corpus covers **4,323/4,524** original functions at
**93.20%** source-shape and **81.95%** strict fuzzy, with
**2,458** linked-exact, **1,050** byte-exact and **123** extras.
Exact-pair sets gain 19 linked and 12 byte matches with zero
losses. `dockToAsteroid` source-shape rises from 98.2% to 99.4%.
Although `sizeof(PlayerEgo)` is now `0x3a0`, matching the original
allocation, this is not proof of full ABI parity: HUD is still at
`+0x218` versus ARM `+0x220`, and later fields also need mapping.
The camera setter and linked gameplay remain unverified. Next:
reconstruct the later HUD/terminal-field offsets, then fix the
camera setter's handling route. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo frame/docking ARM32 layout repair (2026-10-14)

Raw Android ARM `update` stores the frame time at `+0x134` and
copies the player's actual position to the Vector at `+0x148`.
The C++ previously discarded the returned position and copied an
uninitialized array, including its third component into a separate
misidentified field. It now assigns the whole Vector and frame time.
Confirmed surrounding offsets through `handling +0x154`, asteroid
target `+0x1bc` and docking state `+0x1c0` now match ARM; the
constructor also initializes `lastHP` as in the binary. Native
`gof2`, all 206 ARM units, 37 verifier tests (one skipped) and three
PlayerEgo smokes pass. Full corpus: **4,323/4,524** covered,
**93.17%** source-shape, **81.85%** strict fuzzy, **2,439** linked-
exact, **1,038** byte-exact and **123** extras. The exact-pair sets
gain 25 linked and 19 byte matches with zero losses. The ARM32 class
is still `0x398` versus original allocation `0x3a0`; mining-game
and HUD fields remain offset, and the constructor/update are still
non-exact. Next: map those later gaps and the camera setter. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo control-field ARM32 layout repair (2026-10-14)

The next verified group now places camera-switch flags at `+0xb0`
and `+0xb1`, speed/thrust at `+0xb8`/`+0xbc`, free-look mode at
`+0xc4`, and boost values at `+0xc8..+0xd0`, matching Android ARM
accessors and constructor writes. The two camera flags share the
`+0xb0` word; the constructor now initializes them and restores
the original speed/thrust values and boost assignments. Native
`gof2`, all 206 ARM units, and 37 verifier tests pass (one skipped).
Ten more `PlayerEgo` functions become both linked- and byte-exact,
with no lost exact pairs. Full corpus: **4,323/4,524** covered,
**93.13%** source-shape, **81.64%** strict fuzzy, **2,414** linked-
exact, **1,019** byte-exact, **123** extras. The class still has
size `0x378` versus the original allocation `0x3a0`; `handling`
remains at `+0x134` versus ARM `+0x154`, and the constructor and
camera setter remain non-exact. Next: establish the intervening
offset group before moving handling and docking fields. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo early ARM32 layout repair (2026-10-14)

The Android binary allocates `PlayerEgo` with **0x3a0** bytes. Its
constructor constructs a Matrix at `+0x40`; ARM maneuver writes use
`+0x7c`/`+0x80`, and the camera accessor uses `+0x88`. The header
and constructor now represent this early offset group, increasing its ARM32 size
from **0x320** to **0x360** and restoring byte-exact camera getter
code. Constructor source-shape rises from 18.5% to 18.9% and remains
non-exact. Native `gof2` and all 206 ARM units build. The full corpus is
4,323/4,524 covered, 93.11% source-shape, 81.54% strict fuzzy,
2,404 linked-exact and 1,009 byte-exact; no prior exact pair was
lost. This is not a full ABI repair: asteroid target remains at
`+0x184` versus original `+0x1bc`, docking state at `+0x188`
versus `+0x1c0`, and HUD at `+0x1dc` versus `+0x220`.
`setTargetFollowCamera` also still uses the wrong handling route.
Isolated host smokes do not validate the real ARM32 object layout.
Next: reconstruct subsequent offset groups and the camera setter
against ARM, then revisit stage-zero asteroid approach. See
[the ABI audit](PLAYER_EGO_ABI_LAYOUT_GAP_ARM_2026-10-14.md).

## PlayerEgo asteroid mining phase (2026-10-14)

`PlayerEgo::approachAsteroid` now checks the asteroid's dying/dead
state rather than the ego's, passes its real HUD to MiningGame and
loss events, uses the confirmed `Globals::hints`, sound and status
slots, pauses the correct sound event and clears `asteroid->player`'s
radius instead of an unrelated asteroid field. The production-method
host smoke covers the target guard, mining-game creation, target death
during update and a lost game. Native `gof2` static-library build,
all 206 ARM units, three PlayerEgo smokes and 37 verifier tests pass
(one skipped). Focused source-shape rises 23.8% to 39.6%, strict
fuzzy 13.5% to 21.6%; no exact match. Full corpus remains
4,323/4,524 covered, 93.10% average source-shape, 81.52% strict
fuzzy, 2,403 linked-exact, 1,008 byte-exact and 123 extras, with
unchanged exact sets. The approach step still calls undefined
`PE_aa_approachStep`, so this is not a linked gameplay path; full
ARM32 runtime and null-target safety remain unverified. The ABI
audit above must precede the stage-zero approach recovery. See
[the mining-phase audit](PLAYER_EGO_APPROACH_ASTEROID_ARM_2026-10-14.md).

## PlayerEgo update docking teardown (2026-10-14)

The Android ARM `PlayerEgo::update` checks the docking target's
`isDying`/`isDead` state, passes its `Radar*` to asteroid teardown,
and returns directly after either docking teardown. The C++ had
checked the player instead, passed a null radar to asteroid teardown,
and ran an extra post-update path. Both asteroid and docking-point
branches now follow the original sequence. A host smoke compiles the
production docking-state fragment and covers live, dying and dead
targets. Native `gof2`, all 206 ARM units, the two PlayerEgo smokes
and 37 verifier tests pass (one skipped). Focused `update` source-
shape rises 12.2% to 12.7%; strict fuzzy falls 4.8% to 4.1% and
the function remains non-exact. Full corpus stays 4,323/4,524
covered, 93.10% source-shape, 81.51% strict fuzzy, 2,403 linked-
exact, 1,008 byte-exact and 123 extras; exact sets are unchanged.
The full `update` flow, missing-target teardown and ARM32 runtime
remain unverified. The mining-phase follow-up above checks
`approachAsteroid` target and HUD routes. See
[the update audit](PLAYER_EGO_UPDATE_DOCKING_ARM_2026-10-14.md).

## PlayerEgo asteroid-docking target (2026-10-14)

`PlayerEgo::dockToAsteroid` now starts docking only for a non-null
`KIPlayer*` target, stores that target and obtains its asteroid scale.
Previously it tested/stored the separate `Radar*` as the target.
The Android ARM body at `0x9f4a0` confirms the target route; MGame
callers pass the locked asteroid and radar separately. A production-
method host smoke checks no-target, entry and teardown paths.
Focused ARM source-shape rises 90.8% to 94.7% and strict fuzzy
53.7% to 72.1%; the function is not exact. Native `gof2`, all 206
ARM units, the docking and mining handoff smokes and 37 verifier
tests pass (one skipped). Full corpus stays 4,323/4,524 covered,
93.10% average source-shape, 81.51% strict fuzzy, 2,403 linked-exact,
1,008 byte-exact and 123 extra symbols; the compared and exact sets
are unchanged. ARM32 gameplay remains unverified. A separate
`PlayerEgo::update` path called teardown with a null radar; the
follow-up audit above restores the original argument and target
checks. Missing-target runtime safety remains unverified.
See [the docking audit](PLAYER_EGO_DOCK_ASTEROID_ARM_2026-10-14.md).

## Ship cargo-add handoff (2026-10-14)

`Ship::addCargo(Item*)` now forwards to the array overload, as in
Android ARM, so adding ore after a core no longer replaces the core.
The production-method cargo smoke covers retention, duplicate-item
merging and free-space accounting. Native `gof2`, all 206 ARM units,
the combined mining handoff smoke and 37 verifier tests pass (one
skipped). Focused source-shape and strict fuzzy stay at 100%, with
no exact match: the old wrong call target passed shape comparison.
Full corpus remains 4,323/4,524 covered at 93.10% average
source-shape and 81.51% strict fuzzy; all 2,403 linked-exact and
1,008 byte-exact pairs and 123 extra symbols are unchanged.
Full native/ARM32 gameplay, HUD/docking/audio dependencies and cargo
lifetime are not yet verified. Next: test the remaining real handoff
services. See [the cargo audit](SHIP_ADD_CARGO_HANDOFF_ARM_2026-10-14.md).

## Status hardcore difficulty route (2026-10-14)

The original ELF places `Globals::options` at `0x2181e8` and the
hardcore float at `0x218214`, exactly options offset `0x2c`.
`Status::hardCoreMode` now reads that setting instead of dereferencing
an unassigned holder. Its focused ARM source-shape rises 83.6% to
93.7%, strict fuzzy 66.7% to 88.0%; it is not exact. A combined
production-method host fixture covers normal/hardcore mining rewards,
and native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Full corpus remains 4,323/4,524 covered; average
source-shape rises 93.09% to 93.10% and strict fuzzy stays 81.51%.
All 2,403 linked-exact and 1,008 byte-exact pairs and 123 extra
symbols are unchanged. A complete native/ARM32 mining session,
persisted options load and marquee ownership are still unverified.
Next: test the handoff through real cargo/HUD/docking services. See
[the difficulty audit](STATUS_HARDCORE_OPTIONS_ARM_2026-10-14.md).

## PlayerEgo mining-result handoff (2026-10-14)

`PlayerEgo::stopMining` now follows Android ARM and independent
Ghidra evidence for ore/core cargo, hardcore halving, capacity,
discovery flags, HUD events, asteroid/status reset, undocking and
audio. Previously it only deleted MiningGame. Focused source-shape
rises 9.3% to 91.0% and strict fuzzy 4.4% to 39.7%; it is not exact.
Six production-method host scenarios, native `gof2`, all 206 ARM
units, three MiningGame smokes and 37 verifier tests pass (one
skipped). Full corpus remains 4,323/4,524 covered (201 missing);
average source-shape rises 93.08% to 93.09% and strict fuzzy 81.50%
to 81.51%. All 2,403 linked-exact and 1,008 byte-exact pairs and
123 extra symbols are unchanged. At that checkpoint a full runtime
path was unverified because `Status::hardCoreMode` used an unassigned
holder; its route is recovered above. Real cargo/HUD/docking effects,
ARM32 runtime and marquee ownership remain open. See
[the handoff audit](PLAYER_EGO_STOP_MINING_ARM_2026-10-14.md).

## MiningGame outcomes and teardown audit (2026-10-14)

The ARM loss, medal and destructor paths are now checked by
production-method host fixtures. The 2500/2501 ms loss boundary,
ore/status reset, 30% and 100% medal HUD events, single-byte status
flag and drill-sprite destruction pass. The ARM destructor frees only
the drill sprite; it does not free constructor-created marquees, whose
ownership remains unresolved. No production source changed. Native
`gof2`, all 206 ARM units, three mining smokes and 37 verifier tests
pass (one skipped). Full corpus remains 4,323/4,524 covered,
93.08% average source-shape, 81.50% strict fuzzy, 2,403 linked-exact,
1,008 byte-exact and 123 extra symbols, with exact sets unchanged.
`update` is still 95.1% source-shape / 82.1% strict fuzzy, not exact.
Next: recover `PlayerEgo::stopMining` from ARM: it currently only
deletes `MiningGame`, omitting cargo, status, ship and audio effects.
Then test the full handoff. ARM32 runtime and marquee lifetime remain
unverified. See
[the outcomes audit](MINING_OUTCOMES_TEARDOWN_ARM_2026-10-14.md).

## MiningGame render2D service and call recovery (2026-10-14)

`MiningGame::render2D` now uses the Android-backed `Globals`
services and `LAYER_DIAMETERS` instead of unassigned holders. The
canvas calls use their original separate image alignment/placement
flags, the ore suffix is `"t"`, the font is the numeric
`Globals::font`, and the campaign prompt uses the original 1,000-tick
pulse divisor. Focused source-shape rises 25.6% to 97.4%, strict
fuzzy 14.5% to 78.9%; it is not exact. The production-method host
smoke, previous MiningGame smokes, native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). This validates call
parameters, not visual output with game assets or ARM32 runtime.
Full corpus coverage remains 4,323/4,524; average source-shape rises
93.06% to 93.08% and strict fuzzy 81.48% to 81.50%. All 2,403
linked-exact and 1,008 byte-exact pairs and 123 extra symbols are
unchanged. The source now has no local null service holders, but a
complete native mining sequence is untested.
Next: test a complete mining sequence and the status-loss/medal
branches, then audit teardown ownership. See
[the rendering audit](MINING_RENDER_ARM_2026-10-14.md).

## MiningGame constructor service and image bindings (2026-10-14)

The Android-backed constructor now reads the actual `Globals`
screen, layout, status and canvas services and writes image handles
directly into `uint32_t` fields. The drill image output starts at
`-1`, matching the original. Focused C1/C2 source-shape rises 1.3%
to 99.1%, strict fuzzy 0.6% to 89.1%; neither alias is exact.
The production-constructor host smoke covers equipment, coordinates,
images, marquees and both core-image choices; native `gof2`, all 206
ARM units and 37 verifier tests pass (one skipped). A no-equipment
path still leaves `controlDivisor` unset in the Android decompilation;
its reachability is unknown. ARM32 runtime and full mining sequence
are untested. Full corpus coverage remains 4,323/4,524; average
source-shape rises 93.02% to 93.06% and strict fuzzy 81.45% to
81.48%. All 2,403 linked-exact and 1,008 byte-exact pairs and 123
extra symbols are unchanged. At that checkpoint, the next step was
`render2D` (subsequently covered above). See
[the constructor audit](MINING_CONSTRUCTOR_ARM_2026-10-14.md).

## MiningGame update and layer speeds (2026-10-14)

`MiningGame::update` now uses the Android-exported seven-entry
`LAYER_SPEEDS` table and the original `Globals` service routes rather
than unassigned holders. It also restores integer ore-boundary text
alpha, the animation threshold, and the single-byte status medal flag.
Focused source-shape rises 14.1% to 95.1%, strict fuzzy 14.1% to
82.1%; it is not exact. A production-method host smoke, native `gof2`,
all 206 ARM units and 37 verifier tests pass (one skipped). Full
corpus coverage remains 4,323/4,524; average source-shape rises
93.00% to 93.02% and strict fuzzy 81.43% to 81.45%. All 2,403
linked-exact and 1,008 byte-exact pairs and 123 extra symbols are
unchanged. Medal/loss branches and ARM32 runtime are untested; the
constructor and `render2D` retained unassigned service holders at that
checkpoint. Constructor recovery is described above; rendering and a
complete native mining sequence remain. See
[the update audit](MINING_UPDATE_ARM_2026-10-14.md).

## MiningGame layer membership (2026-10-14)

`MiningGame::isInCurrentLayer()` now uses the recovered Android
`LAYER_DIAMETERS` table, `Globals::layout` and `Globals::sqrt` rather
than unassigned holders. Focused source-shape rises 0.5% to 99.1%
and strict fuzzy 0.0% to 81.8%, but the function is not exact. The
production-method host smoke checks layer boundaries; native `gof2`,
all 206 ARM units and 37 verifier tests pass (one skipped). Full
corpus coverage is 4,323/4,524; average source-shape rises 92.98%
to 93.00% and strict fuzzy 81.41% to 81.43%. All 2,403 linked-exact,
1,008 byte-exact pairs and 123 extra symbols are unchanged. ARM32
runtime behavior is untested. Next: audit `MiningGame::update`'s
unassigned service dependencies and the original `LAYER_SPEEDS`
table. See [the layer audit](MINING_LAYER_MEMBERSHIP_ARM_2026-10-14.md).

## Layout constructor MiningGame parameters (2026-10-14)

The Android-backed `Layout+0xd0..+0x104` values consumed by
`MiningGame` now supply center/progress-bar placement, marquee speed,
nonzero drift divisor, ring scale and thresholds, and ore-label
offsets. `+0xdc` is typed as `float`, matching its only consumer.
Focused `Layout` C1/C2 source-shape rises 29.3% to 30.9%, while
strict fuzzy stays 7.6%; neither alias is exact. The `MiningGame`
constructor's focused 1.3% source-shape and 0.6% strict fuzzy are
unchanged. The six-profile host smoke, native `gof2`, all 206 ARM
units and 37 verifier tests pass (one skipped). Full corpus remains
4,323/4,524 covered at 92.98% average source-shape and 81.41%
strict fuzzy; all 2,403 linked-exact and 1,008 byte-exact pairs and
123 extra symbols are unchanged, with zero exact-set losses. Other constructor
fields and `MiningGame`'s unassigned service holders remain; native
ARM32 mining behavior is untested. Next: source-backed binding of its
layout/service holders, beginning with the direct `Globals::layout`
routes. See [the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor HUD initialization fields (2026-10-14)

The eight `Layout+0x12c..+0x148` values copied by `Hud::init` now
follow the Android iPad HD, N9, ordinary, Retina, iPad and iPad Large
branches instead of the zero-fill placeholder. This restores the
HUD's touch extents, analog-stick radius, radar inset, event margins
and boost-text offset at construction. Focused C1/C2 source-shape
rises 27.3% to 29.3%, strict fuzzy 5.7% to 7.6%; neither alias is
exact. The six-profile production constructor smoke, native `gof2`,
all 206 ARM units and 37 verifier tests pass (one skipped). Full corpus
remains 4,323/4,524 covered at 92.98% average source-shape and
81.41% strict fuzzy; all 2,403 linked-exact and 1,008 byte-exact
pairs and the 123 extra-symbol set are unchanged. Original ARM32
runtime and visual/interaction behavior remain untested. Next:
recover the zero-filled MiningGame-dependent `Layout+0xd0..+0x104`
block, especially the nonzero drift divisor and ring scale. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor consumer-backed middle fields (2026-10-14)

Android-backed `Layout+0x70` row height and `+0x8c..+0x9c`
StarMap/Radio metrics now populate the six platform profiles instead
of remaining zero-filled. This restores the float bit pattern copied
into `StarMap::dragScale` and the dimensions consumed by `Radio`.
Focused C1/C2 source-shape rises from 23.9% to 27.3%; strict fuzzy
shifts from 5.8% to 5.7%, with neither alias exact. The production
constructor host smoke, native `gof2`, all 206 ARM units, and 37
verifier tests pass (one skipped). Full corpus remains 4,323/4,524
covered at 92.98% average source-shape and 81.41% strict fuzzy;
2,403 linked-exact and 1,008 byte-exact pairs and the 123 extra
symbols are unchanged, with no exact-set losses. Many intervening
fields remain unrecovered; no ARM32 runtime or visual-layout
validation was done.
Next priority: the zero-filled `Layout+0x12c..+0x148` metrics copied
by `Hud::init`, with direct ARM branch confirmation. See
[the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout constructor platform metrics (2026-10-14)

`Layout::Layout()` no longer dereferences eight unassigned configuration
pointers. Its first 22 metrics and several downstream reward, choice,
scrollbar and help-window values now follow Android-backed platform
branches. Focused C1/C2 source-shape rises from 8.7% to 23.9% (strict
fuzzy 2.9% to 5.8%); this large constructor is **not** fully recovered.
The production-method host smoke covers six platform profiles. Native
`gof2`, all 206 ARM translation units, and 37 verifier tests pass (one
skipped). Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201
missing, 92.98% average source-shape, 81.41% strict fuzzy, 2,403
linked-exact and 1,008 byte-exact. Complete exact and extra-symbol
sets are unchanged. The broad early field block, ambiguous platform
values and ARM32 runtime/visual behavior remain unverified. Next:
recover constructor fields `+0x5c..+0x2d8` in bounded,
consumer-backed groups. See [the constructor audit](LAYOUT_CONSTRUCTOR_METRICS_ARM_2026-10-14.md).

## Layout mission reward (2026-10-14)

`Layout::showMissionRewardMessage` now plays the Android-backed
sound through `Globals::sound` rather than an unassigned holder.
`drawMissionRewardMessage` restores the original fade timing,
color scaling, localized title selection, amount prefix, canvas,
font, icon coordinates/alignment and six-argument box calls.
The production-method host smoke checks both title variants,
formatted credits, both transition branches and six fade-boundary
values. Native `gof2`, all 206 ARM units and 37 verifier tests pass
(one skipped). Focused source-shape rises 89.3% to 100.0% for show
and 90.6% to 97.3% for draw; strict fuzzy rises 51.4% to 55.0%
and 38.7% to 48.5%. Neither is exact. Full corpus: 4,718
comparisons, 4,323/4,524 covered, 201 missing, 92.97% average
source-shape, 81.41% strict fuzzy, 2,403 linked-exact and 1,008
byte-exact; complete exact and extra-symbol sets are unchanged.
ARM32 runtime and visual rendering remain untested. Next priority:
audit `Layout::Layout()`
for unassigned platform configuration holders and field defaults.
See [the reward audit](LAYOUT_MISSION_REWARD_ARM_2026-10-14.md).

## Layout help-window dispatch (2026-10-14)

`Layout::initHelpWindow(String)` now plays the original sound through
`Globals::sound`, uses title text `0x187` from `Globals::gameText`,
and calls `ChoiceWindow::set(title, message, false)` instead of a
different two-argument overload. `drawHelpWindow()` now dispatches
directly, matching the guarded Android call path. The
production-method host smoke covers first/repeated initialization,
sound, title, text, state flags, window reuse and drawing. Native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
Focused `initHelpWindow` source-shape rises 93.9% to 100.0% and
strict fuzzy 74.2% to 97.9%, but remains non-exact;
`drawHelpWindow` rises 39.4% to 100.0% and 28.6% to 100.0%, adding
one linked-exact result (not byte-exact). Full corpus: 4,718
comparisons, 4,323/4,524 covered, 201 missing, 92.97% average
source-shape, 81.41% strict fuzzy, 2,403 linked-exact and 1,008
byte-exact. Complete exact and extra-symbol sets show this one
linked-exact gain with no losses. Original ARM32 runtime and visual
rendering remain untested. Next priority: audit the mission-reward
display path and its unassigned sound holder. See
[the help-window audit](LAYOUT_HELP_WINDOW_ARM_2026-10-14.md).

## Layout color-tag string (2026-10-14)

`Layout::tagString(String)` now constructs the Android-backed
`<c:-8388353>` prefix and `<c:>` suffix, replacing empty literals
and a dereference of an unassigned null String pointer. Its header
now declares an instance method, matching the original ARM ABI.
The production-method host smoke confirms the resulting tag.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Focused source-shape rises 92.1% to 100.0%, strict fuzzy
49.8% to 100.0%; it remains non-exact. Full corpus: 4,718
comparisons, 4,323/4,524 covered, 201 missing, 92.95% average
source-shape, 81.39% strict fuzzy, 2,402 linked-exact and 1,008
byte-exact. Complete exact and extra-symbol sets are unchanged.
ARM32 runtime and original-asset rendering remain untested. Next
priority: audit `Layout::reload()` for real resource-loading gaps.
See [the tag audit](LAYOUT_TAG_STRING_ARM_2026-10-14.md).

## Layout number formatting (2026-10-13)

`Layout::formatNumber(int)` now uses Android-backed comma/dot and
minus literals, takes each three-digit substring from the current
end, and joins groups in the original order. Direct String
construction removes extra temporary copies. The production-method
host smoke covers grouping boundaries, negative values, five
language IDs and `INT_MIN`. Native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). Focused source-shape rises 83.6%
to 98.9%, strict fuzzy 36.6% to 80.1%; it remains non-exact.
Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.95% average source-shape, 81.38% strict fuzzy, 2,402 linked-exact
and 1,008 byte-exact. Complete exact and extra-symbol sets are
unchanged.
For `INT_MIN`, the host path yields a double minus after ARM-style
32-bit negation; original ARM32 runtime output is not directly
tested. Real localized text and visual output remain untested. Next
priority: audit `Layout::tagString(String)` (92.1% source-shape),
which still dereferences an unassigned holder. See
[the formatting audit](LAYOUT_FORMAT_NUMBER_ARM_2026-10-13.md).

## Layout drawBox default-flags wrapper (2026-10-13)

`Layout::drawBox(int,int,int,int,int,String)` now forwards one
Android-backed `String(const String&, false)` copy and flags `1`
instead of an extra copy and flags `0`. The current Android renderer
does not use bit 0 for visual placement, but this restores the
confirmed call contract. Native `gof2`, all 206 ARM units, the
production-method UI smoke and 37 verifier tests pass (one skipped).
Focused source-shape rises 79.2% to 100.0%, strict fuzzy 50.0% to
100.0%, adding one linked-exact (not byte-exact) symbol. Full corpus:
4,718 comparisons, 4,323/4,524 covered, 201 missing, 92.95%
average source-shape, 81.37% strict fuzzy, 2,402 linked-exact and
1,008 byte-exact. Complete exact and extra-symbol sets show this
one linked-exact gain and no losses or other changes. Original
assets, visual output and ARM32 runtime remain untested. Next
priority: audit `Layout::formatNumber(int)` (83.6% source-shape),
including negative and grouping edge cases. See
[the wrapper audit](LAYOUT_DRAWBOX_WRAPPER_ARM_2026-10-13.md).

## Layout window-dimension reset (2026-10-13)

`Layout::resetWindowDimensions()` now reads `Vector.x/.y` directly
after each original `getPosition()` call, without extra indexing or
assignment ABI calls, and reevaluates each footer-button position.
The production-method UI smoke checks screen dimensions, all three
buttons and published back-button coordinates. Native `gof2`, all
206 ARM units and 37 verifier tests pass (one skipped). Focused
source-shape rises 77.8% to 99.8%, strict fuzzy 57.3% to 89.4%; it
remains non-exact. Full corpus: 4,718 comparisons, 4,323/4,524
covered, 201 missing, 92.94% average source-shape, 81.36% strict
fuzzy, 2,401 linked-exact and 1,008 byte-exact. Complete exact and
extra-symbol sets are unchanged. The original performs a back-button
position call before checking its pointer for null, and that risk
remains. Original assets, visual output and ARM32 runtime remain
untested. Next priority: audit the six-argument `Layout::drawBox`
overload (79.2% source-shape) for behavior and ABI gaps. See
[the reset audit](LAYOUT_RESET_WINDOW_DIMENSIONS_ARM_2026-10-13.md).

## Layout one-argument window wrapper (2026-10-13)

`Layout::drawWindow(String)` now copies the title with the
Android-backed non-reversing constructor and forwards `drawBG=false`
instead of incorrectly painting the background. The production-method
UI smoke covers frame/header placement without a background pattern.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Focused source-shape rises 76.1% to 100.0%, strict fuzzy
63.2% to 100.0%, adding one linked-exact (not byte-exact) symbol.
Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.94% average source-shape, 81.35% strict fuzzy, 2,401 linked-exact
and 1,008 byte-exact. Complete exact and extra-symbol comparisons
show one linked-exact gain, no losses and no extra-symbol change.
Original assets, visual output and ARM32 runtime remain untested.
Next priority: audit `Layout::resetWindowDimensions()` (77.8%
source-shape) for remaining behavior and ABI gaps. See
[the wrapper audit](LAYOUT_ONE_ARG_WINDOW_ARM_2026-10-13.md).

## Layout full-screen window wrapper (2026-10-13)

`Layout::drawWindow(String,bool)` now obtains screen and top-inset
dimensions from Android-backed globals instead of unassigned holders,
and forwards the title through the original `String(const String&,
false)` constructor. The production-method UI smoke covers both
background flags and the original double top-inset adjustment. Native
`gof2`, all 206 ARM units and 37 verifier tests pass (one skipped).
Focused source-shape rises 74.7% to 100.0%, strict fuzzy 40.0% to
100.0%, adding one linked-exact (not byte-exact) symbol. Full corpus:
4,718 comparisons, 4,323/4,524 covered, 201 missing, 92.93% average
source-shape, 81.34% strict fuzzy, 2,400 linked-exact and 1,008
byte-exact. Complete exact and extra-symbol comparisons show one
linked-exact gain, no losses and no extra-symbol change. Original
assets, visual output and ARM32 runtime remain untested. Next
priority: the one-argument `Layout::drawWindow(String)` overload
(76.1% source-shape). See [the wrapper audit](LAYOUT_WINDOW_WRAPPER_ARM_2026-10-13.md).

## Layout header (2026-10-13)

`Layout::drawHeader(String,bool)` now uses Android-backed canvas,
cap image, layout metrics and font instead of unassigned holders,
and draws the help button through its normal method only for a
requested transition without an open choice window. The
production-method UI smoke covers empty/nonempty title, cap/pattern
geometry and three help-button branches. Native `gof2`, all 206 ARM
units and 37 verifier tests pass (one skipped). Focused source-shape
rises 74.2% to 99.8%, strict fuzzy 32.1% to 97.7%; it remains
non-exact. Full corpus: 4,718 comparisons, 4,323/4,524 covered,
201 missing, 92.93% average source-shape, 81.33% strict fuzzy,
2,399 linked-exact and 1,008 byte-exact. Exact and extra-symbol sets
are unchanged. Original assets, visual output and ARM32 runtime remain
untested. Next priority: `Layout::drawWindow(String,bool)`.
See [the header audit](LAYOUT_DRAW_HEADER_ARM_2026-10-13.md).

## Layout framed window (2026-10-13)

The six-argument `Layout::drawWindow` now reads the Android-backed
canvas, font and layout metrics, paints the frame with `0x255d8dff`,
uses the caller's header-image Y and restores the prior canvas color.
The production-method UI smoke covers background on/off and frame
geometry. Native `gof2`, all 206 ARM units and 37 verifier tests pass
(one skipped). Focused source-shape rises 24.0% to 100.0%, strict
fuzzy 17.5% to 100.0%; it is not linked- or byte-exact. Full corpus:
4,718 comparisons, 4,323/4,524 covered, 201 missing, 92.92%
average source-shape, 81.32% strict fuzzy, 2,399 linked-exact and
1,008 byte-exact. Exact and extra-symbol sets are unchanged. The
original title condition appears unreachable for ordinary nonempty
strings; it was preserved and remains a behavioral uncertainty for
unusual inputs or other platforms. ARM32 runtime and visual output
remain untested. Next priority: `Layout::drawHeader(String,bool)`.
See [the framed-window audit](LAYOUT_DRAW_WINDOW_ARM_2026-10-13.md).

## Layout empty footer (2026-10-13)

`Layout::drawEmptyFooter` now uses Android-backed canvas, color,
image IDs, screen dimensions and footer height, and calls the normal
back-button draw path only when requested. The production-method
TouchButton/Layout host smoke covers both `showBack` branches,
geometry and image flags. Native `gof2`, all 206 ARM units and 37
verifier tests pass (one skipped). Focused source-shape and strict
fuzzy rise from 43.0% / 20.5% to 100.0% / 100.0%, adding one
linked-exact (not byte-exact) symbol. Full corpus: 4,718 comparisons,
4,323/4,524 covered, 201 missing, 92.90% average source-shape,
81.28% strict fuzzy, 2,399 linked-exact and 1,008 byte-exact.
Complete exact and extra-symbol comparisons show one linked-exact
gain, no losses and no extra-symbol change. ARM32 runtime and visual
output remain untested. Next priority: bounded
`Layout::drawWindow(String,int,int,int,int,bool)` recovery (24.0%
source-shape). See [the empty-footer audit](LAYOUT_EMPTY_FOOTER_ARM_2026-10-13.md).

## Layout tip initialization (2026-10-13)

`Layout::initTip` now selects a tip text ID through the Android-backed
global random service (`340 + nextInt(13)`), wraps text with the global
font, canvas and layout metrics, and releases the previous line array
without an extra reallocation. A production-method host smoke covers
two consecutive calls. Native `gof2`, all 206 ARM units and 37 verifier
tests pass (one skipped). Focused source-shape rises 79.2% to 100.0%
and strict fuzzy 50.5% to 97.0%; it is not linked- or byte-exact.
Full corpus: 4,323/4,524 covered, 201 missing, 92.88% average
source-shape, 81.27% strict fuzzy, 2,398 linked-exact and 1,008
byte-exact. Exact and extra-symbol sets are unchanged. The one fewer
comparison is the now-unused non-exact `ArrayRemoveAll<String*>`
instantiation, not a coverage loss. ARM32 runtime and real text assets
remain untested. Next priority: `Layout::drawEmptyFooter`. See
[the initialization audit](LAYOUT_INIT_TIP_ARM_2026-10-13.md).

## Layout tip (2026-10-13)

`Layout::drawTip` now uses the original Android-backed canvas,
dimensions, layout metrics, font and globals service instead of
unassigned holders. It draws the box, image and centered lines with
the original arguments; null `tipLines` remains a no-op. Focused
source-shape and strict fuzzy rise from 12.9% / 10.0% to 100.0% /
100.0%, adding one linked-exact match (not byte-exact). Native `gof2`,
all 206 ARM units, the TouchButton/Layout host smoke and 37 verifier
tests pass (one skipped). Full corpus: 4,719 comparisons, 4,323/4,524
covered, 201 missing, 92.88% average source-shape, 81.25% strict
fuzzy, 2,398 linked-exact and 1,008 byte-exact. Complete exact and
extra-symbol comparisons show one linked-exact gain, no losses and no
extra-symbol change. ARM32 runtime and visual output remain untested.
Next priority was `Layout::initTip`. See [the tip audit](LAYOUT_TIP_ARM_2026-10-13.md).

## Layout scrollbar (2026-10-13)

The shared scrollbar now uses the Android canvas, track width and color
sequence, and draws its top cap at the calculated thumb position rather
than screen `y=0`. It serves `StatusWindow`, `WantedWindow`,
`HangarWindow` and `ScrollTouchWindow`. A production-method host smoke,
the existing TouchButton/Layout smoke, native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). Focused source-shape rises
39.1% to 93.1% and strict fuzzy 15.9% to 35.1%; the method remains
non-exact. Full corpus: 4,323/4,524 covered, 92.86% average
source-shape, 81.24% strict fuzzy, 2,397 linked-exact and 1,008
byte-exact, without exact-symbol loss. Visual/ARM32 rendering remains
unverified. Next priority: `Layout::drawTip`. See
[the scrollbar audit](LAYOUT_SCROLLBAR_ARM_2026-10-13.md).

## StatusWindow touch achievements service (2026-10-13)

Android `OnTouchBegin` and `OnTouchMove` read medals and elite state via
`Globals::achievements`. The local methods used the separate
`Achievements::gAchievements` holder; both now use the Android service,
consistent with the constructor and `OnTouchEnd`. The production-method
host smoke exercises phone/iPad and locked/unlocked dispatch with that
duplicate holder null. Native `gof2`, all 206 ARM units and 37 verifier
tests pass (one skipped). Focused source-shape remains 98.5% for begin
and 99.0% for move; no exact claim is added. The full corpus remains
4,323/4,524 covered, 92.85% average source-shape, 81.23% strict fuzzy,
2,397 linked-exact and 1,008 byte-exact, with unchanged exact and extra
sets. Real touch input on ARM32 remains unverified. Next priority:
remaining substantial `StatusWindow::draw` gaps. See
[the touch-globals follow-up](STATUSWINDOW_TOUCH_GLOBALS_ARM_2026-10-13.md).

## StatusWindow language-9 separator (2026-10-13)

The Android `draw` method constructs an empty label separator for
language 9; the local code had inserted byte `0xA1`. The Android ELF's
literal at `0x1ca472` is `00`, and the same literal supplies empty box
labels. A production-block host smoke now checks both `":"` in other
languages and the empty separator in language 9. Native `gof2`, all
206 ARM units and 37 verifier tests pass (one skipped). `draw` remains
84.6% source-shape and 21.4% strict fuzzy; full corpus remains
4,323/4,524 covered, 92.85% average source-shape, 81.23% strict fuzzy,
2,397 linked-exact and 1,008 byte-exact, with identical exact and extra
sets. Real text assets and ARM32 rendering remain unverified. Next
priority: remaining `draw` string/formatting blocks. See
[the info-card follow-up](STATUSWINDOW_DRAW_INFO_CARD_ARM_2026-10-13.md).

## StatusWindow destructor ownership (2026-10-13)

The Android destructor releases the four owned arrays and destroys them
without the local code's redundant `ArrayRemoveAll` calls. A
production-destructor host smoke, the existing native ABI/lifetime smoke,
native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Both destructor aliases rise from 86.6% to 100.0%
source-shape and become linked-exact, but not byte-exact. The full
corpus covers 4,323/4,524 originals with 4,719 comparisons, 92.85%
average source-shape, 81.23% strict fuzzy, 2,397 linked-exact and 1,008
byte-exact. No old exact symbols are lost; the one fewer comparison is
an unused non-exact template instantiation in this unit, not a coverage
loss. ARM32 runtime remains unverified. Next priority: bounded
`StatusWindow::draw` recovery. See [the destructor audit](STATUSWINDOW_DTOR_ARM_2026-10-13.md).

## StatusWindow touch-end cleanup (2026-10-13)

Medal selection now reads the Android-backed achievements service and
replaces detail strings without an extra array reallocation after release.
A production-method host smoke covers scroll state, phone/iPad tabs,
locked and elite medals, detail text and help IDs. Native `gof2`, all
206 ARM units and 37 verifier tests pass (one skipped). `OnTouchEnd`
source-shape rises 89.5% to 90.1%, strict fuzzy 48.4% to 48.7%; it is
not exact. Full corpus remains 4,323/4,524 covered, 92.85% average
source-shape, 81.23% strict fuzzy, 2,395 linked-exact and 1,008
byte-exact, with no exact-set loss or extra-symbol change. ARM32 runtime
and real assets remain unverified. Next priority: scroll geometry helpers.
See [the touch-end audit](STATUSWINDOW_TOUCH_END_ARM_2026-10-13.md).

## StatusWindow constructor service bindings (2026-10-13)

The constructor now uses the Android-backed shared text, layout,
dimensions, iPad flag, achievements and canvas services instead of five
unassigned holders. It positions the second tab from the first tab's
width and applies the iPad medal-height branch. A production-constructor
host smoke checks both device modes; native `gof2`, all 206 ARM units
and 37 verifier tests pass (one skipped). Constructor source-shape rises
38.5% to 96.0%, strict fuzzy 16.6% to 53.1%; neither alias is exact.
The full corpus covers 4,323/4,524 original functions, with 92.85%
average source-shape, 81.23% strict fuzzy, 2,395 linked-exact and 1,008
byte-exact. Exact-symbol sets and the 123 extra-symbol set are unchanged.
Real assets and ARM32 runtime are unverified. Next priority is
`StatusWindow::OnTouchEnd`. See [the constructor audit](STATUSWINDOW_CTOR_GLOBALS_ARM_2026-10-13.md).

## StatusWindow medal grid and detail pane (2026-10-13)

The Android grid uses the two-argument `TouchButton::setPosition`, retaining
the button's flags. On iPad its header follows the scroll offset, and the
selected-medal text receives a calculated line position rather than a
byte-truncated screen height. A production-block host smoke checks iPad
and phone positions, culling, header and detail pane. Native `gof2`, all
206 ARM translation units and 37 verifier tests pass (one skipped).
`draw` source-shape rises 84.4% to 84.6%, while strict fuzzy falls 21.8%
to 21.4%; it is not exact. Full corpus remains 4,323/4,524 covered,
92.82% average source-shape, 81.21% strict fuzzy, 2,395 linked-exact and
1,008 byte-exact, with no exact-symbol losses or extra-symbol growth.
Interactive selection, asset-backed UI and ARM32 runtime remain unverified.
Next priority: `StatusWindow` construction and its unassigned global holders.
See [the medal-grid audit](STATUSWINDOW_DRAW_MEDALS_ARM_2026-10-13.md).

## StatusWindow standing and statistics table (2026-10-13)

The Android draw section now includes its two headers, four boxed panels,
four standing labels, and two columns containing 11 statistics rows. Five
values use direct `Status` fields at the ARM-confirmed offsets; their gameplay
names remain unproven. The earlier code omitted those rows and drew six
others at one coordinate. A production-block host smoke checks six box calls,
four standing labels, and all table rows/positions; the existing standing
and info-card smokes, native `gof2`, all 206 ARM translation units, and 37
verifier tests pass (one skipped). `draw` source-shape rises 60.1% to 84.4%,
strict fuzzy 15.9% to 21.8%, with 1,728 local versus 2,324 original
instructions; it is not exact. Full corpus: 4,323/4,524 covered, 92.82%
average source-shape, 81.21% strict fuzzy, 2,395 linked-exact and 1,008
byte-exact, with no exact-symbol losses or extra-symbol growth. Real assets,
full interactive UI, and ARM32 runtime remain unverified. Next priority is
the medal-grid/detail pane and final header/footer layout. See
[the table audit](STATUSWINDOW_DRAW_STATS_TABLE_ARM_2026-10-13.md).

## StatusWindow credits and ship info card (2026-10-13)

The Android draw card places credits, level and time on separate
line-height-spaced rows, with ship name and labeled firepower/HP values
opposite. The local body previously drew several values at one point and
formatted float return bits as an integer with `%`; the recovered block
now shows decimal firepower and the source-backed positions and labels.
A production-body host smoke passes eight text/coordinate checks; native
`gof2`, 206/206 ARM translation units, and 37 verifier tests pass (one
skipped). `draw` source-shape rises 57.4% to 60.1%, strict fuzzy 13.8%
to 15.9%, still not exact. Full corpus remains 4,323/4,524 covered,
92.82% average source-shape, 81.21% strict fuzzy, 2,395 linked-exact
and 1,008 byte-exact, with no old exact-symbol losses. The remaining
two-column statistics table, real assets and ARM32 UI remain
unverified. See [the info-card audit](STATUSWINDOW_DRAW_INFO_CARD_ARM_2026-10-13.md).

## StatusWindow draw service bindings (2026-10-13)

The Android `draw` uses shared canvas, layout, dimensions, iPad flag, font,
image factory, game text, globals and status. The local method instead
dereferenced unassigned holders and collapsed to 38 ARM instructions.
Restoring those bindings, verified text IDs, ship-name offset and the
playing-time output raises `draw` source-shape from 2.6% to 57.4% and
strict fuzzy from 1.4% to 13.8%; 1,047 local instructions remain versus
2,324 original. Native `gof2`, 206/206 ARM units, a draw-prefix smoke,
the existing standing smoke and 37 verifier tests pass (one skipped).
Full corpus remains 4,323/4,524 covered, with 92.82% average source-shape,
81.21% strict fuzzy, 2,395 linked-exact and 1,008 byte-exact; no exact
symbols were lost. The full UI, asset-backed rendering, several stats rows
and exact ARM shape are not yet verified. The subsequent info-card audit
above addresses credits and ship text; the statistics table remains next. See
[the draw-globals audit](STATUSWINDOW_DRAW_GLOBALS_ARM_2026-10-13.md).

## StatusWindow character/rank initialization (2026-10-13)

`reInit` now uses the Android-backed global image factory, status standing,
and canvas rather than unassigned holders and declared-only achievement
helpers. Its character descriptor supplies all five words read by
`ImageFactory::loadChar`; the old one-word descriptor risked an out-of-bounds
read. A focused host smoke passes rank-image selection and first-image size
reads without original assets. Native `gof2`, 206/206 ARM translation units,
and 37 verifier tests pass (one skipped). The method rises 92.0% to 97.5%
source-shape and 65.0% to 89.1% strict fuzzy, but is not exact. Full corpus
remains 4,323/4,524 covered, with average source-shape 92.81%, strict
fuzzy 81.21%, 2,395 linked-exact and 1,008 byte-exact; the complete
exact-symbol sets show no losses. Live image loading and ARM32 UI remain
untested. The subsequent draw audit above addresses its unassigned
global holders. See [the reInit audit](STATUSWINDOW_REINIT_ARM_2026-10-13.md).

## StatusWindow medal-hint text (2026-10-13)

`getMedalHintText` now reads the Android-backed achievements, status, and
game-text globals instead of an unassigned text pointer. Its header and
missing-requirement separators match the Android literals; a focused host
smoke passes all five medal branches and empty cases without original
assets. Native `gof2` and all 206 ARM translation units build. Focused
source-shape rises 90.6% to 92.1%, strict fuzzy 44.7% to 52.1%, with no
family exact-count change; 37 verifier tests pass (one skipped). Full
corpus remains 4,323/4,524 covered, 92.80% average source-shape,
81.20% strict fuzzy, 2,395 linked-exact and 1,008 byte-exact, with no
exact losses. Live medal selection remains untested. The subsequent
reInit audit above addresses its unassigned image-factory holder. See
[the medal-hint audit](STATUSWINDOW_MEDAL_HINT_ARM_2026-10-13.md).

## StatusWindow touch release (2026-10-13)

`OnTouchEnd` now routes its layout, device flag, status, game text, globals,
and font reads through the Android-backed shared fields instead of six
unassigned pointer holders. Medal values now enter the integer `String`
constructor rather than being dereferenced as pointers, and replacement
uses the defined `Status::replaceHash` overload. Focused ARM source-shape
rises 82.7% to 89.5%, strict fuzzy 39.7% to 48.4%; the function is not
exact. Native `gof2`, 206/206 ARM units, an integer-String ABI smoke, and
37 verifier tests pass (one skipped). Full corpus remains 4,323/4,524
covered, 92.80% average source-shape, 81.20% strict fuzzy, 2,395
linked-exact and 1,008 byte-exact, with no exact losses. Interactive
medal/touch behavior and ARM32 runtime remain untested. The subsequent
medal-hint audit above addresses the separate game-text binding. See
[the release audit](STATUSWINDOW_TOUCH_END_ARM_2026-10-13.md).

## Gamepad software-stick bridge (2026-10-13)

The Android `setValuesForGamepad` writes the saved pair in a 28-byte
software-stick block whose initialized data begins with an enabled joystick
flag and six centered floats. The local setter previously wrote disconnected
private floats while its joystick flag started at zero. The shared block now
matches all 28 original initializer bytes, and the setter becomes 100%
source-shape/strict-fuzzy and linked-exact. Native `gof2`, all 206 ARM
translation units, and 37 verifier tests pass (one skipped). Full corpus
stays 4,323/4,524 covered and 1,008 byte-exact; average source-shape rises
92.79% to 92.80%, strict fuzzy 81.18% to 81.20%, and linked-exact 2,394 to
2,395, with no old exact losses. Hardware steering and the large
`simulateTouch` body remain unverified. See
[the gamepad audit](GAMEPAD_SOFTWARE_STICK_ARM_2026-10-13.md).

## Initialized virtual-key table (2026-10-13)

The Android `keys` table is restored as 48 initialized records rather than
zero-filled local state. A local ARM-object comparison matched all 4,224
initializer bytes and all 78 relocation targets, offsets, and types against
the Android ELF. The touch-coordinate X/Y globals now have integer types,
consistent with the Android input path. Focused `VirtualInput` average
source-shape rises 73.87% to 74.59%; linked-exact rows rise 3 to 8.
Native `gof2`, 206/206 ARM translation units, and 37 verifier tests pass
(one skipped). Full corpus stays 4,323/4,524 covered, 201 missing,
92.79% average source-shape at report precision, and 1,008 byte-exact;
strict fuzzy rises 81.17% to 81.18%, linked-exact 2,389 to 2,394,
with no previously exact losses. Device-level key input is still untested.
The later gamepad bridge recovery above connects `setValuesForGamepad` to
`simulateTouch`; hardware behavior remains untested. See
[the table audit](VIRTUAL_KEY_TABLE_ARM_2026-10-13.md).

## Virtual key dispatch and shared flags (2026-10-13)

Android ARM and independent Ghidra output establish the 88-byte virtual-key
layout, key predicate at `+0x54`, touch coordinate pointers at `+0x28/0x2c`,
and the global input-state writes. `keyPressed` now follows the Android
multi-pass exit condition and uses the same speed-up/slow-motion flags exposed
by the Android getters; `keyReleased` updates the actual overlay and steering
state rather than uninitialized pointer placeholders. Focused ARM source-shape
is 95.8% for `keyPressed` (from 1.1%) and 97.7% for `keyReleased` (from 0.1%).
The two getters rise from 59.4% to 93.0%. None of those four functions is
exact. Native `gof2`, 206/206 ARM units and 37 verifier tests pass (one
skipped). Full corpus stays 4,323/4,524 covered, 201 missing, and 1,008
byte-exact; linked-exact rises 2,388 to 2,389 with no old exact losses.
Average source-shape rises 92.70% to 92.79%, strict fuzzy 81.07% to 81.17%.
This historical snapshot predates the initialized table recovery above;
end-to-end key handling is still not claimed operational. Next exercise
press/release with runtime fixtures. See
[the input audit](VIRTUAL_INPUT_KEY_DISPATCH_ARM_2026-10-13.md).

## StatusWindow touch globals (2026-10-13)

`OnTouchMove` and `OnTouchBegin` now read the Android-backed
`Globals::layout`, `Globals::h`, `Globals::mouse_wheel`, and `Globals::iPad`
fields instead of private null pointer placeholders. ARM source-shape moves
from 0.2% to 99.0% for `OnTouchMove` and from 91.2% to 98.5% for
`OnTouchBegin`; neither is linked- or byte-exact. `OnTouchEnd` remains
82.7%. Native `gof2`, all 206 ARM translation units, and 37 verifier tests
pass (one skipped). Full corpus stays 4,323/4,524 covered, 201 missing,
2,388 linked-exact and 1,008 byte-exact with no exact losses; average
source-shape rises 92.68% to 92.70% and strict fuzzy 81.04% to 81.07%.
Interactive touch behaviour and ARM32 runtime remain untested. The much
larger `ParticleSystemMesh::emitTrail` stub still needs recovery; its
decompiler math is ambiguous, so no speculative implementation was added.
See [the touch audit](STATUSWINDOW_TOUCH_GLOBALS_ARM_2026-10-13.md).

## Sprite canvas and scale dispatch (2026-10-13)

Android ARM `Sprite` constructors and drawing methods use `Globals::Canvas`,
not the separately initialized `PaintCanvas::gCanvas` alias used by the
recovered source. The multi-image `draw` branch selects unscaled output on
`scaleX == 1.0f` alone; the former `scaleX || scaleY` test silently dropped
horizontal scaling when only Y was one. Both routes now match ARM. An
extracted-body smoke executes the production Sprite and
`ImageFactory::reload` paths with distinct canvas fixtures and asymmetric
scales. Native `gof2`, 206/206 ARM units, both image smokes, and 37 verifier
tests pass (one skipped). Full corpus stays 4,323/4,524 covered,
2,388 linked-exact and 1,008 byte-exact with no exact losses; average
source-shape is 92.67619% (slightly down from 92.67655%) and strict
fuzzy 81.04487% (slightly up from 81.04394%). Real assets, OpenGL, and
ARM32 runtime remain untested. Next recover the separate
`ParticleSystemMesh::emitTrail` stub. See
[the Sprite audit](SPRITE_CANVAS_SCALE_ARM_2026-10-13.md).

## ImageFactory portrait data and dispatch (2026-10-13)

The Android 13×4 part-count/ID tables and four 13×4×2 coordinate tables
replace null aliases in `ImageFactory`; its constructor, random descriptor
generation, portrait loading, and image drawing now use live `Globals`
services. `ImagePart` creation/draw uses the same canvas. All 520 recovered
table integers match the local Android ELF, and an extracted-body host smoke
passes across screen modes and drawing routes. `ImageFactory` family
source-shape rises 76.31% → 97.87%; five comparison rows become linked-exact.
Native `gof2`, 206/206 ARM units, and 37 verifier tests pass (one skipped).
Full corpus: 4,720 comparisons, 4,323/4,524 covered, 201 missing,
92.67655% average source-shape, 81.04394% strict fuzzy, 2,388 linked-exact,
1,008 byte-exact, and no old exact losses. Original assets, OpenGL, the
production `reload`/`Sprite` path, and ARM32 runtime remain untested.
Next audit integrated image-resource ownership and drawing. See
[the portrait audit](IMAGE_FACTORY_PORTRAITS_ARM_2026-10-13.md).

## Mesh/Transform clone and recursive release smoke (2026-10-13)

The clone smoke now executes production `Mesh::Mesh(Mesh *)`,
`MeshRelease`, `MeshReleaseIntern`, and `TransformRelease` bodies alongside
the `Transform` constructors/destructor. It verifies shared geometry/VBO
ownership, separately cloned mesh animation, shader tangents, and recursive
mesh-slot clearing. The Android-backed release traversal does **not**
delete child `Transform` objects; the test explicitly frees them after
traversal. This remains a caller-ownership question, not a source change.
No production code or ARM metrics changed: latest full corpus remains
4,323/4,524 covered, 92.6014% average source-shape, 2,383 linked-exact,
1,008 byte-exact. See [the clone/release audit](MESH_TRANSFORM_CLONE_RELEASE_SMOKE_2026-10-13.md).
Next trace the owner of cloned child transforms and test real asset lifetimes.

## Transform clone keyframe-list ownership (2026-10-13)

`Transform(Transform *)` now uses Android-backed `ArraySet` to copy
keyframe pointers, keeping the destination's capacity tied to the
copied count rather than inheriting spare source capacity. An
extracted-body native smoke checks a non-null clone with a child
transform, mesh copies, shared frame objects, and spare source
capacity. Both constructor variants remain 91.5% source-shape and
rise 57.6% to 58.1% strict fuzzy; neither is exact. Native `gof2`,
206/206 ARM units, 37 verifier tests (one skipped), and adjacent
release smoke pass. Full corpus: 4,720 comparisons, 4,323/4,524
covered, 201 missing, 92.6014% average source-shape, 80.9370%
strict fuzzy, 2,383 linked-exact, 1,008 byte-exact, with no exact
losses. The two new linked rows are helper instances, not the clone
constructor. The later extracted-body smoke covers actual mesh copying
and release traversal; assets and ARM32 runtime remain untested. See
[the keyframe-list audit](TRANSFORM_CLONE_KEYFRAMES_ARM_2026-10-13.md).

## Transform color ABI slot (2026-10-13)

Android ARM constructors, animation update, and canvas setter all
write transform packed color at `0x48`; the local constructors and
animation update previously wrote the unrelated `0xf4` field while
rendering read `0x48`. The `Transform` layout and color callers now
agree on `0x48`, with a match-build offset assertion; `0xf4` remains
unnamed because its role is unconfirmed. Native `gof2`, 206/206 ARM
units, 37 verifier tests (one skipped), and related canvas/transform
smokes pass. Full corpus remains 4,718 comparisons, 4,323/4,524
covered, 201 missing, 92.5982% average source-shape, 80.9287%
strict fuzzy, 2,381 linked-exact, and 1,008 byte-exact, with no
exact-set losses. Real rendering and ARM32 runtime are untested.
Next audit non-null transform-clone ownership. See
[the color-slot audit](TRANSFORM_COLOR_OFFSET_ARM_2026-10-13.md).

## Transform null-source clone (2026-10-13)

`Transform(Transform *)` no longer explicitly destroys a temporary
that C++ also destroys at scope exit when its source is null. Android
ARM at `0x74918` shows one construction and one destruction. Both
constructor variants rise 90.6% to 91.5% source-shape and 57.6% to
57.7% strict fuzzy; neither is exact. Native `gof2`, 206/206 ARM
units, 37 verifier tests (one skipped), and an extracted-branch
ownership smoke pass. Full corpus: 4,718 comparisons, 4,323/4,524
covered, 201 missing, 92.5982% average source-shape, 80.9288%
strict fuzzy, 2,381 linked-exact, 1,008 byte-exact; no exact losses.
The full non-null clone and real animation lifetime remain untested.
Next audit shared-keyframe and child-mesh ownership in that path. See
[the null-clone audit](TRANSFORM_NULL_CLONE_ARM_2026-10-13.md).

## Transform keyframe ownership (2026-10-13)

`Transform::~Transform` now uses the Android-backed
`ArrayReleaseClasses<KeyFrame *>` path for owned keyframes, instead
of reallocating their array immediately before member destruction.
Both destructor variants rise from 55.0% source-shape / 43.1%
strict fuzzy to 100.0% / 100.0% and become linked-exact. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and an
owned/shared/empty-frame host smoke pass. Full corpus: 4,718
comparisons, 4,323/4,524 covered, 201 missing, 92.5979% average
source-shape, 80.9287% strict fuzzy, 2,381 linked-exact, 1,008
byte-exact; no prior exact rows lost. The third new linked-exact row
is a second unit instance of the already-exact helper, not a third
independent recovered behavior. Real animations and ARM32 runtime
remain untested. Next audit the `Transform(Transform *)` clone and
shared-keyframe lifetime. See [the destructor audit](TRANSFORM_DESTRUCTOR_ARM_2026-10-13.md).

## Mesh release dispatch (2026-10-13)

`AbyssEngine::MeshRelease` now calls the typed `MeshReleaseIntern`
after `TransformRelease`, as Android ARM at `0x6c362` does. The old
zero-initialized, never-assigned function pointer could crash on any
non-null release. Focused source-shape rises 72.0% to 98.3% and
strict fuzzy 31.2% to 83.3%; the function is not exact. Native
`gof2`, 206/206 ARM units, 37 verifier tests (one skipped), and an
extracted-body host smoke pass. The full corpus stays at 4,717
comparisons, 4,323/4,524 covered, and 201 missing. Average
source-shape rises 92.5716% to 92.5772%, strict fuzzy 80.8895% to
80.9006%, while the 2,378 linked-exact and 1,008 byte-exact symbol
sets remain unchanged. Real GL, assets, and ARM32 runtime are
untested. Next audit animated/shared mesh ownership and
`TransformRelease`. See [the mesh-release audit](MESH_RELEASE_ARM_2026-10-13.md).

## Sprite-system resource release dispatch (2026-10-13)

`PaintCanvas::ReleaseSpriteSystemResource(unsigned)` now calls the
typed `AbyssEngine::SpriteSystemRelease` instead of an unresolved
shim. Android ARM at `0x7c7f4` confirms its index guard and slot
dispatch. The wrapper stays 100.0% source-shape, 100.0% strict fuzzy,
and linked-exact (not byte-exact). An extracted-body host smoke checks
the wrapper and release path, including invalid index, null slot,
CPU-array release, adjacent-slot preservation, and repeat release;
it stubs nested `MeshRelease`. Native `gof2`, 206/206 ARM units, and
36 verifier tests pass (one skipped). Full corpus stays 4,717
comparisons, 4,323/4,524 covered, 201 missing, 92.5716% average
source-shape, 80.8895% average strict fuzzy, 2,378 linked-exact,
and 1,008 byte-exact, with no exact-set losses. Real mesh ownership,
assets, and ARM32 runtime are untested. Next audit `MeshRelease`,
currently 72.0% source-shape. See
[the sprite-release audit](PAINTCANVAS_SPRITE_RELEASE_ARM_2026-10-13.md).

## VBO upload body (2026-10-13)

`AbyssEngine::MeshConvertToVBOIntern(Mesh *)` now snapshots raw
attribute pointers before the first GL call and rereads the vertex
count for each upload/accounting step, matching Android ARM at
`0x6d510`. Focused source-shape rises 95.9% to 100.0% and strict
fuzzy 83.7% to 100.0%; it is still not linked- or byte-exact.
A recording-GL extracted-body smoke verifies guards, buffer order and
sizes, count/pointer load timing, byte accounting, raw-data retention
and release, and the GL-error branch. Native `gof2`, 206/206 ARM
units, the new and adjacent canvas smokes, and 36 verifier tests pass
(one skipped). Full corpus remains 4,717 comparisons, 4,323/4,524
covered, 201 missing; average source-shape rises 92.5708% to
92.5716%, strict fuzzy 80.8861% to 80.8895%, and 100%-fuzzy
functions 2,528 to 2,529. The 2,378 linked-exact and 1,008
byte-exact symbol sets have no losses. Real GPU allocation, assets,
ARM32 runtime, and error-path resource lifetime remain unverified.
Next audit canvas sprite-system resource release. See
[the VBO upload audit](MESH_VBO_INTERN_ARM_2026-10-13.md).

## Indexed VBO conversion dispatch (2026-10-13)

`PaintCanvas::MeshConvertToVBO(unsigned)` now calls the typed
`AbyssEngine::MeshConvertToVBO` rather than an unresolved shim.
Android ARM at `0x7a292` confirms the index guard, mesh lookup, and
call. The wrapper remains 100.0% source-shape, 100.0% strict fuzzy,
and linked-exact (not byte-exact). Native `gof2`, 206/206 ARM units,
an extracted-body dispatch smoke, and 36 verifier tests pass (one
skipped). Full corpus remains 4,717 comparisons, 4,323/4,524
covered, 201 missing, 92.5708% average source-shape, 80.8861%
average strict fuzzy, 2,378 linked-exact, and 1,008 byte-exact,
with no exact-set losses. GPU upload, assets, and ARM32 runtime are
untested. Next audit the converter's
`AbyssEngine::MeshConvertToVBOIntern(Mesh *)` body. See
[the indexed-VBO audit](PAINTCANVAS_INDEX_VBO_ARM_2026-10-13.md).

## Recursive mesh material propagation (2026-10-13)

`PaintCanvas::MeshChangeMaterialIntern(Mesh *, Material *)` now
passes the material to the animated transform after assigning it to
the mesh. Its former two-argument unresolved shim lost that argument;
the corrected out-of-line compatibility entry preserves the already
linked-exact mesh function. Android ARM confirms the `+0x30` material
field, `+0x34` animation link, null guards, and recursive mesh/child
traversal. Focused source-shape remains 100.0% for both overloads;
strict fuzzy stays 100.0% for mesh and 92.3% for transform. Native
`gof2`, 206/206 ARM units, an extracted-body host smoke, and 36
verifier tests pass (one skipped). Full corpus remains 4,717
comparisons, 4,323/4,524 covered, 201 missing, 92.5708% average
source-shape, 80.8861% average strict fuzzy, 2,378 linked-exact,
and 1,008 byte-exact, with no exact-set losses. One resolved
compatibility entry increases extra symbols to 125. Material lifetime,
rendering, malformed cyclic graphs, and ARM32 runtime remain
untested. Next audit indexed `MeshConvertToVBO`. See
[the material-propagation audit](PAINTCANVAS_MATERIAL_PROPAGATION_ARM_2026-10-13.md).

## Shader-animation value propagation (2026-10-13)

`PaintCanvas::MeshChangeShaderAnimValue` now passes the value and mode
through animated meshes and nested transforms using its typed
overloads. The mesh overload previously called a two-argument shim
that lost both arguments; the transform overload used two unresolved
recursive shims. Android ARM confirms modes 1/2/4 and the recursive
dispatch. Focused source-shape stays 100.0% for both functions, while
strict fuzzy rises 58.8% to 76.5% for the mesh overload and stays
97.1% for the transform overload. Native `gof2`, 206/206 ARM units,
an extracted-body smoke, adjacent canvas smoke, and 36 verifier tests
pass (one skipped). Full corpus stays 4,717 comparisons,
4,323/4,524 covered, 201 missing, and 92.5708% average source-shape;
average strict fuzzy rises 80.8823% to 80.8861%. The 2,378
linked-exact and 1,008 byte-exact symbol sets have no losses.
Shaders, original assets, malformed cyclic graphs, and ARM32 runtime
remain untested. Next audit recursive mesh material propagation. See
[the shader-animation audit](PAINTCANVAS_SHADER_ANIM_VALUE_ARM_2026-10-13.md).

## Mesh and transform triangle counts (2026-10-13)

The mesh count and indexed transform count now reach the typed
recursive `TransformGetTriCount(Transform *)` implementation instead
of an unresolved shim. The indexed wrapper retains its external-call
shape and linked-exact match through an out-of-line compatibility
entry; a direct same-unit call lost that match. Android ARM confirms
the index guard, null behavior, `indexCount / 3`, and recursive mesh
and child counts. Focused family source-shape remains 98.2% and
strict fuzzy 92.2%. Native `gof2`, 206/206 ARM units, an
extracted-body smoke, and 36 verifier tests pass (one skipped).
Full corpus remains 4,717 comparisons, 4,323/4,524 covered,
201 missing, 92.5708% average source-shape, 80.8823% average strict
fuzzy, 2,378 linked-exact, and 1,008 byte-exact, with no exact-set
losses. The resolved compatibility entry adds one extra symbol.
Malformed cyclic graphs, assets, and ARM32 runtime remain untested.
Next audit the mesh/transform shader-animation value path. See
[the triangle-count audit](PAINTCANVAS_TRI_COUNT_ARM_2026-10-13.md).

## Indexed mesh draw dispatch (2026-10-13)

`PaintCanvas::DrawMesh(unsigned)` now calls the real typed `MeshDraw`
instead of an unresolved shim. Android ARM at `0x7968c` confirms the
index guard, mesh lookup, and direct draw call. The wrapper retains
100.0% source-shape, 100.0% strict fuzzy, and linked-exact status
(not byte-exact). Native `gof2`, 206/206 ARM units, and 36 verifier
tests pass (one skipped). Full corpus remains 4,717 comparisons,
4,323/4,524 covered, 201 missing, 92.5708% average source-shape,
80.8823% average strict fuzzy, 2,378 linked-exact, and 1,008
byte-exact; neither exact symbol set loses a member. This fixes the
dispatch dependency but does not test GL rendering or ARM32 runtime.
Next audit the shared `MeshGetTriCount` / `TransformGetTriCount`
path, where an unresolved transform-count shim remains.

## Recursive mesh draw (2026-10-13)

`PaintCanvas::DrawMesh(Mesh *, Matrix &, Matrix &, unsigned, Matrix &)`
now uses typed matrix, frustum, material-cache, and render calls. It
restores the animated pivot/world/UV path, the `view * world` render
matrix without losing the model matrix, and the original nested-mesh
and child-transform traversal. A zero color divisor and its unresolved
shims are gone. Focused source-shape rises 81.0% to 91.3%, strict fuzzy
15.9% to 28.4%; not exact and short of 98%. Native `gof2`, 206/206 ARM
units, four canvas smokes, and 36 verifier tests pass (one skipped).
Full corpus: 4,717 comparisons; coverage stays 4,323/4,524 with 201
missing. Average source-shape rises 92.5667% to 92.5708%; strict fuzzy
slips 80.8845% to 80.8823% as three newly emitted material-cache
template comparisons enter the denominator. The 2,378 linked-exact
and 1,008 byte-exact symbol sets have no losses. Full 3D rendering,
assets, and ARM32 runtime remain untested. Next audit the indexed
`DrawMesh(unsigned)` wrapper. See [the mesh-draw audit](PAINTCANVAS_MESH_DRAW_ARM_2026-10-13.md).

## Recursive transform draw (2026-10-13)

`PaintCanvas::DrawTransform(Transform *, const Matrix &, Matrix &)`
now composes parent, world, and optional keyframe matrices; calls
typed `DrawMesh`; and recursively dispatches visible children while
counting rejected or camera-less children. Its five unresolved shims
are removed. Focused source-shape rises 89.5% to 100.0%, strict fuzzy
64.2% to 94.9%; not exact. Native `gof2`, 206/206 ARM units, new and
adjacent transform smokes, and 36 verifier tests pass (one skipped).
Full coverage remains 4,323/4,524; average source-shape rises
92.5644% to 92.5667%, strict fuzzy 80.8780% to 80.8845%. The 2,378
linked-exact and 1,008 byte-exact symbol sets have no losses. Actual
mesh rendering, full 3D composition, and ARM32 runtime are untested.
Next audit the called `DrawMesh` path. See
[the recursive-transform audit](PAINTCANVAS_RECURSIVE_DRAW_TRANSFORM_ARM_2026-10-13.md).

## Indexed transform draw (2026-10-13)

`PaintCanvas::DrawTransform(index, viewMatrix)` now uses the original
null/rotation frustum paths, inverts the selected view or camera matrix,
updates the engine eye position, and calls the typed recursive draw.
The zero gravity scale and ten unresolved shims are gone. Focused
source-shape rises 86.6% to 93.5%, strict fuzzy 45.0% to 74.5%; not
exact. Native `gof2`, 206/206 ARM units, an extracted-body host smoke,
and 36 verifier tests pass (one skipped). Full coverage remains
4,323/4,524; average source-shape rises 92.5630% to 92.5644%,
strict fuzzy 80.8717% to 80.8780%. The 2,378 linked-exact and 1,008
byte-exact symbol sets have no losses. Recursive rendering, arbitrary
3D matrix composition, and ARM32 runtime are untested. Next audit the
nested `DrawTransform` method. See
[the indexed-transform audit](PAINTCANVAS_DRAW_TRANSFORM_ARM_2026-10-13.md).

## Point-frustum canvas wrapper (2026-10-13)

`PaintCanvas::CameraIsPointinViewFrustum` now preserves the Android
null-extra-matrix call when uninitialized and passes the recovered
gravity/orientation rotation when initialized. Focused source-shape
rises 86.0% to 100.0% and strict fuzzy 50.3% to 100.0%, but it is
not linked- or byte-exact. Native `gof2`, 206/206 ARM units, the
extended extracted-body frustum smoke, and 36 verifier tests pass
(one skipped). Full coverage stays 4,323/4,524; average source-shape
rises 92.5600% to 92.5630%, strict fuzzy 80.8612% to 80.8717%,
and 100%-fuzzy functions increase from 2,527 to 2,528. The 2,378
linked-exact and 1,008 byte-exact symbol sets have no losses. Real
camera geometry and ARM32 runtime remain untested. Next audit the
canvas `DrawTransform` gravity path. See
[the point-frustum audit](PAINTCANVAS_POINT_FRUSTUM_ARM_2026-10-13.md).

## Sphere-frustum canvas wrapper (2026-10-13)

`PaintCanvas::CameraIsSphereinViewFrustum` now preserves the Android
null-extra-matrix call when uninitialized and passes the recovered
gravity/orientation rotation when initialized. Focused source-shape
rises 84.6% to 98.4%, strict fuzzy 68.9% to 78.6%; not exact. Native
`gof2`, 206/206 ARM units, an extracted-body host smoke, and 36
verifier tests pass (one skipped). Full coverage stays 4,323/4,524;
average source-shape rises 92.5571% to 92.5600%, strict fuzzy
80.8591% to 80.8612%. The 2,378 linked-exact and 1,008 byte-exact
symbol sets have no losses. Real camera geometry and ARM32 runtime
remain untested. Next audit the adjacent point-frustum wrapper. See
[the sphere-frustum audit](PAINTCANVAS_SPHERE_FRUSTUM_ARM_2026-10-13.md).

## One-matrix sprite draw (2026-10-13)

`PaintCanvas::DrawSpriteSystem(index, Matrix)` now applies the
Android-confirmed fractional gravity/orientation rotation to its
matrix, inverts the result, and calls real `SpriteSystemDraw` with an
identity view. The zero gravity scale and eight unresolved shims are
gone. Focused source-shape rises 87.5% to 92.8%, strict fuzzy 36.4%
to 57.2%; not exact. Native `gof2`, 206/206 ARM units, two host smokes,
and 36 verifier tests pass (one skipped). Full coverage remains
4,323/4,524; average source-shape rises 92.5560% to 92.5571%,
strict fuzzy 80.8547% to 80.8591%. The 2,378 linked-exact and 1,008
byte-exact symbol sets have no losses. Arbitrary-matrix composition,
GL output, and ARM32 runtime remain untested. Next audit the canvas
frustum gravity path. See
[the one-matrix audit](PAINTCANVAS_SPRITE_DRAW_ONE_MATRIX_ARM_2026-10-13.md).

## Two-matrix sprite draw (2026-10-13)

`PaintCanvas::DrawSpriteSystem(index, Matrix, Matrix)`, called by
`ParticleSystemSprite`, now preserves Android's integer-truncated
gravity angle and uses the real matrix inverse, multiplication,
trigonometry, and `SpriteSystemDraw` calls. Focused source-shape rises
79.7% to 85.5%, strict fuzzy 27.2% to 34.9%; not exact. Native
`gof2`, 206/206 ARM units, extracted-body and adjacent sprite smokes,
and 36 verifier tests pass (one skipped). Full coverage remains
4,323/4,524; average source-shape rises 92.5547% to 92.5560%,
strict fuzzy 80.8531% to 80.8547%. The 2,378 linked-exact and 1,008
byte-exact symbol sets have no losses. Arbitrary-matrix composition,
GL output, and ARM32 runtime remain untested. Next audit the
one-matrix `DrawSpriteSystem` overload. See
[the two-matrix audit](PAINTCANVAS_SPRITE_DRAW_TWO_MATRIX_ARM_2026-10-13.md).

## PaintCanvas sprite draw camera path (2026-10-13)

The game-facing `DrawSpriteSystem(index)` now inverts the selected
camera matrix, applies the Android gravity/orientation rotation when
initialized, and calls the real `SpriteSystemDraw` instead of nine
unresolved shims. A production-body camera fixture and the adjacent
sprite smoke pass, as do native `gof2`, 206/206 ARM units, and 36
verifier tests (one skipped). Focused source-shape falls 89.5% to
74.9%, strict fuzzy 41.9% to 40.2%; the recovered behavior takes
precedence over preserving shim-shaped instructions. Full coverage
remains 4,323/4,524. Average source-shape changes 92.5578% to
92.5547%, strict fuzzy 80.8534% to 80.8531%; the 2,378 linked-exact
and 1,008 byte-exact symbol sets have no losses. Real-device GL and
non-identity camera rotation composition are untested. Next audit
the two-matrix overload used by `ParticleSystemSprite`. See
[the camera-path audit](PAINTCANVAS_SPRITE_DRAW_CAMERA_ARM_2026-10-13.md).

## World-vector screen projection (2026-10-13)

The matrix/world-vector `PaintCanvas::GetScreenPosition` now uses its
input matrix, the real camera inverse transform and gravity-based
orientation rotation, and returns the ARM-confirmed visibility result.
The two-vector game-facing overload now calls it through an identity
matrix instead of an unresolved shim and retains 100% focused
source-shape/strict fuzzy. The world-matrix overload rises 79.4% to
82.1% source-shape and 21.6% to 36.9% strict fuzzy, not exact.
Native `gof2`, an extracted-body host smoke, 206/206 ARM units, and
36 verifier tests pass (one skipped). Full coverage remains
4,323/4,524; average source-shape rises 92.5573% to 92.5578% and
strict fuzzy 80.8502% to 80.8534%. The 2,378 linked-exact and
1,008 byte-exact symbol sets have no losses. Real-device rendering
and non-identity camera rotation composition remain untested. Next
audit the remaining canvas gravity/orientation placeholders, starting
with `DrawSpriteSystem`. See
[the world-projection audit](GET_SCREEN_POSITION_WORLD_ARM_2026-10-13.md).

## Matrix screen projection (2026-10-13)

`PaintCanvas::GetScreenPosition(const Matrix &, Vector &)` now returns
the ARM-confirmed visibility flag, uses real vector assignment and
numeric casts, and removes two non-original projection guards. Its
focused source-shape rises 64.8% to 71.5%; strict fuzzy falls 30.7%
to 27.5%, so it is not exact. Native `gof2`, 206/206 ARM units, and
36 verifier tests pass (one skipped). The full corpus remains
4,323/4,524 covered and 201 missing. Average source-shape rises
92.5558% to 92.5573%, strict fuzzy falls 80.8509% to 80.8502%,
and the 2,378 linked-exact and 1,008 byte-exact symbol sets have no
losses. Real-camera/ARM runtime behavior is untested. Next audit the
other matrix/world-vector `GetScreenPosition` overload. See
[the screen-projection audit](GET_SCREEN_POSITION_MATRIX_ARM_2026-10-13.md).

## MaterialCreate overloads (2026-10-13)

Android ARM at `0x7c13c` and `0x79e4c` confirms default `Material`
construction, texture/mode assignment, resource lookup and deferred
texture creation, color-vector copy, and `ArrayAdd<Material *>`.
Both local overloads now call these real paths instead of unresolved
material-create shims. A production-body host smoke with real Material
and Array code checks ordinary creation, texture lookup/creation,
resource-cache reuse, missing texture/resource cases, and Array
growth. Native `gof2`, 206/206 ARM units, adjacent material smokes,
and 36 verifier tests pass (one skipped). Focused source-shape rises
78.9% to 100.0% (direct) and 75.8% to 96.8% (resource); strict fuzzy
rises 68.1% to 96.2% and 19.0% to 59.3%. Neither overload is exact.
Full corpus remains 4,323/4,524 functions covered, 201 missing,
2,378 linked-exact and 1,008 byte-exact, with unchanged exact symbol
sets. Average source-shape rises 92.5469% to 92.5558%; strict fuzzy
80.8364% to 80.8509%. The actual nested texture loader, nonzero
`addData` ownership, GL output, and ARM32 gameplay remain untested.
Next audit `TextureCreate`. See
[the material-create audit](MATERIAL_CREATE_ARM_2026-10-13.md).

## MeshCloneMaterial constructor/append path (2026-10-13)

Android ARM at `0x7a380` allocates a `Material`, constructs it from
the mesh's material, appends it through `ArrayAdd<Material *>`, and
writes the new index; invalid mesh indices return `0xffffffff`.
The local method's three unresolved shims are replaced by these real
paths. A production-body host smoke uses real `Material` and `Mesh`
objects to check two copies, Array growth, copied fields, and the
invalid-index branch. Native `gof2`, 206/206 ARM units, the Material
lifecycle smoke, and 36 verifier tests pass (one skipped). Focused
source-shape rises 82.2% to 96.8%, strict fuzzy 74.6% to 87.1%; the
method is not exact. Full corpus stays 4,323/4,524 original functions
covered, 201 missing, 2,378 linked-exact and 1,008 byte-exact,
with unchanged exact symbol sets. Average source-shape rises 92.5438%
to 92.5469%, strict fuzzy 80.8337% to 80.8364%. Nonzero `addData`
clone ownership and full GL/ARM32 runtime remain untested. Next audit
`PaintCanvas::MaterialCreate`'s remaining material-table shims. See
[the clone audit](MESH_CLONE_MATERIAL_ARM_2026-10-13.md).

## Material default ambient sentinel (2026-10-13)

Android ARM `Material` constructor at `0x7c188` writes −10.0f to
`ambientColor.x` (offset `0x68`). The local constructor wrote zero,
so a new material incorrectly selected `MaterialDraw`'s ambient-light
override path. It now uses the original sentinel. A host smoke using
the real `Material` and math units checks defaults, queue reuse over
two rounds, and zero-sized copy/null-source paths. Native `gof2`,
206/206 ARM units, SpriteSystemDraw and MaterialDraw smokes, and 36
verifier tests pass (one skipped). Focused constructor source-shape
rises 97.4% to 99.3%, strict fuzzy 91.4% to 93.5%; it remains
non-exact. Full corpus stays at 4,323/4,524 functions covered,
201 missing, 2,378 linked-exact and 1,008 byte-exact, with no
exact-symbol change. Average source-shape rises 92.5434% to
92.5438%; strict fuzzy 80.8333% to 80.8337%. Clone `addData`
initialization/ownership and full GL/ARM32 runtime remain unverified.
Next audit `PaintCanvas::MeshCloneMaterial`'s unresolved clone path.
See [the Material audit](MATERIAL_DEFAULT_AMBIENT_ARM_2026-10-13.md).

## SpriteSystemDraw cached-array follow-up (2026-10-13)

The five sprite material-queue appends now call the existing
`ArrayAddCached` specializations instead of a local raw-header
duplicate. `extern template` keeps the implementations in `Array.cpp`,
matching the original ARM out-of-line calls and avoiding three extra
local comparison entries. A production-body host smoke checks the
first real Array capacity growth, preservation of matrix/mesh/color
entries, both sprite size modes, and direct drawing. Native `gof2`,
206/206 ARM units, the MaterialDraw smoke, and 36 verifier tests pass
(one skipped). Focused `SpriteSystemDraw` source-shape rises 79.4% to
86.6%, strict fuzzy 11.2% to 22.7%; it remains non-exact. Full corpus
stays at 4,323/4,524 functions covered, 201 missing, 2,378
linked-exact and 1,008 byte-exact with no exact-symbol change.
Average source-shape rises 92.5419% to 92.5434%; strict fuzzy rises
80.8308% to 80.8333%. Android itself combines `new[]`, `realloc`,
and `delete[]` in this Array lifecycle, so portable allocator safety
remains unproven; the algorithm was not silently changed. Next audit
real `Material` copy/lifetime and multi-frame batching. See
[the follow-up audit](SPRITE_SYSTEM_DRAW_MATRIX_QUEUE_ARM_2026-10-13.md).

## SpriteSystemDraw matrix queues (2026-10-13)

Android ARM at `0x86ddc` calls the real `view * world` product and,
on the material path, appends the mesh, view matrix, constant flipped-Y
UV matrix, world matrix, and opaque color to five queues. The local
method had unresolved matrix shims, including a product missing its
world operand; it now uses the real product and typed cached-array
appends. A production-body host smoke checks both size modes, quad
positions, all queues, and direct drawing. Native `gof2`, 206/206 ARM
units, the MaterialDraw smoke, and 36 verifier tests pass (one
skipped). Focused source-shape rises 73.8% to 79.4%, while strict
fuzzy falls 18.3% to 11.2%; the method remains non-exact. Full corpus
stays 4,323/4,524 original functions covered, 201 missing, 2,378
linked-exact and 1,008 byte-exact, with no exact-symbol changes.
Average source-shape rises 92.5407% to 92.5419%; strict fuzzy falls
80.8323% to 80.8308%. The fixture does not test allocator lifetime,
ARM32 runtime, or GL output. Next audit cached-array growth/matrix
lifetime before claiming this batching path is runtime-safe. See
[the sprite queue audit](SPRITE_SYSTEM_DRAW_MATRIX_QUEUE_ARM_2026-10-13.md).

## MaterialDraw matrix and light routing (2026-10-13)

Android ARM at `0x870b8` confirms the batched matrix order
`arr_5c[i] * arr_2c[i]`, with the result sent to Canvas world-view,
`arr_2c[i]` sent to model, and `arr_38[i]` sent to UV. The local
method lost one product operand, sent a different model matrix, and
used the wrong ambient-light service. It now saves/restores the
engine's three light-ambient components, applies the material RGB to
light `0x4000`, and converts packed color channels through the
original double-precision division. A production-body host smoke
checks two batched meshes, call order, colors, the sentinel ambient
path, and Array reset. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). Focused source-shape rises 82.5%
to 99.4%, strict fuzzy 43.9% to 96.3%; the method is not exact.
Full corpus remains 4,323/4,524 original functions covered and 201
missing, with 2,378 linked-exact and 1,008 byte-exact and no exact
symbol-set change. Average source-shape rises 92.5371% to 92.5407%,
strict fuzzy 80.8212% to 80.8323%. The smoke mocks matrix math and
does not execute full rendering or ARM32 runtime. Next audit the
`SpriteSystemDraw` producer of these matrix queues. See
[the MaterialDraw audit](MATERIAL_DRAW_MATRIX_LIGHT_ARM_2026-10-13.md).

## PaintCanvas matrix-field layout (2026-10-12)

Android ARM separates three 16-float Canvas matrices at `0x38`,
`0x78`, and `0xb8` from the 15-float `AEMath::Matrix` constructed at
`0xf8`. The local Canvas had modeled all four as 15 floats, offset the
first two fields, and let writes to `m[15]` reach neighboring members.
The first three now have explicit 16-float storage; 32-bit assertions
confirm the four matrix and downstream resource/image offsets. The
single `MaterialDraw` consumer copies the first 15 values for its
existing `Matrix` interface; that function's wider draw semantics
remain unrecovered. Native `gof2`, 206/206 ARM units, both existing
host smokes, and 36 verifier tests pass (one skipped). Focused
`Initialize` source-shape rises 81.7% to 84.2%; `MaterialDraw` falls
85.8% to 82.5%. Full corpus stays 4,323/4,524 functions covered,
201 missing, and 1,008 byte-exact; linked-exact rises 2,376 to
2,378, with no prior exact-symbol loss. Average source-shape rises
92.5365% to 92.5371%; strict fuzzy rises 80.8140% to 80.8212%.
No real GL, original-asset, or ARM runtime test was run. The subsequent
MaterialDraw audit restores its matrix and light routing. See
[the layout audit](PAINTCANVAS_MATRIX4_LAYOUT_ARM_2026-10-12.md).

## AEMath matrix constructor and product (2026-10-12)

Android ARM at `0x6e8d8` confirms that the default `Matrix` has ones at
`m[10]` and `m[13]`; the local constructor had zeros. The constructor now
scores 100% strict fuzzy and source-shape, up from 50.0% / 78.7%, but
is not linked- or byte-exact. The Android matrix product at `0x802c8`
writes only `m[0..14]`; the local out-of-bounds `m[15]` store is gone.
Product source-shape moves 48.6% to 47.9%, strict fuzzy stays 2.2%,
and it remains non-exact. A native constructor/product smoke, native
`gof2`, 206/206 ARM units, and 36 verifier tests pass (one skipped).
The full corpus stays 4,323/4,524 original functions covered with 201
missing, 2,376 linked-exact and 1,008 byte-exact; exact symbol sets are
unchanged. Average source-shape rises 92.5321% to 92.5365%, strict
fuzzy 80.8034% to 80.8140%. This does not settle the 60/64-byte
matrix layout in every owner or prove native/ARM game runtime. The
subsequent Canvas layout audit separates its 16-float matrix fields
from the 15-float math type. See
[the matrix audit](MATRIX_CTOR_PRODUCT_ARM_2026-10-12.md).

## PaintCanvas cropped-region transform (2026-10-12)

The ten-argument `DrawRegion2D` now uses the supplied angle and pivot to
compose the Android-backed `T(x+pivot) * R(angle) * T(-pivot)` transform.
It restores the third index-buffer word (`0x20003`) and calls the real
texture, matrix, GL, and mesh-draw paths instead of unresolved per-method
shims. A production-body host smoke checks zero/nonzero rotation, UVs,
vertices, index words, and call order. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). The focused method changes
from 82.4% to 74.2% source-shape, while strict fuzzy rises 18.5% to
19.1%; replacing the two original matrix-product calls with a safe
direct affine calculation explains the source-shape loss. Full corpus
remains 4,323/4,524 functions covered and 201 missing, with 2,376
linked-exact and 1,008 byte-exact pairs and no exact-symbol loss.
Average source-shape moves 92.5338% to 92.5321%; strict fuzzy moves
80.8033% to 80.8034%. The host fixture does not prove real GL output
or ARM runtime. The subsequent matrix audit removes the product's
out-of-bounds store; other matrix-boundary questions remain.
See [the region-transform audit](PAINTCANVAS_REGION_TRANSFORM_ARM_2026-10-12.md).

## StatusWindow standing-bar geometry (2026-10-12)

Android ARM-backed row and baseline geometry now positions both standing
emblems, cropped bars, and frames after the second stats header. Both
`DrawRegion2D` calls pass zero rotation and actual destination X/Y;
image flags are restored. A production-block host smoke, native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). The very
large `StatusWindow::draw` still measures 2.6% source-shape / 1.4%
strict fuzzy and is non-exact. Full corpus stays 4,323/4,524 covered,
201 missing, 92.53% average source-shape, 80.80% strict fuzzy,
2,376 linked-exact, and 1,008 byte-exact; exact symbol sets are
unchanged. The rest of the window still has null service holders and
unrecovered UI paths; real rendering is untested. Next audit the
shared `PaintCanvas::DrawRegion2D` transform path, whose angle is
currently ignored. See
[the standing-bar audit](STATUS_STANDING_REGION_ARM_2026-10-12.md).

## Region-call ABI and PlayerEgo throttle (2026-10-12)

The mining progress bar and PlayerEgo throttle gauge now pass zero
rotation and both destination coordinates to `DrawRegion2D`, as in
Android ARM. `PlayerEgo::drawThrottle` also uses the live Canvas,
global font, and crosshair position rather than null holders, with
the recovered percentage-string and text call. A production-body
host smoke, native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). Throttle source-shape rises 79.2% to 94.0%,
strict fuzzy 31.2% to 57.1%; mining `render2D` remains 25.6% /
14.5% and neither is exact. Full corpus stays 4,323/4,524 covered,
201 missing, 2,376 linked-exact and 1,008 byte-exact, with no exact
symbol-set loss. Averages are 92.53% source-shape and 80.80% strict
fuzzy. The shared canvas region renderer still ignores the angle,
and `StatusWindow::draw` had two displaced standing-bar calls. The subsequent
[standing-bar audit](STATUS_STANDING_REGION_ARM_2026-10-12.md) corrects
those calls. Original assets and
full-game/ARM runtime remain untested. See
[the region-call audit](REGION_CALL_THROTTLE_ARM_2026-10-12.md).

## Layout border tiling and rotations (2026-10-12)

`Layout::drawBGBorder` now uses the ARM-confirmed live Canvas and restores
the original edge coordinates and rotation arguments. A host smoke executes
the production border body, including two vertical rows and partial tiles,
and covers its `drawBox` callers. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). The main body's source-shape rises from
88.6% to 94.2%; strict fuzzy falls from 13.4% to 11.1%, and it remains
non-exact. Full corpus stays 4,323/4,524 functions covered, 201 missing,
92.53% average source-shape, 80.80% strict fuzzy, 2,376 linked-exact,
and 1,008 byte-exact; exact symbol sets are unchanged. Original assets,
pixels, and ARM runtime remain untested. The shared `PaintCanvas::DrawRegion2D`
callee currently ignores the angle argument; the subsequent
[region-call audit](REGION_CALL_THROTTLE_ARM_2026-10-12.md) identifies
caller fixes needed before that callee can be changed safely.
See [the border audit](LAYOUT_BG_BORDER_ARM_2026-10-12.md).

## Layout drawBox styles and text geometry (2026-10-12)

`Layout::drawBox` now reads Android ARM-confirmed Canvas, font, and
layout metrics instead of never-set null holders. Styles 5 and 8
forward the caller's requested height to `drawBGBorder` rather than
substituting the image height. A host smoke exercises all 11 styles
with fixture resources. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). The main renderer's source-shape
rises 60.1% to 95.4% and strict fuzzy 21.4% to 37.9%; it is not
exact. Full corpus stays 4,323/4,524 original functions covered;
averages rise to 92.53% source-shape and 80.80% strict fuzzy, with
2,376 linked-exact and 1,008 byte-exact. No prior exact pair is lost.
Real fonts/assets and the called `drawBGBorder` implementation were
not tested in this snapshot; the subsequent
[border audit](LAYOUT_BG_BORDER_ARM_2026-10-12.md) executes the callee in
a fixture. See [the box audit](LAYOUT_DRAW_BOX_ARM_2026-10-12.md).

## Layout UI metrics, background, and mask (2026-10-12)

Android ARM confirms that `getFooterTransitionWidth` uses the live
Canvas, `getHelpButtonOffset` reads the global layout inset, `drawBG`
and no-argument `drawMask` use live screen dimensions, and explicit
`drawMask` uses the fixed `0x80` tint. These five paths no longer read
null holders or substitute the mutable layout color. Native `gof2`,
206/206 ARM units, the focused UI host smoke, and 36 verifier tests
pass (one skipped). Focused family source-shape rises 75.4% to 97.7%;
three functions become linked-exact. Full corpus remains 4,323/4,524
original functions covered; average source-shape rises to 92.52%,
strict fuzzy to 80.79%, linked-exact to 2,376, while byte-exact
stays 1,008. No prior exact pair is lost. Original assets, real
rendering, and ARM runtime are untested. Next audit the null
canvas/font/metric holders in `Layout::drawBox`. See
[the UI metrics audit](LAYOUT_UI_METRICS_ARM_2026-10-12.md).

## Layout footer routing and global bindings (2026-10-12)

The three `drawFooter*` wrappers now forward to one shared renderer,
as in Android ARM. That renderer uses the live Canvas, layout metrics,
status and font instead of null holders, restores the cargo warning
color and `" / "`/`"t"` load label, and draws no secondary button in
no-back mode. Native `gof2`, 206/206 ARM units, the focused UI host
smoke, and 36 verifier tests pass (one skipped). Focused source-shape
rises from 4.0% to 100% for each wrapper and from 11.3% to 93.4%
for the shared renderer; none is exact. Full corpus coverage remains
4,323/4,524 original functions; averages rise to 92.50% source-shape
and 80.75% strict fuzzy, with 2,373 linked-exact and 1,008 byte-exact.
No exact pair was lost. Real assets, original string internals, and
full-game UI runtime are not tested. Next audit the still-null canvas
holder in `Layout::getFooterTransitionWidth`. See
[the footer audit](LAYOUT_FOOTER_ARM_2026-10-12.md).

## Layout fade and full-screen fill (2026-10-12)

`Layout::drawFade` now uses the ARM-confirmed `Globals::Canvas` and
full screen dimensions in both branches instead of dereferencing
never-set null holders and filling a zero-height rectangle. The
`fadeOut` flag now follows the original alpha direction and the
original cap behavior, including the zero-duration edge case. Native
`gof2`, 206/206 ARM units, the focused host UI smoke, and 36 verifier
tests pass (one skipped). Focused source-shape rises 85.3% to 94.5%;
strict fuzzy rises 57.8% to 68.0%, without an exact match. Full corpus
remains 4,323/4,524 original functions, 92.42% average source-shape,
80.68% strict fuzzy, 2,373 linked-exact and 1,008 byte-exact, with no
lost exact pair. Actual game rendering, frame timing, and ARM runtime
remain untested. Next priority: recover the still-null canvas, font,
and metric bindings in `Layout::drawFooter*`; the 201 missing original
functions remain a separate coverage gap. See
[the fade audit](LAYOUT_DRAW_FADE_ARM_2026-10-12.md).

## TouchButton/Layout host smoke and partial pattern tile (2026-10-12)

Production touch/draw/pattern method bodies now pass a focused native host
smoke with fixture canvas and sound services. The smoke exposed a right-edge
pattern tile starting at the bottom remainder instead of the top; the
`Layout::drawBGPattern` cursor now follows the Android original. Native
`gof2`, 206/206 ARM units, and 36 verifier tests pass (one skipped).
The function's source-shape rises 94.6% to 95.7%, while strict fuzzy
falls 45.9% to 22.7%; it remains non-exact. Full corpus coverage remains
4,323/4,524 original functions; averages are 92.42% source-shape and
80.68% strict fuzzy, with 2,373 linked-exact and 1,008 byte-exact.
No exact pairs were lost. The fixture does not prove full game UI,
assets, platform services, or ARM runtime behavior. Next priority:
investigate remaining UI/runtime behavior and the 201 missing original
functions. See [the host-smoke audit](TOUCHBUTTON_UI_HOST_SMOKE_ARM_2026-10-12.md).

## TouchButton touch sound and text width (2026-10-12)

`TouchButton::OnTouchBegin` and `OnTouchEnd` now play their original
sound IDs through `Globals::sound` instead of a null holder; the
kind-10 `replaceTextKeepSize` width uses `Globals::Canvas` and the new
text argument. All three become linked-exact at 100% focused
source-shape/strict fuzzy. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). Full corpus coverage remains
4,323/4,524 original functions, with 92.42% average source-shape,
80.69% strict fuzzy, 2,373 linked-exact and 1,008 byte-exact. No
prior exact pair was lost. Live sound and touch behavior remain
untested; a focused native UI runtime smoke is the next priority.
See [the touch-sound audit](TOUCHBUTTON_TOUCH_SOUND_ARM_2026-10-12.md).

## TouchButton draw/hit and Layout pattern (2026-10-12)

`TouchButton::draw` now uses the ARM-confirmed canvas/layout, frame
mask, tint, label, overlay and progress coordinates instead of null
holders and shifted arguments. Its kind-3 touch inset now reads
`Globals::layout + 0x38`; the called `Layout::drawBGPattern` uses the
live canvas. Native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). Focused source-shape rises 85.9% to 93.3% for
`draw`, 90.2% to 90.6% for `touchedInside`, and 93.6% to 94.6% for
`drawBGPattern`. None is exact. Full corpus coverage remains
4,323/4,524 original functions, with 92.41% average source-shape,
80.68% strict fuzzy, 2,370 linked-exact and 1,008 byte-exact; no
exact pair was lost. Live UI rendering and touch handling remain
untested. See [the draw/pattern audit](TOUCHBUTTON_DRAW_LAYOUT_PATTERN_ARM_2026-10-12.md).

## TouchButton image-handle measurements (2026-10-12)

`TouchButton::init` and `setText` now measure the ARM-confirmed image
handles instead of handle zero. The two special kinds that use an
existing image now store `subId` in their frame slot. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Focused
source-shape rises 79.8% to 80.7% for `init` and 93.9% to 95.6% for
`setText`; neither is exact. Full corpus remains 4,323/4,524 original
functions covered, with 92.41% average source-shape, 80.67% strict
fuzzy, 2,370 linked-exact and 1,008 byte-exact. No exact pair was lost.
Live UI rendering is untested; `TouchButton::draw` still has null layout
holders and is the next priority. See
[the image-handle audit](TOUCHBUTTON_IMAGE_HANDLES_ARM_2026-10-12.md).

## TouchButton constructors and resource tables (2026-10-12)

The `TouchButton::init` argument contract, six constructor call mappings,
live font/canvas/layout/achievement/device globals, and frame/medal
resource-ID tables are now backed by the Android original. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Five
constructor C1/C2 pairs become linked-exact; no previous exact pair is
lost. `init` itself rises from 76.1% to 79.8% source-shape but is not
exact. Full corpus coverage remains 4,323/4,524 original functions;
average source-shape is 92.41%, strict fuzzy 80.67%, with 2,370
linked-exact and 1,008 byte-exact matches. Live UI behavior is untested;
`init`'s image-handle measurements remain a priority. See
[the TouchButton audit](TOUCHBUTTON_CTOR_TABLES_ARM_2026-10-12.md).

## Layout resource reload and window reset (2026-10-12)

`Layout::reload` now uses the ARM-confirmed canvas, device flag, text IDs,
screen dimensions, font, and typed `TouchButton` constructors instead of
null global holders and untyped constructor shims. Its called
`resetWindowDimensions` now reads `Globals` dimensions and layout inset
and writes the original back-button coordinates. Native `gof2`, 206/206
ARM units, and 36 verifier tests pass (one skipped). Focused source-shape
rises 71.8% to 90.4% for `reload` and 21.6% to 77.8% for its reset;
neither is exact. Full corpus remains 4,323/4,524 original functions
covered, with 92.39% average source-shape, 80.58% strict fuzzy, 2,360
linked-exact and 1,008 byte-exact matches. No exact pair was lost.
The called `TouchButton` constructors and `init` still have null global
bindings, so live UI reload is not yet verified; those are the next
priority. See [the Layout audit](LAYOUT_RELOAD_RESET_ARM_2026-10-12.md).

## Globals resource-release handoff (2026-10-12)

`Globals::releaseResources` now releases resources on `Globals::Canvas`
and reloads `Globals::layout`, matching the Android ARM call sequence.
The former null secondary-canvas placeholder is gone. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Focused
source-shape rises 44.2% to 100% and strict fuzzy 57.1% to 100%; the
function becomes linked-exact, not byte-exact. Full corpus remains
4,323/4,524 original functions covered, with 92.37% average source-shape,
80.57% strict fuzzy, 2,360 linked-exact and 1,008 byte-exact matches;
no previous exact pair was lost. The called `Layout::reload` still has
null global bindings, so live resource reload is not yet verified. That
function is the next priority. See
[the resource-release audit](GLOBALS_RELEASE_RESOURCES_ARM_2026-10-12.md).

## Globals leaderboard score handoff (2026-10-12)

`Globals::reportLeaderboards` now reads kills from `Globals::status` and
writes the score to the existing Android leaderboard array instead of a
null destination. Native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). Focused source-shape rises 80.7% to 100% and strict
fuzzy 69.6% to 100%; the function becomes linked-exact, not byte-exact.
Full corpus remains 4,323/4,524 original functions covered, with 92.36%
average source-shape, 80.56% strict fuzzy, 2,359 linked-exact and 1,008
byte-exact matches. No prior exact pair was lost. Live leaderboard handoff
remains untested. Next priority: `Globals::releaseResources`, which still
uses a null secondary-canvas placeholder instead of reloading layout.
See [the leaderboard audit](GLOBALS_LEADERBOARDS_ARM_2026-10-12.md).

## Globals item name lookup (2026-10-12)

`Globals::getItemName` now reads through the real `Globals::gameText`
pointer and uses the ARM-confirmed `String` copy constructor with `false`,
instead of dereferencing a null placeholder. Native `gof2`, 206/206 ARM
units, and 36 verifier tests pass (one skipped). Focused source-shape rises
82.0% to 100% and strict fuzzy 66.7% to 100%; the function becomes
linked-exact, not byte-exact. Full corpus remains 4,323/4,524 original
functions covered, with 92.36% average source-shape, 80.55% strict fuzzy,
2,358 linked-exact and 1,008 byte-exact matches. No prior exact pair was
lost. Live text lookup remains untested. Next priority:
`Globals::reportLeaderboards`, whose output still targets a null placeholder.
See [the item name audit](GLOBALS_ITEM_NAME_ARM_2026-10-12.md).

## Globals wreck collision shapes (2026-10-12)

`Globals::getWreckCollision` now reconstructs sphere and AAB records with
the Android ARM-confirmed coordinate order, scaling factors, constructor
arguments, and `kind == 0` doubling rule. The null guard dereference and
untyped geometry constructor are gone; the input array uses the original
release path. Native `gof2`, 206/206 ARM units, and 36 verifier tests pass
(one skipped). Focused source-shape rises 90.8% to 97.2% and strict fuzzy
41.6% to 56.7%; this function is not exact. Full corpus covers
4,323/4,524 original functions at 92.35% average source-shape and 80.55%
strict fuzzy, with 2,357 linked-exact and 1,008 byte-exact matches and no
prior exact-pair losses. Live wreck collision behavior remains untested.
See [the wreck collision audit](GLOBALS_WRECK_COLLISION_ARM_2026-10-12.md).

## Globals createBillBoard geometry and material (2026-10-12)

`Globals::createBillBoard` now uses the live canvas, the ARM-confirmed
material resource ID, UV corners, and geometric extents instead of null
placeholders and miswired arguments. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). Focused source-shape and
strict fuzzy both reach 100%, adding one linked-exact match. Full corpus
remains 4,323/4,524 original functions covered and 92.35% average
source-shape; strict fuzzy rises 80.52% to 80.54%, linked-exact 2,355
to 2,356, with 1,008 byte-exact and no exact-pair losses. Live
starfield rendering remains untested. See
[the billboard audit](GLOBALS_CREATE_BILLBOARD_ARM_2026-10-12.md).

## Globals random enemy fighter source (2026-10-12)

`Globals::getRandomEnemyFighter` now uses live `Globals::rnd` and
`Globals::status` plus the ARM-confirmed 37-entry race table instead of
null RNG pointers and a one-element placeholder. Native `gof2`, 206/206
ARM units, and 36 verifier tests pass (one skipped). Focused source-shape
rose 86.5% to 98.9% and strict fuzzy 72.7% to 95.8%; the residual
comparison is semantically equivalent. Full corpus remains 4,323/4,524
original functions covered, 92.35% source-shape, 80.52% strict fuzzy,
2,355 linked-exact and 1,008 byte-exact, with no exact-pair losses.
Live enemy spawning is untested.
See [the fighter-selection audit](GLOBALS_RANDOM_ENEMY_FIGHTER_ARM_2026-10-12.md).

## Globals drawLines canvas and row advance (2026-10-12)

Both drawing overloads now use live `Globals::Canvas` and the ARM-confirmed
row advance from `Globals::layout` at offset `+4`, replacing null
placeholders. Native `gof2`, 206/206 ARM units, and 36 verifier tests pass
(one skipped). Both overloads reached 100% focused source-shape and became
linked-exact. Full corpus remains 4,323/4,524 original functions covered;
average source-shape rose 92.33% to 92.35%, strict fuzzy 80.48% to
80.52%, and linked-exact 2,353 to 2,355, with 1,008 byte-exact and no
exact-pair losses. Live rendering remains untested. See
[the draw-lines audit](GLOBALS_DRAW_LINES_ARM_2026-10-12.md).

## Globals getBoundedString repair (2026-10-12)

`Globals::getBoundedString` now measures through the live canvas and font,
appends the ARM-confirmed `"..."` after the shortened line, and destroys
its temporary heap string. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). Focused source-shape rose 82.2% to
100.0% and strict fuzzy 37.8% to 91.8%; no exact match was gained.
Full corpus remains 4,323/4,524 original functions covered and 92.33%
source-shape; strict fuzzy rises 80.47% to 80.48%, with 2,353
linked-exact and 1,008 byte-exact preserved. Live text-width/UI behavior
remains untested. See
[the bounded-string audit](GLOBALS_BOUNDED_STRING_ARM_2026-10-12.md).

## Globals getLineArray trailing newline (2026-10-11)

`Globals::getLineArray` now appends the ARM-confirmed newline sentinel to
its working text; the recovered literal had been empty. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Its focused
source-shape remains 82.7% and strict fuzzy 42.6%; those metrics do not
measure literal correctness. Full corpus stays at 4,323/4,524 original
functions covered, 92.33% source-shape, 80.47% strict fuzzy, 2,353
linked-exact and 1,008 byte-exact, with no exact-pair losses. Live text
layout remains untested. See
[the newline audit](GLOBALS_GETLINEARRAY_NEWLINE_ARM_2026-10-11.md).

## Globals getLine canvas and line breaks (2026-10-11)

`Globals::getLine` now measures glyph width through live `Globals::Canvas`
instead of a null placeholder and ends a line on LF/CR even before the
width limit, as the Android ARM branches do. On width overflow it retains
the original last-space fallback. Native `gof2`, 206/206 ARM units, and
36 verifier tests pass (one skipped). Focused source-shape improves from
84.1% to 87.7%, strict fuzzy from 34.8% to 36.3%. Full corpus remains
4,713 comparisons of 4,323/4,524 original functions, 92.33%
source-shape, 80.47% strict fuzzy, 2,353 linked-exact and 1,008
byte-exact, without exact-pair losses. Live wrapping/rendering is not
yet verified. See [the text-line audit](GLOBALS_GETLINE_CANVAS_BREAKS_ARM_2026-10-11.md).

## Random planet name RNG wiring (2026-10-11)

`Globals::getRandomPlanetName` now uses initialized `Globals::rnd` for
`nextInt(100)` instead of dereferencing a never-set null placeholder.
Android ARM confirms that data source. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). The function reaches 100%
source-shape from 94.2%, while strict fuzzy rises 76.5% to 86.1%.
Full corpus: 4,713 comparisons of 4,323/4,524 original functions,
92.33% source-shape, 80.47% strict fuzzy, 2,353 linked-exact and
1,008 byte-exact, with no lost exact pair. A live generated-name run
remains untested. See [the RNG audit](GLOBALS_RANDOM_PLANET_RNG_ARM_2026-10-11.md).

## Dialogue sound pair table recovery (2026-10-11)

`Globals::getDialogueSoundId` now searches the full 1,504-pair Android
mapping instead of reading far beyond a one-element placeholder. The
12,032-byte table extracted from Android `.rodata` matches the compiled
ARM object's table byte for byte by SHA-256, and a regression test guards
its count, key uniqueness, and contents. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). The function improves from
96.1% to 97.2% source-shape. Full corpus remains 4,713 comparisons of
4,323/4,524 original functions, 92.32% source-shape, 80.47% strict fuzzy,
2,353 linked-exact and 1,008 byte-exact, with zero exact-pair losses.
Live dialogue audio remains untested. See
[the pair-table audit](GLOBALS_DIALOGUE_SOUND_PAIRS_ARM_2026-10-11.md).

## Globals music selection recovery (2026-10-11)

`playMusicAndFadeOutCurrent` now uses the live `Globals::sound`, calls
`Station::getIndex()` without truncating station pointers, stops the current
music event, and selects race tracks from the two four-entry Android ARM
tables. The prior null sound placeholders and zero-sized effective tables
could not run correctly. Native `gof2`, 206/206 ARM units, and 35 verifier
tests pass (one skipped). The function improves from 86.9% to 93.1%
source-shape and 55.9% to 70.4% strict fuzzy. Full corpus: 4,713
comparisons of 4,323/4,524 original functions, 92.32% source-shape,
80.47% strict fuzzy, 2,353 linked-exact and 1,008 byte-exact, with
zero exact-pair losses. Live FMOD playback remains untested. See
[the music recovery audit](GLOBALS_MUSIC_SELECTION_ARM_2026-10-11.md).

## Native Globals canvas pointer width (2026-10-11)

`Globals::init` now copies `ApplicationManager::paintCanvas` into
`Globals::Canvas` at native pointer width when the global is empty. The
former `int *` write retained only the low 32 bits on 64-bit Windows;
Android ARM reads the same first application-manager field. Native `gof2`,
206/206 ARM units, and 35 verifier tests pass (one skipped). Full ARM
corpus remains 4,713 comparisons of 4,323/4,524 original functions,
92.32% source-shape, 80.46% strict fuzzy, 2,353 linked-exact and 1,008
byte-exact, with no changed function shape or lost exact pair. Runtime
canvas rendering is still untested because the full executable does not
link. See [the canvas ABI audit](GLOBALS_CANVAS_NATIVE_POINTER_ARM_2026-10-11.md).

## TouchSlider layout-backed touch padding (2026-10-11)

The slider constructor no longer reads its touch padding from a never-set
null application placeholder. Android ARM loads `Globals::layout` and its
field at `+0x7c`; native now does the same. Native `gof2`, 206/206 ARM units,
and 35 verifier tests pass (one skipped). Full corpus remains 4,713
comparisons of 4,323/4,524 original functions, 92.32% source-shape,
80.46% strict fuzzy, 2,353 linked-exact and 1,008 byte-exact, with no
lost exact pair. The constructor's focused source-shape drops 86.3% to
85.1% while strict fuzzy rises 68.7% to 72.5%; the corrected data source
takes precedence over that local score. Live touch behavior is untested.
See [the slider audit](TOUCH_SLIDER_LAYOUT_ARM_2026-10-11.md).

## ScrollTouchWindow live layout/canvas wiring (2026-10-11)

The draw paths and both constructors no longer dereference six null local
placeholders. They use the live `Globals::layout` and `Globals::Canvas`
identified in the Android ELF. The 16-function family rises from 71.96% to
96.51% source-shape; six affected functions improve, including `drawTextBG`
from 0.3% to 81.3%. Native `gof2`, 206/206 ARM units, and 35 verifier tests
pass (one skipped). Full corpus: 4,713 comparisons of 4,323/4,524 original
functions, 92.32% source-shape, 80.46% strict fuzzy, 2,353 linked-exact and
1,008 byte-exact, with no lost exact pair, skipped unit, or wrong-type
symbol. Visual runtime behavior remains untested because the full native
executable still does not link. See
[the global wiring audit](SCROLL_TOUCH_WINDOW_GLOBALS_ARM_2026-10-11.md).

## GameRecord native pointer runtime smoke (2026-10-11)

A focused native test constructs the real `GameRecord`, assigns 29 distinct
64-bit pointer markers through its typed setters, and verifies all getters
and four adjacent scalar fields. It passes; the temporary executable was
removed. Normal native `gof2`, 206/206 ARM units, and 35 verifier tests pass
(one skipped). Full corpus is unchanged: 4,713 comparisons of 4,323/4,524
original functions, 92.24% source-shape, 80.43% strict fuzzy, 2,353
linked-exact and 1,008 byte-exact, with no lost exact pair. This smoke uses
minimal string stubs and omits `GameRecord::load` only in its test build;
it does not prove a live save/reload or full executable link. See
[the pointer runtime smoke report](GAME_RECORD_POINTER_NATIVE_RUNTIME_SMOKE_2026-10-11.md).

## Native executable-link duplicate audit (2026-10-11)

A focused `RecordHandler::recordStoreRead` link probe exposed seven
duplicate native definitions: six `Vector`/`Matrix` operators and
`ApplicationManager::gAppManager`. Native builds now use the existing
`AEMath.cpp` operator definitions and `ApplicationManager.cpp` global,
while the 32-bit ARM units retain their prior definitions for corpus
parity. The duplicate linker count fell from seven to zero; a small
native math link/runtime smoke passes. A full record executable still
does not link: thousands of unresolved call sites remain, including
missing game helpers and FMOD binaries that are not in the repository.
Native `gof2`, 206/206 ARM units, and 35 verifier tests pass (one
skipped). Full corpus remains 4,713 comparisons of 4,323/4,524
original functions, 92.24% source-shape, 80.43% strict fuzzy, 2,353
linked-exact and 1,008 byte-exact, with no lost exact pair, skipped
unit, or wrong-type symbol. A live save/reload remains untested. See
[the native link audit](NATIVE_LINK_DUPLICATE_DEFINITIONS_2026-10-11.md).

## Record core pointers and save writer (2026-10-11)

Eight remaining pointer-valued `GameRecord` slots in the mission,
medal, and status-array block (`0x54`–`0x8c`) now retain full-width
native pointers while preserving ARM offsets. The writer also serializes
the actual reroute counter at `Status + 0x88` and reads the previous
ship at `+ 0x8c`, as the Android ARM source does, instead of treating
unrelated array pointers as those values. Native `gof2`, 206/206 ARM
units, and 35 verifier tests pass (one skipped). Full corpus remains
4,713 comparisons of 4,323/4,524 original functions, 92.24%
source-shape, 80.43% strict fuzzy, 2,353 linked-exact, 1,008
byte-exact, with zero lost exact pairs, skipped units, or wrong-type
symbols. A live save/reload and full executable link remain untested;
these are the next priority. See [the ARM record writer audit](RECORD_CORE_POINTERS_SAVE_WRITER_ARM_2026-10-11.md).

## Previous-ship record and Status offset repair (2026-10-11)

Android ARM distinguishes `Status + 0x84` (menu preset), `+ 0x88`
(reroute counter), and `+ 0x8c` (previous ship pointer). The native
source had accidentally aliased the preset and counter to `+ 0x8c`.
Those uses now match ARM, and `GameRecord + 0x134` transfers the ship
pointer at full native width to `Status`. Native `gof2`, 206/206 ARM
units, and 35 verifier tests pass (one skipped). Full corpus remains
4,713 comparisons of 4,323/4,524 original functions, 92.24%
source-shape, 80.43% strict fuzzy, 2,353 linked-exact and 1,008
byte-exact, with no lost exact pair, skipped unit, or wrong-type
symbol. `Status::departStation` source-shape rises 83.8% to 84.0%; both
affected menu starts rise 0.1 point. A live save/reload and new-game
runtime test remain unverified. See [the ARM offset audit](GAME_RECORD_PREVIOUS_SHIP_STATUS_OFFSETS_ARM_2026-10-11.md).

## GameRecord station-stack and wanted pointers (2026-10-11)

Android ARM record reading identifies `GameRecord + 0x5c` as an
`Array<Station *> *` and extended-record slot `+ 0x1b4` as an
`Array<Wanted *> *`. Both now keep full-width pointers on native 64-bit
builds while retaining their ARM offsets. The wanted-object copy into
`Status` also uses full-width elements. Native `gof2`, 206/206 ARM
units, and 35 verifier tests pass (one skipped). Full corpus:
4,713 comparisons of 4,323/4,524 original functions, 92.24%
source-shape, 80.43% strict fuzzy, 2,353 linked-exact, 1,008
byte-exact; no lost exact pair, skipped unit, or wrong-type symbol.
Affected function shapes are unchanged. A live save/reload round trip
and destination wanted-array capacity remain unverified. The ship slot
`GameRecord + 0x134` needs separate analysis because its apparent
destination `Status + 0x8c` is also used as a counter. See
[the ARM pointer audit](GAME_RECORD_STATION_STACK_WANTED_ABI_ARM_2026-10-11.md).

## GameRecord array pointers 0x160–0x184 (2026-10-11)

The ten adjacent record slots for visibility, four integer arrays,
two further boolean arrays, and item/ship arrays are now typed pointers
at their original ARM offsets. Native 64-bit builds retain full-width
values in aligned sidecars instead of writing pointers across adjacent
32-bit slots. The save-file read order remains unchanged. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Full corpus:
4,713 comparisons of 4,323/4,524 original functions, 92.24%
source-shape, 80.43% strict fuzzy, 2,353 linked-exact, 1,008
byte-exact; no lost exact pair, skipped unit, or wrong-type symbol.
`recordStoreRead` source-shape rises 91.4% to 91.5%; other changed
functions retain their prior shapes. A live save/reload round trip and
full executable link remain untested. Remaining raw pointer slots such
as `GameRecord + 0x5c`, `+ 0x134`, and `+ 0x1b4` are next to audit. See
[the record array ABI audit](GAME_RECORD_ARRAY_POINTERS_ARM_2026-10-11.md).

## Record wingmen native ABI (2026-10-11)

Android ARM confirms `GameRecord + 0x14c` and `+ 0x158` as the wingmen
array and five-integer stats pointers transferred into `Status + 0x24`
and `+ 0x28`. Both objects now retain full-width native pointers, and
`Level` uses the typed array rather than 32-bit pointer arithmetic.
`Globals::init` allocates at least the native `sizeof(Status)` and calls
its defined constructor instead of the declared-only `Status_ctor`.
Native `gof2`, 206/206 ARM units, and 36 verifier tests pass (one
skipped). Full corpus: 4,713 comparisons of 4,323/4,524 original
functions, 92.24% source-shape, 80.43% strict fuzzy, 2,353
linked-exact, 1,008 byte-exact, no lost exact pair, skipped unit, or
wrong-type symbol. `Status::getWingmen` remains byte-exact; the
non-exact `setWingmen` and `Level::createWingmen` source-shapes decline
slightly with typed access. A live save/reload round trip and full
executable link remain untested; audit `GameRecord` slots `0x160+`
next. See [the ARM wingmen ABI audit](RECORD_WINGMEN_NATIVE_ABI_ARM_2026-10-11.md).

## GameRecord pending-product and agent pointers (2026-10-11)

Android ARM confirms `GameRecord` slots `0x144` and `0x148` as arrays
of `PendingProduct *` and `Agent *`. Native 64-bit builds now keep
their pointers in aligned sidecars, and loading copies agent pointers
at full width rather than through `uint32_t`. Native `gof2`, 206/206
ARM units, and 36 verifier tests pass (one skipped). Focused shapes
remain stable. Full corpus: 4,713 comparisons of 4,323/4,524 original
functions, 92.24% source-shape, 80.43% strict fuzzy, 2,353
linked-exact, 1,008 byte-exact, no lost exact pair, skipped unit, or
wrong-type symbol. Runtime save/reload remains untested; audit the
wingmen and later pointer slots with their `Status` consumers next.
See [the ARM product/agent audit](GAME_RECORD_PRODUCT_AGENT_POINTERS_ARM_2026-10-11.md).

## GameRecord standing and blueprint pointers (2026-10-11)

ARM slots `0x13c` and `0x140` are `Standing *` and
`Array<BluePrint *> *`. Reader, constructor, and load now use typed
pointers; native 64-bit builds store them in aligned sidecars instead
of truncating them through 32-bit slots. Blueprint transfer into
`Status` also copies full-width pointer elements. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Full
corpus: 4,713 comparisons of 4,323/4,524 original functions,
92.24% source-shape, 80.43% strict fuzzy, 2,353 linked-exact, 1,008
byte-exact, no lost exact pair, skipped unit, or wrong-type symbol.
Two extra blueprint-array comparisons add one linked-exact pair but
do not increase unique function coverage. Save/reload remains untested;
pointer-bearing slots from `0x144` onward need an ABI audit next. See
[the ARM standing/blueprint audit](GAME_RECORD_STANDING_BLUEPRINT_POINTERS_ARM_2026-10-11.md).

## Record preview station and ship pointers (2026-10-11)

Android ARM identifies `GameRecord + 0x138` as the station pointer and
`+ 0x130` as the ship pointer used by preview writing. Both are now typed
on ARM and stored separately on 64-bit native builds; all known reader,
writer, and load uses are updated. The live-status preview writer also
calls the real station/system getters rather than treating a local string
as those objects. Focused preview-writer source-shape rises from 85.0%
to 88.2% (live status) and 85.9% to 86.3% (loaded record); strict fuzzy
rises from 31.7% to 45.6% and 60.1% to 61.1%, respectively. Native
`gof2`, 206/206 ARM units, and 36 verifier tests pass (one skipped).
Full corpus remains 4,323/4,524 original functions, 92.24% average
source-shape, 80.43% strict fuzzy, 2,352 linked-exact, 1,008
byte-exact, with no lost exact pair, skipped unit, or wrong-type symbol.
Other raw 32-bit pointer slots in `GameRecord` still make native
save/reload unproven. See [the ARM preview-pointer audit](RECORD_PREVIEW_POINTERS_ARM_2026-10-11.md).

## GameRecord preview string lifetime (2026-10-11)

Android ARM confirms two 12-byte `String` members at `0x188` (system)
and `0x194` (station), constructed and destroyed by `GameRecord`.
Replacing raw integer slots restores both destructor aliases to
linked-exact and raises constructor source-shape from 58.8% to 92.2%.
The 64-bit native preview strings are placed separately and checked for
alignment, rather than overlapping nearby raw fields. Native `gof2`,
206/206 ARM units, and 36 verifier tests pass (one skipped). Full corpus:
4,323/4,524 original functions, 92.24% average source-shape, 80.43%
strict fuzzy, 2,352 linked-exact, 1,008 byte-exact, no lost exact
pair, skipped unit, or wrong-type symbol. A native save/reload round trip
is not yet proven: `GameRecord::load` retains raw 32-bit assumptions and
prevents a focused native executable link. Next audit the preview station
lookup and close the runtime test path. See
[the ARM preview-string audit](GAME_RECORD_PREVIEW_STRINGS_ARM_2026-10-11.md).

## Record file I/O wiring (2026-10-11)

The declared-only `AEFile_Read*`/`Write*` calls in `RecordHandler` now
forward to existing `AEFile::Read`/`Write` methods. Native `gof2`, 206/206
ARM translation units, and 36 verifier tests pass (one skipped); its object
has no remaining references to the invented file-I/O helper names. Full
corpus: 4,323/4,524 original functions compared, 92.18% average
source-shape, 80.37% strict fuzzy, 2,350 linked-exact, 1,008 byte-exact,
no lost exact pair, skipped unit, or wrong-type symbol. A final executable
link and a save/reload round trip remain untested. The preview record's
`String` fields are still raw 32-bit slots in `GameRecord`, unsafe to assume
correct on 64-bit native builds. Next recover those field types and test
serialization. See [the ARM file-I/O audit](RECORD_FILE_IO_WIRING_ARM_2026-10-11.md).

## Record reader station, standing, and string calls (2026-10-11)

Android ARM resolves four declared-only reader helpers to existing
`Galaxy::getStation`, `Standing` construction/setter, and `String`
copy construction. The reader now calls those methods directly; the
native object no longer references the four invented symbols, and
the corresponding definitions exist in `libgof2.a`. Focused
`recordStoreRead` source-shape/strict fuzzy improves from
91.6%/16.9% to 91.8%/23.3%. Native `gof2`, 206/206 ARM units, and
36 verifier tests pass (one skipped). Full corpus remains 4,323/4,524
at 92.18% source-shape, improves to 80.38% strict fuzzy, and retains
2,350 linked-exact and 1,008 byte-exact with no lost exact pair,
skipped unit, or wrong-type symbol. Full executable linking and a
save/reload round trip are untested; `RecordHandler` still references
declared-only `AEFile_Read*`/`Write*` wrappers. Audit those file-I/O
calls next. See
[the ARM helper audit](RECORD_READER_HELPER_CALLS_ARM_2026-10-11.md).

## Record reader item and ship definitions (2026-10-11)

Android ARM confirms that `recordStoreRead` obtains ship and item
prototypes from `Globals::ships` and `Globals::items`. All ten reader
lookups now use these live arrays rather than null local aliases;
ship pointers also no longer pass through a narrowing 32-bit integer
on native builds. Its source-shape improves from 26.4% to 91.6%,
strict fuzzy from 14.1% to 16.9%. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). Full corpus: 4,323/4,524
original functions, 92.18% source-shape, 80.37% strict fuzzy,
2,350 linked-exact, 1,008 byte-exact, with no lost exact pair,
skipped unit, or wrong-type symbol. Fourteen newly emitted array
template symbols account for seven additional linked-exact
comparisons; no older pair newly became exact. A live save/reload
round trip remains untested. Next replace the four unresolved reader
helper calls and test a final native link. See
[the ARM reader audit](RECORD_READER_DEFINITION_TABLES_ARM_2026-10-11.md).

## Record hint and extended-format wiring (2026-10-11)

Android ARM/ELF confirms that the saved flag block is `Globals::hints`,
the option flags are `Globals::options`, the empty mission comes from
`Mission::empty`, and the extended save tag is `0x6e6a78`. Both writer
and reader now use that tag, and `GameRecord::load` maps the gas-cloud
hint `0x2d` to the same live array used by the cloud. The former
absolute pointer slots and null writer aliases are removed for these
paths. Focused source-shape improves from 96.9% to 97.6% for
`GameRecord::load` and 62.0% to 90.9% for `recordStoreWrite`;
`recordStoreRead` remains 26.4%. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). The full corpus remains
4,323/4,524 at 92.14%/80.38% source-shape/strict fuzzy, with
2,343 linked-exact, 1,008 byte-exact, and no lost exact pair,
skipped unit, or wrong-type symbol. A live save/reload round trip is
not verified: `recordStoreRead` still has null item/ship-definition
aliases. Audit those reader dependencies next. See
[the ARM record audit](RECORD_HINT_AND_VERSION_WIRING_ARM_2026-10-11.md).

## PlayerGasCloud particle update and cargo pickup (2026-10-11)

Android ARM confirms the live Canvas, sound, item table, and
`Globals::hints[0x2d]` dependencies. The update path now advances
transforms by the real `dt`, uses the original movement and fade
constants, creates a one-unit cargo item rather than adding its
prototype, and steers toward the player using a normalized direction
and equipment attribute `49`. Its source-shape/strict fuzzy rises
from 80.7%/6.8% to 93.4%/42.0%. Native `gof2`, 206/206 ARM units,
and 36 verifier tests pass (one skipped). The full corpus is
4,323/4,524 at 92.14% source-shape, 80.38% strict fuzzy, 2,343
linked-exact, and 1,008 byte-exact, with no lost exact pair, skipped
unit, or wrong-type symbol. Runtime pickup and save/reload behavior
are not tested. Next audit the `GameRecord` hint save/load block,
which still uses an absolute pointer overlay in native source. See
[the ARM update audit](PLAYER_GAS_CLOUD_UPDATE_ARM_2026-10-11.md).

## PlayerGasCloud explosion, construction, and render (2026-10-11)

Android ARM confirms the shared Canvas, item table, and RNG dependencies;
explosion deactivates the player and uses bounded `nextInt`, item
attribute `56`, and the `130`, `100`, `5000`, and `200` scale factors.
The null aliases and zero divisors in these three paths are removed.
The four targeted comparisons (`explode`, two constructor aliases,
`render`) rise from 80.7%/60.25% to 94.1%/70.9% average source-shape/
strict fuzzy. Native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). Full corpus: 4,323/4,524 original functions,
92.13% source-shape, 80.37% strict fuzzy, 2,343 linked-exact,
1,008 byte-exact, zero skipped and wrong-type symbols. No previously
exact pair is lost; the five additional linked-exact comparisons are
newly emitted `ArrayAdd` template symbols, not improved old pairs.
`PlayerGasCloud::update` still uses null placeholders and zero constants;
live-game effects are untested. This is the next priority. See
[the ARM gas-cloud audit](PLAYER_GAS_CLOUD_EXPLOSION_ARM_2026-10-11.md).

## PlayerJunk death, loot, and effect (2026-10-11)

Android ARM confirms that reset reactivates the player, death sound and
loot RNG use shared `Globals`, the crate contains item `99` in quantity
`1–10`, and the death particle effect uses lifetime `-1.0`. The cargo
flag is set again after crate creation. These paths no longer use null
sound/RNG aliases. Native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). The `update` source-shape/strict fuzzy improves from
88.1%/70.4% to 93.6%/72.9%; `reset` retains its linked-exact score
despite the corrected call target. The 12-comparison focused family
averages 99.5% source-shape and 94.3% strict fuzzy, with 10 linked-exact
and 2 byte-exact. The full corpus remains 4,323/4,524
at 92.11%/80.34%, 2,338 linked-exact, and 1,008 byte-exact, with no
lost exact pair, skipped unit, or wrong-type symbol. Running-game loot
and effects remain untested. Next inspect `PlayerGasCloud`'s null global
aliases and zero constants against ARM. See
[the ARM junk audit](PLAYER_JUNK_DEATH_AND_LOOT_ARM_2026-10-11.md).

## PlayerAsteroid motion and blast force (2026-10-11)

Android ARM confirms push duration `5000`, geometry-matrix-first
rotation, and a blast impulse using scale clamped to `0.6–1.0` and
multiplier `1024`. The recovered impulse had usually rounded to zero.
It now stores the scaled vector in inherited `targetPos` and calls
the asteroid's virtual translation rather than moving a base route.
The projection method also updates the original class-static `pos`.
Native `gof2`, 206/206 ARM units, and 36 verifier tests pass (one
skipped). The four-function focused source-shape/strict fuzzy improve
from 85.6%/48.9% to 86.8%/55.0%, without new exact matches. The full
corpus remains 4,323/4,524 at 92.11% source-shape, 80.34% strict fuzzy,
2,338 linked-exact, and 1,008 byte-exact, with no lost exact pair,
skipped unit, or wrong-type symbol. Running-game physics is untested;
next inspect `PlayerJunk`'s null RNG/sound dependencies. See
[the ARM motion audit](PLAYER_ASTEROID_MOTION_ARM_2026-10-11.md).

## PlayerAsteroid ABI and global dependencies (2026-10-11)

The Android ARM field layout now includes the missing words at `0x130`
and `0x154`, with assertions through class size `0x170`. Constructor
and update paths use the real Canvas, RNG, Level, and Status objects,
restore the ARM radius, spin, hit-flash, and force constants, and
remove an undefined transform wrapper. Native `gof2`, 206/206 ARM
units, and 36 verifier tests pass (one skipped). The 26-function
family improves from 89.8%/70.1% to 93.1%/80.7% mean source-shape/
strict fuzzy; exact counts rise from 8/3 to 13/8 linked/byte. The full
corpus remains 4,323/4,524 compared but rises to 92.11% source-shape,
80.33% strict fuzzy, 2,338 linked-exact, and 1,008 byte-exact, with
no lost exact pair, skipped unit, or wrong-type symbol. Running-game
loot, collision, and explosion behavior remain untested; next inspect
the remaining asteroid projection/force paths. See
[the ARM asteroid ABI audit](PLAYER_ASTEROID_ARM_ABI_AND_GLOBALS_2026-10-11.md).

## PlayerCreature ABI, stats, and runtime wiring (2026-10-11)

The Android ARM `PlayerCreature` layout now places its derived fields
and byte flags at their original offsets, using inherited geometry
and type instead of duplicated fields. One ELF-backed `{1, 1, 1, 0}`
table replaces three zero tables. Constructor arguments, reset
reactivation, RNG/sound globals, and the death-effect call were also
restored. Native `gof2`, 206/206 ARM units, and 36 verifier tests pass
(one skipped). The 20-function family improves from 85.5%/56.7% to
96.1%/90.6% source-shape/strict fuzzy; exact counts rise from 5/2 to
13/8 linked/byte. The full corpus remains 4,323/4,524 compared, but
improves to 92.09% source-shape, 80.27% strict fuzzy, 2,333 linked-exact,
and 1,003 byte-exact, with no lost exact pair, skipped unit, or wrong-type
symbol. Live creature behavior remains untested; next inspect the
`PlayerAsteroid` global dependencies. See
[the ARM creature audit](PLAYER_CREATURE_ARM_LAYOUT_AND_STATS_2026-10-11.md).

## RecordHandler save/options hash inputs (2026-10-11)

The three SHA-256 paths now use the Android ARM 25-byte salt and 16
bytes from the JNI-owned key, replacing zero salts and null key
aliases. The original ELF bytes and compiled ARM relocations confirm
the inputs. Native `gof2`, 206/206 ARM units, and 36 verifier tests
pass (one skipped). Focused source-shape/strict fuzzy rise from
81.0%/42.5% to 82.3%/50.0%; no function becomes exact. The full
corpus remains 4,323/4,524 compared at 92.05% source-shape, with
strict fuzzy improving from 80.12% to 80.13%. It retains 2,325
linked-exact and 997 byte-exact, with no lost exact pair, skipped unit,
or wrong-type symbol. Android save/load round-trip and native-only
key provision remain unverified; see
[the ARM hash-input audit](RECORD_HANDLER_HASH_ARM_2026-10-11.md).

## Asteroid quality letters (2026-10-11)

`PlayerAsteroid::getQualityString` now uses the Android ARM grades C/B/A
for qualities 5/6/7, D for quality 4, and E otherwise, replacing empty
placeholders. The ELF's pointer table and string bytes confirm the mapping.
Native `gof2`, 206/206 ARM units, and 36 verifier tests pass (one skipped).
Focused source-shape/strict fuzzy improve from 32.9%/9.9% to
35.2%/18.4%, without an exact match. The full corpus remains
4,323/4,524 compared at 92.05% source-shape and 80.12% strict fuzzy,
with 2,325 linked-exact and 997 byte-exact; no exact-pair loss,
skipped unit, or wrong-type symbol. Game-level display remains untested;
see [the ARM quality-string audit](PLAYER_ASTEROID_QUALITY_STRINGS_ARM_2026-10-11.md).

## Station blueprint and pirate-base tables (2026-10-11)

`Station.cpp` now uses the Android ARM station ID tables: pirate bases
`1, 33, 47, 86` and hidden blueprints `132, 133, 134, 129, 123`,
replacing two zero-filled arrays. The bytes match the original ELF and
generated ARM object, and agree with existing `SolarSystem` tables.
Native `gof2`, 206/206 ARM units and 36 verifier tests pass (one skipped).
The four-function focused family remains 96.3% source-shape / 97.3%
strict fuzzy, with two linked-exact index methods and no byte-exact.
The full corpus remains 4,323/4,524 compared at 92.05% source-shape
and 80.12% strict fuzzy, with 2,325 linked-exact and 997 byte-exact;
no exact-pair loss, skipped row or wrong-type symbol. End-to-end
campaign/discovery flags remain unverified; see
[the ARM station table audit](STATION_BLUEPRINT_PIRATE_TABLES_ARM_2026-10-11.md).

## Agent mod-price table (2026-10-11)

The four ARM rodata percentages used by `Agent::getModPricePercentage`
are **20, 30, 40, 20**, not the four zeros in the recovered source.
They now match both the original ELF and generated ARM object's bytes;
the fallback 40 is unchanged. Native `gof2`, 206/206 ARM units and 36
verifier tests pass (one skipped). This function remains 100%
source-shape/strict fuzzy and linked-exact, demonstrating that
instruction matching alone does not validate referenced data. The
full corpus remains 4,323/4,524 compared at 92.05% source-shape and
80.12% strict fuzzy, with 2,325 linked-exact and 997 byte-exact; no
exact-pair loss, skipped row or wrong-type symbol. Live agent offers
remain unverified; see [the ARM table audit](AGENT_MOD_PRICE_TABLE_ARM_2026-10-11.md).

## Collision and part resource paths (2026-10-11)

Android ARM confirms five `data/bin` resources for wreck, station and
static collision plus station and ship parts. Ten stale path literals
were corrected without changing record parsing. Native `gof2`, 206/206
ARM units and 36 verifier tests pass (one skipped). The focused family
remains 86.0% source-shape / 45.6% strict fuzzy, with no exact matches.
The full corpus remains 4,323/4,524 compared at 92.05% source-shape and
80.12% strict fuzzy, with 2,325 linked-exact and 997 byte-exact; no
exact-pair loss, skipped row or wrong-type symbol. Original collision/
part assets were not available locally, so parsing and runtime behavior
remain unverified; see
[the ARM resource-path audit](FILE_READ_COLLISION_PART_PATHS_ARM_2026-10-11.md).

## Dock-space point loader (2026-10-11)

Android ARM `FileRead::loadSpacePoints` reads nine raw floats per point
from `data/bin/docks_hd.bin`. The recovered loader had byte-swapped
those words; it now uses the raw float overload without changing
coordinate conversion or group filtering. Native `gof2`, 206/206 ARM
units and 36 verifier tests pass (one skipped). Focused source-shape/
strict fuzzy remain 75.7%/26.3%, not exact. The full corpus remains
4,323/4,524 compared at 92.05% source-shape and 80.12% strict fuzzy,
with 2,325 linked-exact and 997 byte-exact; no exact-pair loss, skipped
row or wrong-type symbol. The resource itself was unavailable locally,
so runtime values remain unverified; see
[the ARM dock-space audit](FILE_READ_SPACE_POINTS_RAW_FLOAT_ARM_2026-10-11.md).

## Weapon-position loader (2026-10-11)

Android ARM `FileRead::loadWeaponPositions` opens `data/bin/weapons_hd.bin`
and reads position words and type-3 floats without byte swapping. The
recovered path and seven switched reads were corrected. A read-only walk
consumed all 7,484 reference bytes as 59 ship records, 552 positions and
236 extra vectors. Native `gof2`, 206/206 ARM units and 36 verifier tests
pass (one skipped). Focused source-shape/strict fuzzy remain 83.1%/36.0%,
not exact. The full corpus remains 4,323/4,524 compared at 92.05%
source-shape and 80.12% strict fuzzy, with 2,325 linked-exact and 997
byte-exact; no exact-pair loss, skipped row or wrong-type symbol.
Asset-backed runtime behavior and absent ship IDs remain unverified; see
[the ARM weapon-position audit](FILE_READ_WEAPON_POSITIONS_ARM_2026-10-11.md).

## Core binary resource paths (2026-10-11)

The systems, wanted, agents, items and ships loaders now probe and open
the Android ARM `data/bin/*.bin` paths instead of five incorrect
`*_binary.bin` filenames. Native `gof2`, 206/206 ARM units, and 36
verifier tests pass (one skipped). The five-function focused family is
92.7% source-shape / 59.1% strict fuzzy, with no exact matches. The
full corpus remains 4,323/4,524 compared at 92.05% source-shape and
80.12% strict fuzzy, with 2,325 linked-exact and 997 byte-exact; no
exact-pair loss, skipped row or wrong-type symbol. The original assets
have not been mounted into a running game; see
[the ARM resource-path audit](FILE_READ_CORE_RESOURCE_PATHS_ARM_2026-10-11.md).

## Station resource loader (2026-10-11)

All three `FileRead::loadStationsBinary` overloads now probe and open
Android's `data/bin/stations.bin`, replacing the incorrect
`stations_binary.bin` path. Native `gof2` and 206/206 ARM units build.
The focused family is 93.3% source-shape / 54.5% strict fuzzy; none of
the overloads is exact. The full corpus remains 4,323/4,524 compared,
92.05% source-shape, 80.12% strict fuzzy, 2,325 linked-exact and 997
byte-exact, with zero exact-pair losses, skips or wrong-type symbols.
The game runtime asset mount and the no-argument loader's incomplete
color-table writes remain unverified; see
[the ARM station-loader audit](FILE_READ_STATIONS_RESOURCE_ARM_2026-10-11.md).

## Ticker resource loader (2026-10-11)

Android ARM `FileRead::loadTicker` opens `data/bin/ticker.bin` and reads
59 records with four raw race-flag words. The recovered loader used the
wrong filename and switched those four words; both are corrected. The
available reference binary is **1,652 bytes = 59 × 28**, and every record
passed a read-only layout/flag/level-bound check. Focused source-shape
and strict fuzzy remain **81.6%/52.1%**, not exact. Native `gof2`,
**206/206** ARM units and 36 verifier tests pass (one skipped). The full
corpus remains **4,323/4,524** compared at **92.05%** source-shape,
**80.12%** strict, **2,325 linked-exact** and **997 byte-exact**;
there are no lost exact pairs, skipped rows or wrong-type symbols.
The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_ticker_loader_20261011.json`.
The analyzed resource was not installed into a native game runtime;
see [the ARM loader audit](FILE_READ_TICKER_RESOURCE_ARM_2026-10-11.md).

## NewsTicker token replacement (2026-10-11)

The former `replaceTokens` identity stub now follows the Android ARM
sequence for all **21** marker literals, including conditional news
groups and their random draws. Focused source-shape/strict fuzzy rise
from **0.3%/0.2%** to **91.8%/45.6%**; the function itself is not exact.
Native `gof2`, **206/206** ARM units and 36 verifier tests pass (one
skipped). The full corpus compares **4,323/4,524** originals at
**92.05%** source-shape and **80.12%** strict, with **2,325
linked-exact** (+2, both `Item` array template operations emitted by
the Vossk branch) and **997 byte-exact** (unchanged). No exact-pair
losses, skipped rows or wrong-type symbols occurred. The retained report
is `C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_ticker_tokens_20261011.json`.
News rendering and asset-dependent token paths have not been exercised
in a running game; see [the ARM token audit](NEWS_TICKER_TOKEN_REPLACEMENT_ARM_2026-10-11.md).

## Random-name loader and composition (2026-10-11)

`Globals::getRandomName` now draws bounded indices from `Globals::rnd`,
copies list entries before releasing them, and joins first/last names
according to the Android ARM branch. `FileRead::loadNamesBinary` now
selects the 13 confirmed `data/bin/names_*.bin` resources and uses the
same RNG for mixed-race selection; extracted Java assets corroborate all
13 basenames. Focused source-shape/strict fuzzy rise from **85.7%/59.4%**
to **95.6%/69.8%** for the composer, and **80.4%/59.4%** to
**84.3%/77.3%** for the loader. Neither is exact. Native `gof2`,
**206/206** ARM units and 36 verifier tests pass (one skipped). The full
corpus remains **4,323/4,524** compared at **92.02%** source-shape,
**80.11%** strict, **2,323 linked-exact** and **997 byte-exact**;
there are no exact-pair losses, skipped rows or wrong-type symbols.
The report is retained at
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_names_20261011.json`.
No game-session asset read was exercised. `replaceTokens` was recovered in
the later token audit above; see [the ARM random-name audit](RANDOM_NAME_LOADER_ARM_2026-10-11.md).

## Globals random station and drinks system (2026-10-11)

Android ARM confirms `Globals::getRandomStation` draws from `Globals::rnd`,
loads through a real `FileRead`, and returns the station. The previous null
RNG read is gone; the function is now **linked-exact** at **100%/100%**
focused source-shape/strict fuzzy. `getRandomSystemForDrinks` now draws
from the same RNG and returns `Galaxy::getSystem`, correcting a `void`
return and null writes; its focused source-shape/strict rise to
**94.3%/45.7%**. Native `gof2`, **206/206** ARM units and 36 verifier
tests pass (one skipped). The full corpus compares **4,323/4,524**
original functions at **92.02%** source-shape and **80.10%** strict,
with **2,323 linked-exact** (+1) and **997 byte-exact** (unchanged).
No exact-pair losses, skipped rows or wrong-type symbols occurred. The
retained report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_globals_random_20261011.json`.
The later audits recovered random-name loading and `replaceTokens`;
game-data runtime behavior is still untested. See
[the ARM random-source audit](GLOBALS_RANDOM_SOURCES_ARM_2026-10-11.md).

## ModStation station-entry ticker construction (2026-10-11)

The Android ARM `state 80` branch copies the station's ticker/touch X
boundary from `Layout+0xcc` to `ModStation+0xa8`; `state 60` then constructs
`NewsTicker`, resets its related flags, creates `ChoiceWindow`, and loads
the faction image. These steps now replace the unresolved ticker placeholder.
The `+0xa8` field is a coordinate, not geometry; the unrelated cutscene
geometry dereference now uses `CutScene+0x20` as in ARM. Focused
`OnInitialize` source-shape rises **57.9% to 61.4%**; strict is **16.9%**,
so this function is still far from exact. Native `gof2`, **206/206** ARM
translation units, and 36 verifier tests pass (one skipped). The full
corpus remains **4,323/4,524** compared, **92.01%** source-shape,
**80.09%** strict, **2,322 linked-exact**, **997 byte-exact**, with no
lost exact pair, skipped row, or wrong-type symbol. The report is retained
at `C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_station_ticker_final_20261011.json`.
Station entry/exit in a running game remains unverified; the later token
audit above restored `replaceTokens`. See [the ARM station-entry audit](MODSTATION_TICKER_CONSTRUCTION_ARM_2026-10-11.md).

## NewsTicker global wiring and lifetime (2026-10-11)

Android ARM confirms the ticker's canvas, screen, layout, font, random,
status and text sources. The recovered class now uses those `Globals`
members instead of null stand-ins, restores the original text separator,
and lets C++ destroy its member `String` once. Across its 11 focused rows,
source-shape rises **63.9% to 88.0%** and strict **50.7% to 71.3%**;
`getHeight` and two destructor aliases gain linked-exact matches. Native
`gof2`, 206 ARM units and 36 verifier tests pass (one skipped). The full
corpus is **4,323/4,524**, **92.01%** source-shape, **80.09%** strict,
**2,322 linked-exact**, **997 byte-exact**, with no exact-pair loss.
Station-side construction and token substitution were restored in the
subsequent audits above; in-game rendering remains unverified. See
[the ARM ticker audit](NEWSTICKER_GLOBAL_WIRING_ARM_2026-10-11.md).

## StarSystem supernova intro transform handoff (2026-10-11)

The intro path now calls the actual canvas transform getter and animation
state setter in the ARM-confirmed **0, 3, 1** sequence instead of two null
function pointers. It also reloads the sun and streak pointers as the native
code does. Focused source-shape/strict fuzzy rise from **57.5%/42.6%** to
**100%/100%** at 108/108 instructions (not linked- or byte-exact). Native
`gof2`, 206 ARM units and 36 verifier tests pass (one skipped). The full
corpus is **4,323/4,524**, **91.95%** source-shape, **80.04%** strict,
**2,319 linked-exact**, **997 byte-exact**, with no exact-pair losses or
wrong-type symbols. Device gameplay and broader initialization remain
untested; see [the ARM intro audit](STARSYSTEM_SUPERNOVA_INTRO_ARM_2026-10-11.md).

## StarSystem supernova scale recovery (2026-10-11)

Three supernova scaling paths now use the Android ARM float words instead of
incorrect values several orders of magnitude away. Reversal and expansion
also use the native `Vector const&` overload. Both reach **100% focused
source-shape / 100% strict fuzzy**; the delta-scaling path remains **96.2% /
73.0%** but now uses the correct `0.00004f` coefficient. Native `gof2`, all
206 ARM units and 36 verifier tests pass (one skipped). The full corpus is
**4,323/4,524**, **91.94%** source-shape, **80.03%** strict, **2,319
linked-exact**, **997 byte-exact**, with no exact-pair loss. Device gameplay
remains unverified; the intro calls were resolved in the later audit. See
[the ARM scale audit](STARSYSTEM_SUPERNOVA_SCALE_ARM_2026-10-11.md).

## Purchase application-data producer cross-check (2026-10-11)

The Android Google Play consumable callback uses direct credit dispatch and
does not fill `GameData+0x54/+0x58/+0x60`. Recovered Android C++ initializes
those pointers to null and reads them in `ModStation`, with no recovered writer.
An original-binary follow-up confirmed the pointer getter at `0x82498`,
checked its 40 named call-site references across 16 functions, and verified
the station's three field loads in Thumb code. No producer appears on that
named-getter path; direct/indirect aliases remain unresolved.
An iOS StoreKit response independently confirms the product-ID list and
indexed integer-value pattern, but has a different application-data layout.
No Android producer or ownership contract has been confirmed, so no synthetic
JNI allocation was added. No C++ changed; the last validated corpus remains
**4,323/4,524**, **91.91%** source-shape, **80.00%** strict, **2,319
linked-exact**, **997 byte-exact**. See
[the cross-platform audit](PURCHASE_APPDATA_PRODUCER_CROSS_PLATFORM_2026-10-11.md).

## Android consumable purchase dispatch (2026-10-11)

The JNI consumable callback goes directly through `ndk_iapBoughtConsumable`
to `setBaughtCredits`; it does not produce the separate `GameData` station
purchase arrays. Reconstructing the ARM `tbb` switch raises focused
source-shape/strict fuzzy from **36.3%/39.1%** to **70.2%/83.3%** while a
native smoke confirms all five amounts and both rejection gates. Native
`gof2`, 206 ARM units, and verifier tests pass. The full corpus remains
**4,323/4,524** compared at **91.91%** source-shape and **80.00%** strict,
with **2,319 linked-exact** and **997 byte-exact** and no lost exact pair.
End-to-end billing and any other producer of the `GameData` arrays remain
unverified; see [the ARM audit](ANDROID_CONSUMABLE_IAP_ROUTE_ARM_2026-10-11.md).

## Android item-information reset and globals (2026-10-11)

The reset now frees the same 25 ID/name/description/currency/price pointers
that the Android setter fills, matching the original ARM GOT targets. Host
copies use nothrow `new[]` to match that `delete[]`, while ARM retains its
original `malloc` call. Twenty
synthetic `cItemListID_05..24` data symbols absent from the original were
removed. The native reset smoke, native `gof2`, 206 ARM units and 36 verifier
tests pass (one skipped). The full corpus remains **4,323/4,524** compared,
**91.91%** source-shape, **79.99%** strict fuzzy, **2,319 linked-exact** and
**997 byte-exact**, with no lost exact pair. The verifier's 112 extra *function*
symbols are unaffected by removing data symbols. Setter/reset call sequencing
and the original Android allocator pairing are not runtime-tested; see
[the ARM audit](ANDROID_ITEM_INFO_RESET_ARM_2026-10-11.md).

## Android premium-DLC callback (2026-10-11)

The Android JNI purchase callback now applies the ARM-confirmed per-pack DLC
flags, system visibility and campaign effects, and rejects invalid `bought`
values. Native smoke, native `gof2`, 206 ARM units and verifier tests pass.
The full corpus compares **4,323/4,524** originals at **91.91%** source-shape,
**79.99%** strict fuzzy, **2,319 linked-exact** and **997 byte-exact**, with no
exact-pair or row losses. Focused source-shape/strict fuzzy rise to
**98.6%/92.0%**. This callback is
separate from the unresolved `GameData` purchase-list producer; see
[the ARM audit](ANDROID_PREMIUM_IAP_CALLBACK_ARM_2026-10-11.md).

## ModStation purchase-result handoff (2026-10-11)

The station purchase path now uses distinct ARM-confirmed application-data
fields for product IDs (`+0x54`), pending count (`+0x58`) and credit amounts
(`+0x60`), and passes the amount to the real `Status::changeCredits` method.
The product-ID lookup now uses ARM-confirmed strings instead of empty literals
and a null stack-protector placeholder.
Native `gof2`, 206/206 ARM units and verifier tests pass. The full corpus
compares **4,323/4,524** originals at **91.89%** source-shape and **79.98%**
strict fuzzy, with **2,319 linked-exact** and **997 byte-exact**; no exact-pair
or row losses. The purchase data producers and end-to-end billing path remain
unverified; see [the purchase audit](MODSTATION_PURCHASE_RESULT_ARM_NATIVE_2026-10-11.md).

## GameData application-data layout (2026-10-11)

`GameData` now preserves the Android **0xc8** layout on ARM32 while menu,
hangar, JNI and `MGame` read its flags by typed fields on 64-bit hosts.
This removes fixed-offset accesses that would overlap wider host `String`
objects. Native `gof2`, 206/206 ARM units and verifier tests pass.
The full corpus compares **4,323/4,524** originals: **91.89%** source-shape,
**79.97%** strict fuzzy, **2,319 linked-exact** (+2) and **997 byte-exact**;
no exact-pair or row losses. `ndk23_newrender` rises to 98.8% source-shape.
The separate `ModStation` purchase fields and running-game DLC flow remain
unverified; see [the layout audit](GAME_DATA_APPDATA_LAYOUT_ARM_NATIVE_2026-10-11.md).

## Android engine singleton and DLC handoff (2026-10-11)

The JNI bridge, NDK hooks and `GetEngine()` now use the Android-exported
`gEngine` pointer instead of an uninitialized `Engine::g_pEngine`
pointer-to-pointer. ARM confirms both initialization paths, cleanup,
pause/resume, DLC checks and the `+0x3d` to `+0x40` native request handoff.
Native `gof2`, all 206 ARM units and verifier tests pass. The full corpus
remains **4,323/4,524** compared and rises to **91.89% source-shape**,
**79.96% strict fuzzy**, **2,317 linked-exact** (+3) and **997 byte-exact**,
without exact-pair or row losses. The Java store side and 64-bit
`GameData` byte-layout path remain unverified; see
[the ARM singleton audit](ANDROID_ENGINE_SINGLETON_DLC_HANDOFF_ARM_2026-10-11.md).

## MenuTouchWindow DLC dialog call (2026-10-11)

`callDlcMenu` now uses the ARM-confirmed application-data getter and
two-string `ChoiceWindow::set` overload instead of undefined callbacks.
It rises from 97.3%/84.9% to **100% source-shape / 100% strict fuzzy**
and gains a linked-exact match. Native `gof2`, all 206 ARM units and verifier
tests pass. The full corpus remains **4,323/4,524** originals at
**91.88%** source-shape and **79.94%** strict fuzzy, with **2,314
linked-exact** and **997 byte-exact**; no row or exact-pair losses.
The platform DLC request consumer and running-game dialog remain untested;
see [the ARM call evidence](MENUTOUCH_CALL_DLC_MENU_ARM_2026-10-11.md).

## ModStation DLC menu setup (2026-10-11)

`ModStation::showDlcMenu` now uses the ARM-confirmed menu constructor,
button-coordinate globals, open/close flag bytes and DLC-menu call instead
of null globals and unresolved fragments. Focused source-shape rises
**57.3% to 99.0%**, strict fuzzy **29.8% to 72.6%**; it is not exact.
Native `gof2`, 206/206 ARM units and verifier tests pass. The full corpus
compares **4,323/4,524** originals: **91.88%** source-shape, **79.94%**
strict, **2,313 linked-exact**, **997 byte-exact**, with no row or exact-pair
losses. Running-game DLC behavior remains unverified; next audit the
downstream helper. See [the ARM evidence](MODSTATION_SHOW_DLC_MENU_ARM_2026-10-11.md).

## Station loader synthetic smoke (2026-10-10)

A native test now runs the production system-specific loader body against
synthetic station records. It verifies file-order output, duplicate and
missing IDs, an empty ID list, one close per case, and released station
instances. No engine source changed, so the last full ARM report remains
**4,323/4,524**, **91.87%** source-shape, **79.93%** strict fuzzy,
**2,313 linked-exact** and **997 byte-exact**. Real file decoding, asset
order and whole-game rendering remain untested; see the
[smoke evidence](FILE_READ_STATION_SYNTHETIC_SMOKE_2026-10-10.md).

## Station loader ordering audit (2026-10-10)

Android ARM confirms that the system-specific loader writes matching
stations sequentially in file-record order, closes the file on normal
returns, and calls `SolarSystem::getStations()` for the ID list. The source
now uses that getter; native `gof2` and 206/206 ARM units build. The full
corpus remains **4,323/4,524**, **91.87%** source-shape, **79.93%** strict,
**2,313 linked-exact** and **997 byte-exact**, with no exact-pair losses.
The loader's strict fuzzy rises **48.3% to 56.2%**, while source-shape falls
**94.0% to 93.7%**. The original missing-file path leaks its preallocated
array; a representative host fixture and real asset-order check remain
unperformed. See the [ARM loader audit](FILE_READ_STATION_ORDER_ARM_2026-10-10.md).

## Status planet arrays and station loader (2026-10-09)

`Status` planet-name/texture arrays now keep full pointers on native hosts.
ARM confirms both getters' offsets and the system-specific station loader
calls in `Status::setStation` and `StarSystem`; those two callers no longer
invoke the zero-return no-argument stub. Native `gof2`, 206/206 ARM units
and verifier tests pass. Corpus coverage remains **4,323/4,524** at
**91.87%** source-shape, **79.93%** strict fuzzy, **2,313 linked-exact**
and **997 byte-exact**, with no exact-pair losses. The getter bodies remain
byte-exact; `setStation` improves **86.5% to 86.9%** source-shape.
Station-loader order/ownership and the whole-game display path remain
unverified; see the [ARM call evidence](STATUS_PLANET_ARRAY_STATION_LOADER_ARM_2026-10-09.md).

## Status standing pointer ABI (2026-10-09)

Android ARM confirms `Status::getStanding()` returns the `Standing *` stored
at `Status + 0x14`. The getter and all direct callers now preserve that
pointer on 64-bit native hosts; the getter remains ARM byte-exact. Native
`gof2`, 206/206 ARM units and verifier tests pass. The full corpus remains
**4,323/4,524** at **91.87%** source-shape and **79.93%** strict fuzzy,
with **2,313 linked-exact** and **997 byte-exact** pairs and no exact losses.
Whole-game runtime and other 32-bit pointer fields remain unverified; see
the [standing pointer evidence](STATUS_STANDING_POINTER_ABI_2026-10-09.md).

## Wanted-system array lookup (2026-10-09)

`Status::activateNewWanted` now indexes the loaded `SolarSystem *` array by
each station's numeric system number before calling `getRoutes()`, matching
both lookup sequences in Android ARM. Native `gof2`, all 206 ARM units and
the verifier tests pass. The full corpus remains **4,323/4,524** at
**91.87%** source-shape and **79.93%** strict fuzzy, with **2,313
linked-exact** and **997 byte-exact** pairs; no comparison row or exact pair
is lost. This function's own source-shape falls **88.7% to 88.4%** despite
the corrected dataflow. Its remaining control flow and whole-game runtime
are not verified; see the [ARM lookup evidence](STATUS_ACTIVATE_WANTED_SYSTEM_LOOKUP_ARM_2026-10-09.md).

## Galaxy and Status system pointer ABI (2026-10-09)

Android ARM confirms that `Galaxy::getSystem` returns a `SolarSystem *` and
`Status::getSystem` reads that pointer from `Status + 0x1a4`. The declaration,
stored field and callers that narrowed through Windows `long` now preserve
the pointer on 64-bit native builds. Native `gof2` and all 206 ARM units build;
the full corpus remains **4,323/4,524** at **91.87%** source-shape and
**79.93%** strict fuzzy, with **2,313 linked-exact** and **997 byte-exact**
and no exact-pair losses. The two short methods remain byte-exact. Native
whole-game runtime and raw 64-bit `Status` layout consumers are not yet
verified; see the [system pointer evidence](GALAXY_SYSTEM_POINTER_ABI_2026-10-09.md).

## Galaxy station pointer ABI (2026-10-09)

Android ARM dataflow confirms that `FileRead::loadStation` and
`Galaxy::getStation` return `Station *`, not a 32-bit integer. Their native
64-bit declarations, definitions and direct callers now preserve the full
pointer. Native `gof2` and all 206 ARM units build; the full comparison still
covers **4,323/4,524** originals across 4,689 rows at **91.87%** source-shape
and **79.93%** strict fuzzy, with **2,313 linked-exact** and **997 byte-exact**
pairs and no exact losses. This does not verify native gameplay; the allocation
paths still differ from ARM. See the [station pointer evidence](GALAXY_STATION_POINTER_ABI_2026-10-09.md).

## BoundingVolume projection return contract (2026-10-09)

Android ARM proves that `staticProjectCollisionOnSurface` returns a `Vector`
through a hidden result pointer, not through a fake `BoundingVolume` receiver.
The method and its three game callers now use the typed static return; a
focused host smoke passes. Native `gof2`, 206 ARM units and the full verifier
remain green. Coverage is **4,323/4,524** across 4,689 rows; source-shape
**91.87%**, strict fuzzy **79.93%**, linked-exact **2,313**, byte-exact **997**.
No comparison row or exact pair is lost. The function itself rises to 96.4%
source-shape but is not exact. Runtime collision behavior beyond the smoke is
unverified; see the [ABI follow-up](BOUNDING_VOLUME_VTABLE_ARM_2026-10-09.md).

## BoundingVolume virtual ABI recovery (2026-10-09)

The Android-backed five-slot virtual order is restored for BoundingVolume,
BoundingSphere and BoundingAAB, removing the incorrect virtual destructor
slots. A relocation-based check matches all 15 compiled method slots to the
original ELF. Native `gof2` and all 206 ARM units build. The full corpus
still compares **4,323/4,524** originals across 4,689 rows with zero
wrong-type misses and skipped units; source-shape is **91.87%**, strict fuzzy
**79.92%**, linked-exact **2,313** and byte-exact **997**. No comparison row
or exact pair is lost relative to the prior integration; four linked pairs
are gained. The virtual ABI is verified, but collision gameplay and native
runtime remain untested. See the [ABI evidence](BOUNDING_VOLUME_VTABLE_ARM_2026-10-09.md).

## MenuTouchWindow, PlayerFixedObject and Generator integration (2026-10-09)

The three latest fixed-family branches are merged into
`codex/integrate-main-working-20261005`. MenuTouchWindow improves to 87.684%
unweighted / 61.514% original-instruction-weighted source-shape across 29
unique bodies; PlayerFixedObject reaches 94.079% / 97.095% across 29; Generator
reaches 98.673% / 97.460% across 11. **None passes both 98% gates.** The
native `gof2` library builds, all 206 ARM units compile, verifier tests and
the MenuTouchWindow update smoke pass. The full corpus compares **4,323/4,524**
originals across 4,689 rows, with zero wrong-type misses and zero skipped
units. Mean source-shape is **91.85%**, strict fuzzy **79.89%**,
linked-exact **2,309**, byte-exact **997**. Relative to the preceding
integrated report, no comparison row or exact pair is lost; four linked-exact
pairs are gained in PlayerFixedObject. These are assembly-comparison figures,
not a playable-game completion percentage. Whole-game/ARM-device runtime is
untested; the pre-existing BoundingVolume virtual-slot discrepancy and
64-bit pointer-width contracts remain risks. See the
[integration evidence](MAIN_WORKTREE_RECONCILIATION_2026-10-05.md) and family
notes for [MenuTouchWindow](MENUTOUCHWINDOW_UNIQUE_ARM_98_2026-10-08.md),
[PlayerFixedObject](PLAYERFIXEDOBJECT_UNIQUE_ARM_2026-10-08.md) and
[Generator](GENERATOR_UNIQUE_ARM_RECOVERY_2026-10-08.md).

## ChoiceWindow, Route and Ship integration (2026-10-08)

The three fixed-family branches are merged into
`codex/integrate-main-working-20261005`. Each reaches both unique-body 98%
source-shape means: ChoiceWindow 99.638% unweighted / 98.634% weighted,
Route 99.341% / 98.409%, and Ship 99.299% / 98.588%. The native `gof2`
library builds, all 206 ARM units compile, and verifier/scope tests pass.
The full corpus compares **4,323/4,524** originals across 4,689 rows,
with zero wrong-type misses and zero skipped units. Mean source-shape is
**91.80%**, strict fuzzy **79.73%**, linked-exact **2,305** and byte-exact
**997**. Compared by (unit, symbol) with the prior integrated report, no
comparison row or exact pair is lost; four linked-exact pairs are gained.
These are assembly-comparison results, not a playable-game completion
percentage. Native gameplay/ARM-device runtime remains untested. See the
[integration evidence](MAIN_WORKTREE_RECONCILIATION_2026-10-05.md) and family
notes for [ChoiceWindow](CHOICEWINDOW_UNIQUE_ARM_98_2026-10-08.md),
[Route](ROUTE_UNIQUE_ARM_98_2026-10-08.md) and
[Ship](SHIP_UNIQUE_ARM_98_2026-10-08.md).

## HangarWindow, MGame and Radar integration (2026-10-08)

The three fixed-family branches are merged into
`codex/integrate-main-working-20261005`. HangarWindow reaches 99.34%
unweighted / 98.007% original-instruction-weighted source-shape on 30 unique
bodies; MGame reaches 98.874% / 98.041% on 35 unique bodies. Radar improves
to 99.629% / 97.977% on 17 unique bodies and **does not pass** the weighted
98% gate. Native `gof2`, all 206 ARM units and verifier tests pass. The full
corpus compares **4,323/4,524** originals across 4,689 rows, with zero
wrong-type misses and zero skipped units. Its mean source-shape is **91.76%**,
strict fuzzy **79.65%**, linked-exact **2,301** and byte-exact **997**.
Compared by (unit, symbol) with the preceding integrated report, no exact
pair is lost; one extra comparison row comes from a restored native
`ArrayRemove<Ship*>` call. These are source/assembly comparisons, not a
playable-game completion percentage. See the [integration evidence](MAIN_WORKTREE_RECONCILIATION_2026-10-05.md)
and the family notes for [HangarWindow](HANGARWINDOW_UNIQUE_ARM_98_2026-10-08.md),
[MGame](MGAME_ARM_SOURCE_SHAPE_2026-10-08.md) and
[Radar](RADAR_ARM_98_UNIQUE_BODY_GATE_2026-10-08.md).

## Latest local integration (2026-10-08)

The latest Gun, Mesh loader and StarMap branch tips are merged into
`codex/integrate-main-working-20261005`. Native `gof2`, 206/206 ARM translation
units, verifier tests and focused Gun/Mesh smokes pass. The full ARM corpus
compares **4,323/4,524** original functions across 4,688 rows, with zero
wrong-type misses and zero skipped units. Mean source-shape is **91.76%** and
strict fuzzy **79.63%**; **2,301 linked-exact** and **997 byte-exact** pairs
remain. Relative to the prior integrated snapshot there are zero exact-pair
losses and one linked-exact gain (`StarMap::render`). These are comparison
figures, not a playable-game completion percentage. The original `main`
checkout and GitHub remote have not been changed.

## Main working-tree reconciliation (2026-10-05, local integration branch)

The non-overlapping AbyssEngine, HangarWindow, Radar, ParticleSettings and
ABI changes from the dirty `main` checkout are now collected on
`codex/integrate-main-working-20261005`; newer Player/StarMap source forms
remain in place. Native `gof2`, 206/206 ARM units and focused runtime smokes
build or pass. The full corpus compares **4,323/4,524** originals across
4,688 rows at **91.75%** mean source-shape / **79.62%** strict, with **2,300
linked-exact** and **997 byte-exact** pairs. Against the preceding integrated
snapshot there are zero exact-set losses and 101 linked / eight byte gains.
This is not a playable-game or semantic-completion percentage; integrated
gameplay runtime remains unverified. The original dirty `main` checkout is
unchanged; see [the reconciliation note](MAIN_WORKTREE_RECONCILIATION_2026-10-05.md).
The later Gun, StarMap and Mesh-documentation branch tips have also been
merged; their focused source-shape improvements leave these rounded full-corpus
totals and all exact-pair sets unchanged. The final local report is
`_work/integrated_main_working_latest_20261005.json`.

## Integrated recovery branches (2026-10-05, local integration branch)

The LevelScript/Level, TargetFollowCamera, PlayerTurret, particle control,
Player/Gun, StarMap, Mesh loader, and Blueprint/ModStation recovery branches
are combined in `codex/integrate-decomp-20261005`. This does not include the
uncommitted changes in the separate `main` checkout and is not yet merged into
`main`. Native `gof2`, 206/206 ARM units, verifier unit tests, and focused
Player, particle, Mesh and PlayerTurret checks pass. The integrated ARM corpus
compares **4,322/4,524** originals across 4,683 rows, with 202 absent,
zero wrong-type misses and zero skipped units. Mean source-shape is **90.63%**
and strict fuzzy **78.15%**; **2,199 linked-exact** and **989 byte-exact**
pairs remain after the later Gun collision-control update. Relative to the
latest standalone ModStation snapshot, exact
sets have zero losses and gain 33 linked / 12 byte pairs. Saved focused exact
sets for TargetFollowCamera, PlayerTurret and StarMap also remain present.
These are code-generation comparisons, not a percentage of playable or
semantically verified game behavior; integrated gameplay runtime is untested.
The local full report is `_work/integrated_full_gun_20261005.json`.

## TargetFollowCamera 99% Gate (2026-10-02)

The unchanged 40-body / 42-symbol family now passes both means: **99.965%
unweighted and 99.269% weighted by original executable instructions** across
unique bodies (99.967% / 99.340% with aliases). Native `gof2`, all 204 ARM TUs,
the 4,631-pair full corpus and zero wrong-type gate pass. No baseline exact
pair is lost; focused counts rise from 27/19 to 32 linked-exact / 23 byte-exact.
The result uses a tested correction for proven NEON literal pools and COMDAT
aliases, applied equally to baseline and final. `update` is 98.6% source /
39.9% strict, not exact; no ARM32 runtime is claimed. See
[the evidence and trial ledger](TARGETFOLLOWCAMERA_ARM_SOURCE_SHAPE_2026-10-02.md).

## ModStation pending-product delivery (2026-10-05, local branch)

`ModStation::checkPendingProducts` now follows the Android-confirmed cargo,
message and special-product paths instead of null fragment placeholders.
Focused source-shape rises from 43.3% to 99.7%, strict fuzzy from 23.1% to
86.8%; it is not exact. `Status::getPendingProducts` now preserves pointer
width on native hosts and remains ARM byte-exact. Native `gof2` and 206/206
ARM units build. The full corpus compares 4,322/4,524 originals across
4,675 rows, at 90.15% mean source-shape / 77.30% strict, with 2,166
linked-exact and 977 byte-exact pairs and zero exact-set losses. Integrated
runtime remains unverified. The next recovery priority is
`ModStation::showDlcMenu`; see
[`MODSTATION_PENDING_PRODUCTS_ARM_2026-10-05.md`](MODSTATION_PENDING_PRODUCTS_ARM_2026-10-05.md).

## ModStation entry snapshot (2026-10-05, local branch)

`ModStation::enterStation` now calls the Android-confirmed station, medal,
ship and equipment paths directly instead of a null `getShip` callback.
Focused source-shape and strict fuzzy rise from 46.1%/38.6% to 100%/100%,
and the function is newly linked-exact (not raw byte-exact). Native `gof2`
and 206/206 ARM units build. The full corpus remains at 4,322/4,524
originals across 4,675 rows, with 90.14% mean source-shape, 77.28%
strict, 2,166 linked-exact and 977 byte-exact. Exact pair sets have one
linked gain and zero losses. Integrated game runtime remains unverified;
the next source-recovery priority is `ModStation::checkPendingProducts`.
See [`MODSTATION_ENTER_STATION_ARM_2026-10-05.md`](MODSTATION_ENTER_STATION_ARM_2026-10-05.md).

## ModStation station lighting (2026-10-05, local branch)

`ModStation::resetLight` now uses the Android-confirmed engine calls,
negated direction fields, race-dependent ambient colors, and material
shininess instead of null fragment helpers. Its 108-instruction focused
comparison is 100% source-shape and strict fuzzy (from 28.9%/15.2%),
but is not linked-exact or byte-exact. Native `gof2` and all 206 ARM units
build. The full corpus compares 4,322/4,524 originals across 4,675 rows
at 90.13% mean source-shape / 77.27% strict, with 2,165 linked-exact and
977 byte-exact; exact pair sets have zero losses. In-game rendering is
unverified. The subsequent `enterStation` recovery is recorded above;
see [`MODSTATION_RESET_LIGHT_ARM_2026-10-05.md`](MODSTATION_RESET_LIGHT_ARM_2026-10-05.md).

## ModStation departure and campaign pointer ABI (2026-10-04, local branch)

The recursive `ModStation::leaveStation` stub now follows the Android ARM
cargo, campaign, equipment, ship-index and modal checks before displaying
departure confirmation. Focused source-shape rises from 0.1% to 91.8%
(strict 0.0% to 71.5%), without an exact claim. The related
`Status::getCampaignMission` return and one `Player::damage` local now retain
the mission pointer width on 64-bit native hosts; the ARM getter remains
byte-exact. Native `gof2` and 206/206 ARM units build. The full corpus
compares 4,322/4,524 originals across 4,675 rows at 90.11% mean
source-shape / 77.25% strict, with 2,165 linked-exact and 977 byte-exact;
exact pair sets have zero losses. Integrated departure/runtime behavior is
still unverified; see
[`MODSTATION_LEAVE_STATION_ARM_2026-10-04.md`](MODSTATION_LEAVE_STATION_ARM_2026-10-04.md).

## ModStation medal-array source ABI (2026-10-04, local branch)

Android `ModStation+0xbc` holds an `Array<int *> *`, not an `int *` needing
pointer-to-pointer casts. The field is now typed directly, with its ARM32
offset asserted. The campaign-mission `addAchievement` call now uses the
real station object rather than constructing an invalid C++ receiver from
integer `1`; the ARM call sequence and linked-exact callee remain unchanged.
Native `gof2` and 206/206 ARM units build. Full comparison stays at
4,322/4,524 originals, 4,675 rows, 90.09% mean source-shape, 77.24%
strict, 2,165 linked-exact and 977 byte-exact, with zero exact-set losses.
Array ownership and in-game runtime are still unverified; see
[`MODSTATION_MEDAL_ARRAY_ABI_ARM_2026-10-04.md`](MODSTATION_MEDAL_ARRAY_ABI_ARM_2026-10-04.md).

## ModStation medal reward and notification bindings (2026-10-04, local branch)

The Android-confirmed four-value credit table now drives both award paths
in `ModStation::checkMedals`, replacing null pointers. `addAchievement`
now updates the existing `g_android_current_achievements[3]` array through
the original global route and is linked-exact; `checkMedals` rises from
92.5% to 94.5% source-shape but is not exact. Native `gof2` and all 206 ARM
units build. The full corpus compares 4,322/4,524 originals across 4,675
rows at 90.09% mean source-shape / 77.24% strict, with 2,165 linked-exact
and 977 byte-exact. Exact sets have one linked gain and zero losses.
Integrated medal/credit runtime, `medalArray` typing and an unusual native
call receiver remain unverified; see
[`MODSTATION_MEDAL_REWARDS_ARM_2026-10-04.md`](MODSTATION_MEDAL_REWARDS_ARM_2026-10-04.md).

## ChoiceWindow draw bindings and medal display (2026-10-04, local branch)

`ChoiceWindow::draw` now uses the Android-confirmed `Globals` bindings,
medal-color and credit tables, the `"+ "` credit prefix, original image/box
overloads, and button-position/visibility stores instead of null local
placeholders. Its focused source-shape rises from 79.3% to 95.8% and strict
fuzzy from 32.4% to 74.4%; it is not exact or runtime-certified. Native
`gof2` and all 206 ARM units build. The full corpus still compares
4,322/4,524 originals across 4,675 rows, with 90.09% mean source-shape,
77.23% strict, 2,164 linked-exact and 977 byte-exact; exact-set comparison
has zero losses. Medal reward accounting in `ModStation` is the next related
behavior gap. See [`CHOICEWINDOW_DRAW_ARM_2026-10-04.md`](CHOICEWINDOW_DRAW_ARM_2026-10-04.md).

## Status agent pointer-width ABI (2026-10-04, local branch)

`Status::getAgents()` now returns its `Array<Agent *> *` field directly;
the sole `Generator::createAgents` caller no longer routes the address through
Windows 32-bit `int` and `long`. Android ARM's getter remains byte-exact and
the caller's focused source-shape/strict scores remain 93.6%/40.6%. Native
`gof2` and all 206 ARM units build. The full corpus is unchanged at
4,322/4,524 originals, 4,675 rows, 90.09% mean source-shape, 77.22%
strict, 2,164 linked-exact and 977 byte-exact; exact pair sets have no losses
or gains. Other pointer-width risks and integrated runtime remain unverified.
See [`STATUS_GETAGENTS_POINTER_ABI_2026-10-04.md`](STATUS_GETAGENTS_POINTER_ABI_2026-10-04.md).

## Achievement constructor flags fixed (2026-10-04, local branch)

The Android constructor clears all three achievement-completion flags. The
local constructor omitted `gotAllGoldMedals_`, allowing an uninitialized
value before `countMedals()` runs; that flag is read by mission generation and
the missions window. The added zero makes both C1/C2 constructor aliases
100% linked-exact (one unique body). Native `gof2` and 206/206 ARM units
build. The full corpus remains 4,322/4,524 unique originals over 4,675 rows,
with 90.09% mean source-shape, 77.22% strict fuzzy, **2,164 linked-exact**
and **977 byte-exact**. Exact-set comparison finds two gained linked rows and
zero losses of either kind. No in-game runtime claim is made; see
[`ACHIEVEMENTS_CTOR_FLAGS_ARM_2026-10-04.md`](ACHIEVEMENTS_CTOR_FLAGS_ARM_2026-10-04.md).

## Achievement detection recovered from ARM (2026-10-04, local branch)

`Achievements::checkForNewMedal` now evaluates all 44 Android-confirmed
dispatch cases and reads the existing 45-by-3 value table, rather than a
zero-filled duplicate. Its prerequisite equipment check now uses the native
`Globals::status` binding instead of a null local pointer. Focused
source-shape rises from 6.9% to 73.7% for the check (strict 2.3% to 20.8%);
neither body is exact or in-game runtime-certified. Native `gof2` and all
206 ARM translation units build. The complete corpus compares 4,322/4,524
originals across 4,675 rows at 90.09% mean source-shape / 77.22% strict,
with 2,162 linked-exact and 977 byte-exact rows at that checkpoint. Both
exact pair sets had zero losses. The next priority is runtime validation of
representative medal states and the remaining shape divergence, preserving
the ARM-observed conditions. See
[`ACHIEVEMENTS_CHECK_ARM_2026-10-04.md`](ACHIEVEMENTS_CHECK_ARM_2026-10-04.md).

## Achievement values restored (2026-10-04, local branch)

The 135-word `Achievements::getValue` table now matches Android `.rodata`
word-for-word; its rebuilt ARM object also matches all 540 table bytes. The
method was already 100% linked-exact with a zero-filled table, demonstrating
that instruction exactness alone does not validate referenced data. Native
and 206/206 ARM builds pass. The complete corpus remains 4,322/4,524
unique originals, 90.07% source-shape, 77.21% strict, 2,162 linked-exact
and 977 byte-exact rows, with no exact-set losses. At this historical
checkpoint, achievement detection in `checkForNewMedal` was still a
17/414-instruction stub; the newer section above supersedes it. See
[`ACHIEVEMENTS_VALUE_TABLE_ARM_2026-10-04.md`](ACHIEVEMENTS_VALUE_TABLE_ARM_2026-10-04.md).

## ChoiceWindow medal message and image IDs (2026-10-04, local branch)

Android ARM evidence restores the `setMedal` number conversion, surrounding
newlines, four native global routes, 53 resource-ID table entries and direct
image-handle output fields. Focused source-shape rises from 83.9% to 99.2%,
strict fuzzy from 25.1% to 96.3%; the method is not exact. The full corpus
compares 4,322/4,524 unique originals across 4,675 rows at 90.07% mean
source-shape / 77.21% strict, with 2,162 linked-exact and 977 byte-exact
rows and no exact-set losses. Native and 206/206 ARM builds pass. The
zero-filled `Achievements::getValue` table and unresolved `ChoiceWindow::draw`
remain separate work; no complete-game runtime claim is made. See
[`CHOICEWINDOW_SETMEDAL_ARM_2026-10-04.md`](CHOICEWINDOW_SETMEDAL_ARM_2026-10-04.md).

## ChoiceWindow modal set bindings (2026-10-04, local branch)

All five `ChoiceWindow::set` overloads now use the Android-confirmed global
text, layout, dimensions, sound, font and `Globals` objects. Their focused
mean source-shape rises from 65.8% to 97.8%; the primary modal body rises
from 30.7% to 96.1%, but none is exact or runtime-certified. The full report
compares 4,322/4,524 unique originals across 4,675 rows: 2,162 linked-exact,
977 byte-exact, 90.07% mean source-shape and 77.20% strict fuzzy, with no
exact-set losses. The two extra exact rows are duplicate `Array<String*>`
comparisons, not new unique coverage. Native and 206/206 ARM builds pass.
See [`CHOICEWINDOW_SET_GLOBALS_ARM_2026-10-04.md`](CHOICEWINDOW_SET_GLOBALS_ARM_2026-10-04.md).

## ChoiceWindow constructor/width/misc layout (2026-10-04, local branch)

Android-confirmed `Globals::layout`, `Globals::w`, and `Globals::h` bindings
restore the `ChoiceWindow` constructor, `setWidth`, and `setMiscButton` from
null local pointers. Constructor C1/C2 and `setWidth` are linked-exact;
`setMiscButton` reaches 97.6% source-shape but is not exact. The complete ARM
report compares 4,322/4,524 unique originals across 4,673 rows: 2,160
linked-exact, 977 raw-byte-exact, 90.03% mean source-shape and 77.15% strict
fuzzy, with zero exact-set losses. Native and 206/206 ARM builds pass. See
[`CHOICEWINDOW_LAYOUT_GLOBALS_ARM_2026-10-04.md`](CHOICEWINDOW_LAYOUT_GLOBALS_ARM_2026-10-04.md).

## ChoiceWindow setHeight ARM binding (2026-10-04, local branch)

The Android-confirmed `Globals::layout` and `Globals::h` bindings replace
permanently null local pointers in `ChoiceWindow::setHeight`. The method rises
from 0.3% to 100% source-shape and becomes linked-exact. Full ARM verification
compares 4,322/4,524 originals across 4,673 rows, with 2,157 linked-exact,
977 byte-exact, 89.99% mean source-shape and 77.09% strict fuzzy; no exact
unit/symbol pair is lost. Native and 206/206 ARM builds pass. Other
`ChoiceWindow` bodies remain unverified; see
[`CHOICEWINDOW_SETHEIGHT_GLOBALS_ARM_2026-10-04.md`](CHOICEWINDOW_SETHEIGHT_GLOBALS_ARM_2026-10-04.md).

## NDK libc++abi runtime coverage (2026-10-04, local branch)

Two ARM-only wrappers compile the matching NDK r18b exception and type-info
runtime sources without vendoring them. The current full ARM report compares
4,322/4,524 unique original functions across 4,673 rows, with 202 absent,
zero wrong-signature and zero skipped; mean source-shape is 89.97%, strict
fuzzy 77.07%, linked-exact 2,156 and raw-byte-exact 977. Native `gof2` and
206/206 ARM translation units build. This is not an integrated `main` result
or a percentage of semantically completed game logic. ELF-declared symbol
sizes fixed three false non-exact results without exact-set losses; see
[`NDK_LIBCXXABI_RUNTIME_ARM_2026-10-04.md`](NDK_LIBCXXABI_RUNTIME_ARM_2026-10-04.md).
## ARM and TextureCreate Recovery (2026-09-24)

The verified source tree compiled all **204/204** ARM translation units at
baseline and after the accepted texture changes. The full corpus compares
4,274/4,524 original functions across 4,613 symbols, with no skipped units.
The run reports 2,038 linked-exact, 932 raw-byte-exact, 250 missing, and seven
implemented under a different signature. Compared with baseline, there are no
lost linked-exact or raw-byte-exact symbols; `TextureCreateGlobal` gained
linked-exact status.

The five-symbol `TextureCreate*` family currently averages **97.54%
unweighted source-shape** and **93.07% instruction-weighted source-shape**;
strict fuzzy averages 80.40% unweighted and 54.39% weighted. Three of five
symbols are linked-exact; none are raw-byte-exact. The 99% target was not
reached. `TextureCreateGlobal` now binds the created GL id instead of zero,
and the raw-format mipmap path follows the Android shared branch. The focused
metrics, symbol list, rejected trials, and limits are in
[`AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md`](AEI_TEXTURE_CREATE_SOURCE_SHAPE_ARM_2026-09-24.md).

## Selective Upstream Reconciliation (2026-09-20)

The unchanged fionera `ef63e5a0` snapshot was rechecked against the dirty local
tree and only Android-confirmed gaps were transferred. `PlayerTurret` now has
the native `0x168` layout and reset/revive activation; `TargetFollowCamera`
keeps size `0x178` with its tail fields at `0x120/0x130/0x138/0x13c`;
`Camera` is `0x5c` with a single Matrix construction; `Gun::setOffset(Vector*)`
uses `100.0f` and its Array lifetime no longer calls a null placeholder.
UCRT64 builds and the ARM corpus remains `206` compiled objects with the same
three unrelated failures. `ListItemWindow::update` also retains the expanded
local card while improving strict/source shape from `71.7/93.6%` to
`84.8/96.9%`. Focused family results and exact boundaries are in
`../../docs/findings/gof2_fionera_selective_transfer_20260920_ru.md`.

## How To Read Progress

Three different measures are tracked here:

1. **Buildability**: whether the host-side static library compiles.
2. **Source recovery**: whether a class, layout, or behavior has direct evidence.
3. **ARM matching**: whether a matching-toolchain build equals or resembles the
   original instructions for a function.

They are related but not interchangeable. A function can compile while still
being a placeholder; a source-backed body can be useful while not
byte-identical; a strong instruction match does not prove every semantic name.

## Measured Snapshot

The committed `report.json` snapshot reports:

| Metric | Value |
| --- | ---: |
| Original functions discovered | 4,522 |
| Functions compared by exact mangled name | 4,507 |
| Linked-exact functions | 1,697 |
| Raw-byte-exact functions | 948 |
| Average instruction similarity | 67.54% |
| Missing exact-name counterparts | 15 |
| Wrong-signature counterparts | 0 |

This is a saved measurement, not a permanent project score. Re-run the
validator locally for a new result. High comparison coverage does not mean the
entire program is semantically recovered: it includes compatibility bodies,
small accessors, and code that still needs review.

The native UCRT64 target `gof2` is currently a green compile gate and produces
`cmake-build-ucrt/libgof2.a`.

## Source Inventory

| Area | Files | Status |
| --- | ---: | --- |
| `src/` total | 204 C++ files and 234 headers | Compilable host-side tree, not a finished game source release. |
| `src/engine/` | Shared engine systems | Partial recovery; layouts and helper paths remain under audit. |
| `src/game/` | UI, ships, missions, weapons, and world logic | Partial and uneven; high-value packages received focused passes. |
| `src/platform/android/` | 4 files | Android/JNI boundary representation, not a drop-in application. |

There is no trustworthy global percentage for semantic source recovery. A
percentage attached to a named package is a local evidence estimate, never a
percentage for the whole game.

## External Cross-Version Evidence

DeepOpen J2ME v1.0.4 is pinned at commit `d300f93` as an external shared
workspace reference at `../references/DeepOpen/`; it is not part of this
Android HD decompilation repository. Its J2ME menu, Radar, and resource-manager
bodies are useful to corroborate intent and stable semantic ordering, but they
cannot establish Android ARMv7 fields, masks, timing, or byte shape. The
initial MGame/HUD, Radar, and resource-chain comparison is recorded in
`DEEPOPEN_J2ME_CROSSWALK_2026-07-20.md`.

## Notable Packages

| Package | Current understanding | ARM status |
| --- | --- | --- |
| `MenuTouchWindow` | Draw/touch routing, load/save, settings, HUD layout, DLC/store, and language-list paths received focused source-backed passes. The main-state dispatcher covers the confirmed `+0x04` button ID table, cargo summary, Google Play/restore/quit controls, and native X/Y forwarding. The state/dialog follow-up restores the phone/iPad `state 3` options body, states `10/12/13/17`, the calibration-pending byte at `+0x176`, native challenge fade values, and the selected DLC/game context at `+0x234`. State `15` now has its Android DLC launch/acknowledgement, purchase, restore, selector, result/error and app-data protocol (`+0x3c/+0x3f/+0x40/+0x48`); state `16` follows its separate native `OnTouchEnd` fallthrough instead of reusing that modal flow. Constructor social controls live in the confirmed shared auxiliary array at `+0xc0`, separate from DLC selector slots at `+0xf8`. State `9` now respects the confirmed `WantedWindow -> MissionsWindow -> MenuTouchWindow` `int` close-return chain instead of closing after every touch. See `MENUTOUCH_MAIN_DISPATCH_FREECAM_ARM_2026-08-23.md`, `MENUTOUCH_STATE_DIALOG_OPTIONS_ARM_2026-08-23.md`, `MENUTOUCH_DLC_STORE_STATE_ARM_2026-08-23.md`, and `MISSIONSWINDOW_TOUCH_END_RETURN_ABI_ARM_2026-08-23.md`. Local source-backed coverage remains an estimate around 72%; exact String cleanup and whole-switch shape remain. | UCRT64 is green. Forced-inline `OnTouchEnd` is `8.7%` at `3001/2887` target/base instructions, not linked- or byte-exact; this replaces the previous `3.7%` at `3001/2675` checkpoint. The source body is closer in instruction span but still differs in CFG, temporary lifetime and frame shape. `isMakingScreenshot` remains `100%` linked- and byte-exact at `6/6`. |
| `Radar` | Whole enemy body, station/planet phases, gas/plasma and asteroid/dead-cargo flows, direct audio, constructor/labels and distance/current-lock String lifetime recovered against Android instructions with iOS cross-checks. Render returns are now verified `void`. See `RADAR_DRAW_SOURCE_SHAPE_2026-09-15.md`. | The unchanged 19-symbol set averages **99.0% source-shape**, from 96.0526316%; 13 linked-exact and 8 raw-byte-exact. Draw is **90.8% source / 41.7% strict**, not byte-matched. Constructor aliases are 98.1%, current-lock 96.6%, distance 99.1%, ellipse 98.3%. Native build passes; current-source ARM compilation is 204/204. |
| `Hud` | Core-frame work recovered direct `Globals` routing, `PlayerEgo::level`, the `Hud::init` image/coordinate map, exact 20-entry event queue, four quick-menu construction modes, menu frame/cargo gauge, status/gamma rows, and linked gamma runtime. The complete `hudEvent` switch covers events `1..47`, Android GameText/sound routing, duplicate suppression and the `0x100019` important-event mask. Four staged runtime migrations now place every live field from `+0x000` through `+0x53b` at its Android ARM offset: Strings, event/touch state, 71 images, all packed coordinates, flash/hit timers, camera arrays and final ownership fields. The real class is compile-locked at `sizeof(Hud) == 0x53c`; the out-of-line `ArrayReleaseClasses<TouchButton *>` specialization is `100%` linked-exact. The pause button and right action cluster use the native low-byte mask, confirmed Image2D pairs, flash ordering, readiness timers and `Level+0x69` delayed-secondary lifecycle. The source-backed `Hud::draw` prelude gates the event queue, remaps persisted iPad control anchors, restores the original canvas color on both exits, and leaves pause/orbit/menu orchestration to `MGame::OnRender2D`. Its hit-direction follow-up restores the four 300 ms direction pulses, Radar-centred transforms, hacking-aware steering dimming, and distinct rocket/normal color exits. The mission-panel pass restores kill-counter, passenger, production, ordinary cargo and countdown dispatch. The reticle/dock/camera pass restores hacking controls, disabled tint, alien-orbit dock exceptions, time-extender flow, autopilot dock substitution, direct camera tables and the auto-turret/banner split. The quick/secondary/fire audit restores native pressed/idle branches, the direct Radar target predicate and the correct `fireForTutorial` main-fire state. The progress/mining tail separates the native dock-fill draw image from its geometry-metrics image and exposes the signed 64-bit `LevelScript+0x08` timer. The first whole-function lifetime pass now inlines the native iPad remap and both steering copies, restores the seven-object secondary-label chain, and matches the dock progress query/String order. The boost/protector follow-up restores the three native boost draw paths and selects the evidence-backed `-fstack-protector-strong`. The mission/cargo lifetime pass restores direct `Globals::status/Canvas` reloads, native production `getCargo` repetition and the eight temporary String slots. The progress/mining control-flow pass removes non-native singleton guards, restores the shared jump/cloak body, fresh mining predicates, single pulse-timer store and white color reset. The prelude/health-bar pass now caches post-remap iPad anchors, restores boost read lifetimes, uses native `(rate * 0.01f) * width` recurrence, and joins armor coordinates through the confirmed address pairs. The gamma/rocket/hit-entry pass restores the native sub/add gamma-coordinate recurrence, the uncollapsed secondary-flash branch, and offset-selected shield/armor hit images. The challenge/orbit/menu ABI pass restores the live challenge-score body, orbit color stride, event 27, exact secondary setter, `GameSettings` iPad anchors and the corrected `Hud+0x4cc..0x4f0` quick-menu/touch layout. The touch/menu/cargo pass restores native phone/iPad hitbox routing, quick-menu frame iteration, real cargo replacement markers, full-width event text Y and the confirmed `Hud+0x3c0` secondary-label position. The init/event pass restores native quick-menu allocation order, equipment/station String lifetimes, duplicate-event queue construction and exact medal text. Its source-shape follow-up removes the non-native event-line alias, confirms event 19 as `Hud+0x100`, restores branch-local queue-string allocation, and returns phone/iPad hit testing to native field-load lifetimes. The queue-family pass restores the integer `updateQueue` ABI, 2000/4000 ms lifecycle, source-order banner fade/slide and a byte-exact slot insertion scan. The dedicated `Hud::init` pass removes the non-native image descriptor stack table, restores all 75 static image call sites, native queue/key allocation order, ship capability queries, integer pause coordinates and the 32-byte layout copy. The cargo/menu/touch/orbit source-shape pass restores the typed `Status::replaceHash` hidden return, native cargo String expressions, draw-site coordinate reads, nonzero-element-first touch routing and orbit Canvas/font lifetimes. Generated `Hud::draw` size is now `3033/3223`, while stack reservation and the VFP save set match at `224` bytes and `d8-d11`. See `HUD_ARM32_ABI_LAYOUT_2026-08-16.md`, `HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`, `HUD_RUNTIME_TAIL_2026-08-17.md`, `ARRAY_TOUCHBUTTON_ARM_ABI_2026-08-17.md`, `HUD_PAUSE_BUTTON_ARM_2026-08-17.md`, `HUD_ACTION_CLUSTER_ARM_2026-08-17.md`, `HUD_DRAW_PRELUDE_IPAD_QUEUE_2026-08-17.md`, `HUD_HIT_DIRECTION_STEERING_ARM_2026-08-17.md`, `HUD_CARGO_MISSION_PANELS_ARM_2026-08-17.md`, `HUD_RETICLE_DOCK_CAMERA_ARM_2026-08-18.md`, `HUD_QUICK_SECONDARY_FIRE_ARM_2026-08-18.md`, `HUD_PROGRESS_MINING_TAIL_ARM_2026-08-18.md`, `HUD_DRAW_STACK_LIFETIME_ARM_2026-08-18.md`, `HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md`, `HUD_MISSION_CARGO_LIFETIME_ARM_2026-08-19.md`, `HUD_PROGRESS_MINING_CONTROL_FLOW_ARM_2026-08-19.md`, `HUD_PRELUDE_HEALTH_BARS_ARM_2026-08-19.md`, `HUD_GAMMA_ROCKET_HIT_ENTRY_ARM_2026-08-19.md`, `HUD_CHALLENGE_ORBIT_MENU_ABI_ARM_2026-08-19.md`, `HUD_TOUCH_MENU_CARGO_ARM_2026-08-19.md`, `HUD_INIT_MENU_EVENT_ARM_2026-08-20.md`, `HUD_EVENT_QUEUE_ARM_2026-08-20.md`, `HUD_INIT_ARM_2026-08-20.md`, and `HUD_CARGO_MENU_TOUCH_ORBIT_ARM_2026-08-20.md`. | UCRT64 is green; ARM corpus is `201/204` with three unrelated known failures. The 48-function Hud set averages about `86.7%`, with 26 linked-exact and 19 byte-exact functions. Constructor is linked-exact at `100%`; destructor is `97.5%` linked-exact at `80/80` instructions; `drawPauseButton` is `92.7%` linked-exact; `Hud::firePressed` and `addToEventQueue` are byte-exact; `Hud::init` is `64.7%` at `1077/1021`; `checkIfQuickMenuIsEmpty` is `96.4%` at `42/41`; `Hud::draw` is `47.4%`; challenge score is `50.1%`; orbit information is `50.3%` at `225/216`; `touchedElement` is `94.1%` at `444/440`; `touchBegin` is `59.4%` at `65/63`; `drawMenu` is `63.1%` at `225/219`; `catchCargo` is `71.5%` at `470/453`; `drawEventQueue` is `55.6%`; `drawEventString` is `68.3%`; `updateQueue` is `95.3%`; `updateSecondaryWeaponString` is `93.0%` linked-exact; `hudEventMedal` is `84.1%` at `182/182` instructions; `hudEvent` is `66.1%` at `1088/1010`; `initHudMenu` is `27.3%` at `1245/1152`; touchEnd is `92.3%`; getAnalogX/Y are linked- and byte-exact. Large draw/event bodies, the remaining init coordinate lifetime shape and final destructor unwind tail remain. |
| `Hud` latest ARM snapshot | The main-action/target-context and String hidden-return passes restore the native fresh mining predicate, local Canvas boundaries, signed mining-alpha mirror, secondary temporary cleanup graph, direct `Level+0x69` route, dock label stack-slot reuse and explicit progress-label bool constructors. The image-metric/mission-overlay pass confirms the existing kill/passenger/production/cargo String cleanup graph, corrects `PaintCanvas::GetImage2DWidth/Height` to the native signed `int` return contract and restores the ordered-greater volatile-force branch. The quick-menu resource audit corrects mode 1 from the orbit header `0x4f4` to its native `0x4f6`, proves the complete `0x4f3..0x4f6` atlas chain and closes the direct `drawMenu` consumer. Its layout follow-up restores the asymmetric native recurrence: `Layout+0x1dc` remains cached while `Layout+0x30` is reloaded at each row transition, and the equipment String graph again belongs directly to mode 1. The frame-lifetime follow-up restores the native 160-byte dynamically aligned `initHudMenu` frame, stages the iPad fire anchor before platform-offset selection and removes non-native final-loop `Vector` lifetimes. The presentation-shape audit restores the native challenge-row Y recurrence and unsigned bonus centering, reconfirms all five orbit String lifetimes and records the remaining event-line load-order boundary. The dedicated init call-frame pass removes non-native metric lambdas, restores the four-word layout transfer, fresh screen-global reads and the exact steer/copy/fire anchor lifetime. See `HUD_MAIN_ACTION_TARGET_CONTEXT_ARM_2026-08-21.md`, `HUD_STRING_HIDDEN_RETURN_CLEANUP_ARM_2026-08-21.md`, `HUD_IMAGE_METRIC_MISSION_OVERLAY_ARM_2026-08-22.md`, `HUD_INIT_MENU_RESOURCE_CHAIN_ARM_2026-08-22.md`, `HUD_INIT_MENU_LAYOUT_RECURRENCE_ARM_2026-08-22.md`, `HUD_INIT_MENU_FRAME_LIFETIME_ARM_2026-08-23.md`, `HUD_CHALLENGE_ORBIT_PRESENTATION_SHAPE_ARM_2026-08-23.md` and `HUD_INIT_COORDINATE_CALL_FRAME_ARM_2026-08-23.md`; this row supersedes older numeric snapshots in the historical Hud synopsis above. | `Hud::draw` is `50.0%` at `3047/3223`; `initHudMenu` is `44.3%` at `1199/1245`; `drawMenu` is `84.3%` at `221/225`; `Hud::init` is `81.1%` at `1077/1068` with an exact 188-byte stack reservation; challenge score is `49.3%` at `340/350`; orbit information is `56.3%` at `219/225`; event queue is `75.4%`; event string is `90.1%` at `64/67`. All 48 Hud functions average `89.6%`, with 26 linked-exact and 19 byte-exact. `GetImage2DWidth/Height` remain linked- and byte-exact at `9/9` each. The historical ARM snapshot was `201/204` because of the same three unrelated `SolarSystem *` failures; current-source compilation is `204/204`. |
| Hangar / `ModStation` | `HangarList`, `HangarWindow`, `ListItemWindow`, scene-23 assembly and station camera routes received a focused source-backed pass. The package replaces local placeholder globals with typed Status/GameText/Item routes, restores the 64-entry preview mesh tables and direct native camera tables, records the original `BuildResourceList` mesh entry for resource `6801`, and types the 24 native Hangar action slots plus Android row metrics at `Layout+0x238/+0x248/+0x24c/+0x250`. The station routing audit now proves the Android `+0x60..+0x6a` ownership map for radio/main/DLC/Choice, missions/lounge/hangar/StarMap, and status/dialogue/medal modal paths; all touch, update and draw consumers use that same mapping. `StatusWindow`, `SpaceLounge`, and now `HangarWindow` expose their source-backed `int OnTouchEnd(x, y)` ABI directly. Hangar returns nonzero from the confirmed top-level `Layout -> readyToClose()` path and the temporary station-credit close path at `ModStation+0x18`; ordinary detail, tab and internal modal paths retain ownership and return zero. `HangarWindow::OnTouchBegin` now also has its Android `int` ABI and source order for modal credit buttons, list selection, remote-blueprint confirmation and auto-equip routing. The ARM32 class map is now locked at the Android `operator new(0x134)` allocation, including scroll/inertia and list-action offsets; `OnTouchMove` follows the Android modal/detail/list control flow and drag transition. See `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md`, `MODSTATION_SUBWINDOW_ROUTING_ARM_2026-08-23.md`, `STATION_SUBWINDOW_TOUCH_RETURN_ABI_ARM_2026-08-23.md`, `HANGAR_TOUCH_RETURN_ABI_ARM_2026-08-23.md`, `HANGAR_TOUCH_BEGIN_SOURCE_SHAPE_ARM_2026-08-24.md`, `HANGAR_ARM32_LAYOUT_TOUCH_MOVE_2026-08-24.md`, `HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md`, and `HANGAR_CHOICE_TAIL_SOURCE_SHAPE_2026-08-24.md`. | UCRT64 is green. The historical focused snapshot was `201/204` due to three unrelated `SolarSystem *` errors; current-source ARM compilation is `204/204`; `OnRender3D` is 82.1% (`70/64`), `HangarWindow::OnTouchBegin` is 27.2% (`448/390`) and `HangarWindow::OnTouchMove` rises from 11.6% to 78.1% (`141/138`). The large mixed-recovery `ModStation::OnTouchEnd` is 6.4% (`2022/906`), `HangarWindow::OnTouchEnd` is 11.8% (`2154/1968`), `SpaceLounge::OnTouchEnd` is 6.0% (`2713/333`) and `StatusWindow::OnTouchEnd` is 19.2% (`306/236`), with no linked/byte-exact result. Exact preview camera/output shape, remaining String/stack shape in Hangar touch bodies, platform layout branches, the full Hangar behavior body, and ARM byte matching remain separate work. |
| `MGame` touch routing | The `Hud::touchEnd` branch covers pause, primary-fire/hacking/auto-turret/boost actions, camera/maneuver release, quick-menu/orbit actions, and the paused autopilot-menu, StarMap, ordinary ChoiceWindow, `MGame+0xcf`, cargo-conversion `MGame+0xca`, terminal/challenge `MGame+0x1e4`, MenuTouchWindow and dialogue routes. Successful dialogue includes rewards, route/objective cleanup, mission-183 follow-up and all confirmed campaign station/module checkpoints. The shared MenuTouch close tail now restores the three native engine-sound routes, touch reset, light reinit, packed `Globals::options+0x28` particle thresholds, Skip-button producer/consumer flow, campaign `0/1/154/157/158` transitions and cinematic/free-camera cleanup. The related `OnInitialize` fragment keeps `options+0x0f` engine-effect routing separate from the native `>=0.25`/`>0.7` particle-quality thresholds. The free-camera ARM audit proves that its two coordinate arguments are dead: native `freeCamTouchEnd` consumes only `this`, touch ID, and stored deltas, so the non-reloaded `r1/r2` values are not missing logic. See `MGAME_HUD_ACTION_ROUTING_2026-07-20.md`, `MGAME_PAUSED_TOUCH_ROUTING_2026-07-20.md`, `MGAME_ARM_MATCH_2026-07-20.md`, `MGAME_TOUCH_SUCCESS_CARGO_TRANSITIONS_ARM_2026-08-23.md`, `MGAME_MENU_CLOSE_CUTSCENE_PARTICLE_ARM_2026-08-23.md`, and `MENUTOUCH_MAIN_DISPATCH_FREECAM_ARM_2026-08-23.md`. | Focused ARM verification is `8.5%` at `2863/2358` target/base instructions, versus the original 4.3%/1426 baseline and the preceding 9.2%/2169 checkpoint. It is neither linked- nor raw-byte-exact. `freeCamTouchEnd` is `20.0%` at `56/64`; `OnInitialize` is `14.1%` at `771/182`. Whole-function block order/local lifetime remains. |
| `Level` particle setup | The player-engine loop in `Level::initParticleSystems` creates `field_80` systems for `ParticleSet(29 + nozzle)`, records their handles in `field_a8`, and applies confirmed Android scale, position, color, and per-ship atlas UV data to the live `ParticleSettingsRef::cur` table. The companion ABI/runtime audit recovered the paired `cur`/`init` `48 x 0xa0` tables, direct manager/system consumers, baseline scaling/interpolation, and a native smoke test. See `LEVEL_PARTICLE_SYSTEMS_2026-07-21.md` and `PARTICLE_SETTINGS_REF_ABI_RUNTIME_2026-07-21.md`. | Native UCRT64 build and the isolated particle smoke target are green. Original particle-name payloads and full render-path/ARM byte matching remain separate work. |
| Mesh merging | `MeshMerger`, `SimpleMeshMerger`, and `LodMeshMerger` have recovered merge and transform paths. | Full ARM verification remains package-specific. |
| Weapons | `Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`, and related runtime helper work have focused evidence. | Constructors, vtables, and bodies remain under ARM audit. |
| Resource loading | AEM mesh creation/clone paths and the full AEI `ResourceTexture -> ImageCreateFromFile/ImageCreateRegionFromFile -> TextureCreateFromFileIntern` path are source-backed. The AEI passes restore the real `AEimage\0` parser, typed `Image2D`/`AELoadedTexture` ownership, exact atlas triangles, format-specific mip uploads, filters, cubemap faces, cache writes and GL/error accounting. The appended font-table path restores exact UTF-16 set selection, glyph rectangles and per-glyph mesh ownership; 21 real atlases and 23,370 glyph records validate through their final byte. Font runtime covers native advance, direction, baseline/clipping, shader batches, fixed-function matrices and `<c:RRGGBBAA>` segmented color lifetime. See `AEM_AEI_NATIVE_LOADER_SPEC.md`, `AEI_TEXTURE_UPLOAD_ARM_2026-08-27.md`, `AEI_IMAGE_PARSER_ARM_2026-08-27.md`, `AEI_FONT_ATLAS_PARSER_ARM_2026-08-27.md`, `IMAGE_FONT_DRAW_RUNTIME_ARM_2026-08-27.md`, and `DRAW_STRING_COLOR_TAGS_ARM_2026-08-27.md`. | `ImageCreateRegionFromFile` is `81.0%` at `216/221`; `ImageCreateFromFile` is `34.0%` at `295/317`; `ImageCreateFontFromFile` is `57.6%` at `314/314` and `ImageFontRelease` is `88.3%` at `40/37`. Substring width is byte-exact, ordinary width is `97.8%`, `SetWorldViewMatrix` is linked-exact, `FontCreate` is `50.3%`, full draw is `17.1%` at `474/394`, `DrawStringColor` is `93.3%` at `136/132`, and `SplitTags` is `76.3%` at `184/183`. Real GLES payload/reload coverage, multi-line layout, and full draw byte matching remain. |

The touch/key lifecycle follow-up in
`HUD_TOUCH_KEY_LIFECYCLE_ARM_2026-08-23.md` supersedes the Hud aggregate in the
table above: `touchBegin`, `touchMove` and `touchEnd` are now `87.0%`, `97.1%`
and `97.8%`; `closeHudMenu` is `100%` linked-exact. All 48 Hud functions average
`90.7%`, with 27 linked-exact and 19 byte-exact.

The event dispatch/String lifetime follow-up in
`HUD_EVENT_DISPATCH_STRING_LIFETIME_ARM_2026-08-23.md` further supersedes that
aggregate: `hudEvent` is now `76.3%` and `hudEventMedal` is `84.9%`. All 48 Hud
functions average `91.0%`; linked-exact and byte-exact counts remain 27 and 19.

The health/hit Canvas follow-up in
`HUD_HEALTH_HIT_CANVAS_ARM_2026-08-23.md` raises `Hud::draw` from `50.0%` to
`51.4%` at `3223/3025` target/base instructions. It removes non-native gamma
guards and restores native Canvas reload boundaries across the health,
rocket-control, directional-hit and steering cluster. The 48-function average
and exact counts remain `91.0%`, 27 linked-exact and 19 byte-exact.

## Latest Hud Verification

The `HUD_EVENT_PRESENTATION_ARM_2026-08-20.md` and
`HUD_CHALLENGE_ORBIT_LIFETIME_ARM_2026-08-20.md` follow-ups are extended by
`HUD_INIT_MENU_SOURCE_SHAPE_ARM_2026-08-21.md`, followed by
`HUD_INIT_MENU_RESOURCE_CHAIN_ARM_2026-08-22.md` and
`HUD_INIT_MENU_LAYOUT_RECURRENCE_ARM_2026-08-22.md`, then
`HUD_INIT_MENU_FRAME_LIFETIME_ARM_2026-08-23.md`,
`HUD_CHALLENGE_ORBIT_PRESENTATION_SHAPE_ARM_2026-08-23.md`, and
`HUD_TOUCH_KEY_LIFECYCLE_ARM_2026-08-23.md`, followed by
`HUD_EVENT_DISPATCH_STRING_LIFETIME_ARM_2026-08-23.md` and
`HUD_HEALTH_HIT_CANVAS_ARM_2026-08-23.md`, and supersede the older inline Hud
figures above. The 48-function Hud set now averages `91.0%`, with 27
linked-exact and 19 byte-exact functions. `Hud::draw` is now `51.4%` at
`3223/3025` target/base instructions. Other current focused scores include
`drawEventQueue` 75.4%, `drawEventString` 90.1%, `drawChallengeModeScore`
49.3%, `drawOrbitInformation` 56.3%, and `initHudMenu` 44.3% at `1245/1199`
target/base instructions. The menu pass restores post-state cloak/jump action
insertion, the paired command action table, signed docking actions and native
menu-count reloads. The resource follow-up corrects the mode-1 header to
`0x4f6` and records all four header atlas entries; `drawMenu` currently reaches
`84.3%` at `225/221` target/base instructions. The layout recurrence follow-up
restores per-row `Layout+0x30` loads while retaining only `Layout+0x1dc`. The
frame-lifetime pass then restores the Android 160-byte dynamic alignment,
iPad anchor operand order and post-translate button reloads. The remaining
mode-1/mode-2 shared String/action area is still allocated at `sp+0x70` rather
than the native `sp+0x40`, so no byte-match is claimed.

The touch/key follow-up restores the original moved-touch Y coordinate and the
native key-slot cleanup lifecycle. Its focused scores are `87.0%` for
`touchBegin`, `97.1%` for `touchMove`, `97.8%` for `touchEnd`, and linked-exact
`100%` for `closeHudMenu`.

The event dispatch follow-up restores the complete seven-String cargo-loss
expressions for events 30 and 47, types the packed dock-transfer state and
places medal percentage clamping inside the native String chain. `hudEvent`
reaches `76.3%` and `hudEventMedal` reaches `84.9%`.

The 2026-08-31 whole-family audit in `HUD_ARM_SOURCE_SHAPE_99_2026-08-31.md`
supersedes every numeric Hud snapshot above. Across all 48 native Hud symbols,
the allocation-insensitive source-shape average is now `99.24%`; the independent
family averages are `98.75%` instruction-shape inventory, `99.35%` opcode
inventory, `99.21%` ordered control flow and `99.65%` instruction-count
agreement. The strict ordered fuzzy average remains `95.61%`: 36 functions are
strict-fuzzy `100%`, 32 are linked-exact and 20 are raw byte-exact. This is a
verified source-shape milestone, not a claim that `Hud::draw`, `initHudMenu`,
`init`, challenge/orbit presentation, or the whole class are byte-matched.

## Latest Hangar Verification

The `HANGAR_TOUCH_END_ECONOMY_SHIP_SWAP_2026-08-24.md` follow-up extends the
previous layout and input passes with the exact Android social-credit reward
table, blueprint cancellation route, and the two-stage ship-swap/DLC/trade-in
flow. The 2026-08-24 focused pass was `201/204` because of three unrelated
`SolarSystem *` compile errors. `HangarWindow::OnTouchEnd` rises from `6.0%`
at `2154/1487` target/base instructions to `6.4%` at `2154/1776`.
`OnTouchBegin`, `OnTouchMove`, and `update` remain respectively `27.2%`,
`78.1%`, and `48.3%`. No Hangar method is claimed linked- or byte-exact.

The following `HANGAR_TOUCH_END_LIST_DIALOG_SOURCE_SHAPE_2026-08-24.md` pass
restores Android's continued list-action walk and scoped help-text String
branches. `HangarWindow::OnTouchEnd` rises to `11.2%` at `2154/1871`
target/base instructions. The shared modal application-module close flag is
now typed as `ModStation+0x18`, `closeHangarAfterCredits`. The
`HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md` correction proves its
station-launched paid-credit close route also returns nonzero to the owner;
the focused score stays `11.2%` at `2154/1879`.

`HANGAR_CHOICE_TAIL_SOURCE_SHAPE_2026-08-24.md` then restores the direct
Android `LABEL_117` convergence for replace-equipment, credit and sell-ship
dialogs. `HangarWindow::OnTouchEnd` reaches `11.8%` at `2154/1968`; no
linked- or raw-byte-exact result is claimed.

`HANGAR_SET_SELL_MODE_STORE_BLUEPRINT_2026-08-24.md` replaces the remaining
shim-based `HangarWindow::setSellMode` Store/Blueprint body with the Android
inventory rebuild, fabrication, pending-product, route-warning and
auto-equip flows. It also confirms that the second ship-swap cancellation and
both ship-purchase error notices clear the pending-swap word at `+0x90`.
Focused ARM validation measures `setSellMode` at `13.5%` (`708/643`) and
keeps `OnTouchEnd` at `11.8%` (`2154/1973`); both remain source-backed, not
linked- or byte-exact.

## Latest MGame Verification

The 2026-09-23 MGame gate in
`MGAME_INIT_STARMAP_SOURCE_SHAPE_2026-09-21.md` compares all 45 native MGame
symbols at `99.04%` average source-shape and `90.81%` strict fuzzy. Sixteen
functions are linked-exact and nine are byte-exact. `OnTouchEnd` and
`OnUpdate` reach `94.9%` and `96.7%` source-shape, but remain only `38.3%`
and `58.3%` strict; no byte-match is claimed for either large body.

The matching full-corpus run builds all 209 ARM objects without failures and
compares 4844 functions at `88.74%` source-shape / `75.50%` strict, with 2100
linked-exact and 975 byte-exact functions. UCRT64 `libgof2.a` also links.

## Priorities

1. Recover large behavior bodies only with an evidence packet and uncertainty list.
2. Audit shared layouts before cosmetic per-function rewrites.
3. Continue `Radar::draw` source-shape work without regressing exact functions.
4. Recover `Hud`, `Globals`, `ModStation`, `MGame`, mission, economy, AI, and
   platform helpers as separate packages.
5. Keep public documentation synchronized and do not import original game data.

## Evidence Terms

- **Source-backed**: tied to a named native, iOS, Java, symbol, or directly
  equivalent call-path source.
- **Confirmed**: cross-checked independently or supported by a matching result.
- **Needs confirmation**: plausible mapping awaiting stronger evidence.
- **Heuristic**: practical approximation with no claim of original behavior.

The original Android binary is required only for local verification and is
ignored in `_work/bins/`. Do not add proprietary binaries, game assets, APKs,
IPAs, OBBs, JARs, or extraction dumps.
