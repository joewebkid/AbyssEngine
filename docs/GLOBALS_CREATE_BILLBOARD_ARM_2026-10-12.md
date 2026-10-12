# Globals createBillBoard — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 function at ELF address `0xe3e08` creates a mesh through
`Globals::Canvas` with 12 vertices, six triangles, mesh type 19, and the
low 16 bits of its final argument as the material resource ID. It assigns
the four `(u0, v0)`, `(u0, v1)`, `(u1, v0)`, `(u1, v1)` UV corners to each
of three planes, then builds vertices from the first two integer extents.
The only in-tree caller, `MovingStars`, passes `70`, `500`, and `0x4e54`
for those extents and material ID.

The recovered function dereferenced null canvas/guard placeholders,
passed material ID zero, miswired UV pairs, and treated the material ID as
a geometric extent. It now follows the ARM-confirmed calls and uses names
in the declaration that reflect the recovered parameter roles. The six
triangle indices and returned mesh handle were already correct and remain
unchanged.

## Verification and limits

- Native `gof2` static-library build: pass, including the `MovingStars`
  caller after the declaration update.
- ARM build: 206/206 translation units.
- Verifier suite: 37 run, 36 passed, one skipped.
- Focused `Globals` comparison: 49 functions. `createBillBoard` rose from
  83.8% to 100% source-shape and from 14.6% to 100% strict fuzzy, adding
  one linked-exact match (unit total 12 to 13). It is not byte-exact.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Average source-shape remains
  92.35% after rounding, strict fuzzy rises 80.52% to 80.54%, and
  linked-exact rises 2,355 to 2,356; byte-exact remains 1,008. A
  per-symbol comparison found only `createBillBoard` changed and no
  exact-pair losses. Report:
  `A:/Galaxy on Fire new version/_work/globals_billboard_full_20261012.json`.

No live starfield rendering run was performed; the native target is a
static library. Next priority: `getWreckCollision`, which still begins
with a null guard placeholder and is called by `PlayerFixedObject`.
