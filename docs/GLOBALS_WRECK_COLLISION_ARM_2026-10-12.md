# Globals getWreckCollision — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 function at ELF address `0xe4bb8` loads variable-length
wreck collision records through `FileRead::loadWreckCollision`. Record type
0 occupies five integers and creates a sphere; type 1 occupies seven and
creates an axis-aligned box. Unknown types consume one integer and retain
the output array's null slot. The only in-tree caller is
`PlayerFixedObject`, which passes a wreck type and geometry pointer.

Direct ARM disassembly confirms that the first integer argument (`kind`),
not the geometry pointer, controls a 2.0 scale when zero. It also confirms
the 0.6 sphere-radius and 1.1 box-size factors. Both shapes map the record
coordinates to `(-record[1], record[3], record[2])`; the box size maps to
`(record[4], record[6], record[5])`. The original calls the constructors
with a zero base position and these values as extents. A sphere takes the
scaled absolute radius; a box takes twice each scaled size component.

The previous reconstruction dereferenced a null guard placeholder, scaled
by record data and the wrong parameter, reversed sphere constructor roles,
and used a six-argument untyped AAB constructor. The replacement uses the
typed constructors and the original `ArrayRelease<int>` cleanup path.

## Verification and limits

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 50 functions. `getWreckCollision` rose
  from 90.8% to 97.2% source-shape and from 41.6% to 56.7% strict fuzzy.
  It is neither linked-exact nor byte-exact.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Average source-shape remains
  92.35% after rounding; strict fuzzy rises 80.54% to 80.55%, linked-exact
  rises 2,356 to 2,357, and byte-exact stays 1,008. The additional
  linked-exact match is `ArrayRelease<int>`; no previous exact pair is lost.
  Report: `A:/Galaxy on Fire new version/_work/globals_wreck_collision_full_20261012.json`.

No live wreck collision run was performed; the native target is a static
library. The remaining machine-code gap in `getWreckCollision` should not
be closed by changing the ARM-confirmed behavior to chase a metric.
