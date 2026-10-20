# Pan stage-one asteroid-field return (2026-10-20)

## Scope and evidence

`tools/run_level_stage1_asteroid_field_pan_gate.py` extends the continuous
original-asset Pan `Level::init` scenario from the first complete asteroid
through the rest of production `Level::createAsteroids` in
`src/game/world/Level.cpp`. The test takes the same function body from the
repository and stops at the next `Level::createGasClouds` call. Its replacement
for that next service throws a boundary exception; it is not a gas-cloud
implementation. The Android ARMv7 binary remains the primary source for
the recovered code, as documented in the preceding asteroid and Explosion
notes.

The original Pan assets are supplied locally, not stored in Git. The Galaxy
probability service remains a bounded fixture returning kind `0x9a` at 100%.
Thus the gate verifies the complete field for that kind, not the full Galaxy
probability algorithm or every asteroid mesh variant. `AERandom::reset()`
uses the current time; the density of near objects is checked as a contiguous
group of 2–9 rather than asserted as one deterministic value.

## Checks and result

- Windows x64 SwiftShader and a physical Android ARMv7 device pass at both
  `-O0` and `-O2` against the original Pan assets.
- Production `createAsteroids` returns before the boundary fires; the array
  has its expected station-seeded 40–79 slots, all populated, and the LOD
  manager contains the two preexisting objects plus one per asteroid.
- For every asteroid the gate checks level/player/geometry linkage, kind,
  quality, mineability, rotation, near/far scale-derived radius, center-relative
  position and first LOD threshold. The first object's stronger explosion,
  health, mesh and transform assertions from the previous gate remain active.
- The Pan stage-zero file-lifetime and previous player assertions still pass.
  No new production C++ was changed. The previous revision's green native
  build, 208/208 ARM translation units and full verifier therefore remain
  applicable: 4,323/4,524 originals, 4,735 pairs, 93.96% source-shape,
  83.24% strict fuzzy, 2,551 linked-exact and 1,079 byte-exact, zero skipped.

For reproduction, run `tools/run_level_stage1_asteroid_field_pan_gate.py`
with `--asset-root` pointing at the lawfully obtained original assets and
`--out-dir` pointing at a disposable local directory. On Windows pass
`--angle-dir` pointing at SDK `gles_swiftshader` and `--egl-include` pointing
at the NDK sysroot include directory. On Android pass `--ndk`, `--adb` and
`--serial` for the connected device. The runner performs both optimization
levels. The host's other ANGLE backend did not advertise ETC1 in this run;
SwiftShader was required for the asset gate.

## Remaining boundary

This does not establish `createGasClouds`, the rest of `Level::init` stage one,
a playable frame, or asteroid update/collision/render. The probability
distribution, alternate asteroid kinds, and cleanup of the whole level still
need their own evidence and tests. The next narrow milestone is crossing
production `createGasClouds` and returning from stage one without broadening
the probability fixture claim.
