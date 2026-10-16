# Trail point-update and width behavior (Android ARM)

Primary reference: `android_2.0.16_libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x15c184`, `Trail::setWidth(int)` subtracts the new width from
the old width, adds that difference to each vertex's integer X value,
calls `PaintCanvas::MeshSetPoint` for each of `2 * (segments + 1)`
vertices, then stores the new width. The original `Trail::update`
at `0x15bfac` writes the two endpoint pairs into the first twelve
integers, then at `0x15c07c` shifts six-integer pairs toward the end
of the point array. This ordering is observable: with three pairs,
the final order is the first new pair, a copy of that pair, and the
second new pair. It then computes integer positions relative to the
first endpoint, sends the relative vertices to `MeshSetPoint`, and
sets the local transform's translation to that endpoint.

The existing production C++ already follows this ordering; no runtime
source was changed. The expanded extracted-body fixture in
`tools/trail_canvas_calls_smoke.cpp` verifies positive and negative
width differences across six vertices, all mesh point arguments,
the complete point and relative-point arrays for fractional and
negative coordinates, and the local matrix translation. Its first
expectation deliberately failed because it assumed the endpoint
copy occurred before the stores; the ARM instructions resolved that
mistake. The corrected oracle passes under host `-O0` and `-O2`.

Native `gof2`, 207/207 ARM translation units, and 40 verifier tests
(one existing skip) pass. Focused Trail verification remains
**98.76% source-shape / 88.72% strict fuzzy**, **7 linked-exact /
1 byte-exact** among twelve functions. Because no production C++
changed, the preceding full report remains applicable: **4,323/4,524**
compared, **93.66%/82.73%** average source-shape/strict fuzzy,
**2,509 linked-exact / 1,074 byte-exact**, with no exact-set changes.
Reports: `A:/Galaxy on Fire new version/_work/trail_semantics_focused_20261016.json`
and `A:/Galaxy on Fire new version/_work/trail_canvas_full_20261016.json`.

The Trail constructor now has a separate bounded fixture in
[the constructor audit](TRAIL_CONSTRUCTOR_SEMANTICS_ARM_2026-10-16.md).
This point-update fixture does not exercise GPU rendering, live ARM32
execution, or unusual signed-overflow and strict-aliasing cases in
the point-copy implementation. The remaining fuzzy mismatch
in these functions is not evidence of a behavior error by itself.
