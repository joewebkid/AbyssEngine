# Globals canvas pointer width — Android ARM and native (2026-10-11)

## Evidence and correction

The original Android `Globals::init` at `0xe3618` checks whether
`Globals::Canvas` is null and, if so, copies the first 32-bit word of
the `ApplicationManager` object. In the recovered class this first field
is `paintCanvas`. The C++ implementation previously cast the address of
the global to `int *` and copied an `int` from the application manager.
That matches 32-bit ARM width but truncates a pointer in the native
64-bit build. The code now checks `Globals::Canvas` and assigns
`app->paintCanvas` as a typed pointer. The original null-preserving branch
is retained.

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier tests: 36 run, 35 passed, one skipped.
- Focused `Globals` family: 49 comparisons, no source-shape or exact-pair
  changes; `Globals::init` remains 86.5% source-shape and 75.9% strict fuzzy.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; 92.32%
  source-shape, 80.46% strict fuzzy, 2,353 linked-exact, 1,008 byte-exact.
  Per-symbol comparison to the preceding full report found zero shape
  changes and zero linked-exact or byte-exact losses. Current report:
  `A:/Galaxy on Fire new version/_work/globals_canvas_pointer_full_20261011.json`.

## Remaining limit and next priority

The ARM corpus cannot prove the native 64-bit runtime path, and the
full executable still has unresolved references. No live canvas rendering
was run. Other native pointer-to-`int` casts in `Globals.cpp`, notably
the station-index calls, remain a separate ABI audit priority.
