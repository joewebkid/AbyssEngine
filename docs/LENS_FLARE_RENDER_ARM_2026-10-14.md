# LensFlare render2D recovery (2026-10-14)

Android `LensFlare::render2D(float,float,float,int)` at `0x11d088`
uses color tables `dword_201E40/201E20/201E00`, accepts only negative
depth and points strictly inside `(-width, 2*width) × (-height,
2*height)`, then draws seven image passes using the three handles
loaded by the constructor. The original body reads the shared
`Globals::Canvas` for all image operations, computes distance with
`Globals::sqrt`, and stores its fade in `LensFlare::intensity`.

The recovered source previously dereferenced a null canvas alias,
called declared-only `LensFlare_*` helpers, scaled image *handles*
as though they were widths, and used unrelated color/state operations.
It now calls existing `PaintCanvas` overloads with the Android pass
order: image 0 at half displacement (unscaled), image 0 at quarter
(0.75 scale), image 1 at three quarters (0.5 scale), conditional
image 0 at one eighth (1.25 scale), image 1 at one eleventh (0.5
scale), then image 2 at negative three quarters (2.0 scale) and
negative one fifth (0.5 scale). Each size comes from
`GetImage2DWidth`, as in the Android dump. Positive/NaN depth and
out-of-range/NaN coordinates now take the original ordered early
return. Positive fade saves the *retained input canvas* color, fills
its full rectangle, then restores that color; image passes use the
shared canvas. All declared-only `LensFlare_*` calls are removed.

The existing LensFlare host smoke now extracts the production render
body. It checks constructor resource routing, seven centered draw
passes and alpha values, a displaced white flare, ordered no-draw
guards, and final color save/restore on a distinct input canvas.
Native `gof2`, 206/206 ARM translation units and 37 verifier tests
(one skipped) pass. Focused source-shape/strict fuzzy improves from
**64.2%/15.5% → 94.8%/48.7%**; `render2D` is neither linked- nor
byte-exact.

The full report `_work/lens_flare_render_full_20261014.json` compares
**4,323/4,524** original functions, average source-shape **93.29%**,
strict fuzzy **82.21%**, **2,482 linked-exact**, **1,065 byte-exact**,
**123 extras**, and zero skipped units. Its exact-pair and extra-symbol
sets are unchanged from `_work/lens_flare_ctor_full_20261014.json`.

No real textures, GL context, camera projection or ARM32 execution
was exercised. Float-to-byte alpha conversion at distant in-bounds
distances and compiler-specific assembly shape still need validation.
Next: verify a live StarSystem-to-LensFlare frame on an ARM32 or
faithful graphics harness, then investigate the remaining render
instruction-shape difference without changing confirmed behavior.
