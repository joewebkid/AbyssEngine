# LensFlare constructor canvas binding (2026-10-14)

Android `LensFlare::LensFlare(PaintCanvas *)` at `0x11d00c` allocates
three image handles and loads resources **1288, 1289, 1290** through
`Globals::Canvas`. It obtains width and height from the canvas passed
to the constructor, then retains that pointer. The former C++ instead
dereferenced a null translation-unit canvas pointer and called three
declared-only helper functions. The constructor now calls the existing
`PaintCanvas` methods directly; those unused declarations are removed.

The host smoke extracts the production constructor and destructor
and uses distinct resource and input canvas fixtures. It checks all
three IDs and handles, dimensions, and retained pointer. Native
`gof2`, 206/206 ARM translation units, and 37 verifier tests (one
skipped) pass. Both constructor aliases improve from **91.0%**
source-shape / **46.2%** strict fuzzy to **100%/100%** and become
linked-exact, but not byte-exact.

The full report `_work/lens_flare_ctor_full_20261014.json` compares
**4,323/4,524** original functions at **93.29%** average source-shape
and **82.20%** strict fuzzy, with **2,482 linked-exact**, **1,065
byte-exact**, **123 extras**, and zero skipped units. Against
`_work/system_pathfinder_full_20261014.json`, the exact-pair sets
show zero losses, two linked-exact gains (C1/C2), no byte-exact gains,
and no extra-symbol changes.

The test does not exercise real GL resources. `LensFlare::render2D`
still has a null canvas alias, declared-only drawing helpers and
substantial geometry/color differences from the Android body at
`0x11d088`. Its focused source-shape/strict fuzzy remains
**64.2%/15.5%**; live rendering, complete executable linking and
ARM32 runtime are unverified. Recover `render2D` as the next bounded
LensFlare package.
