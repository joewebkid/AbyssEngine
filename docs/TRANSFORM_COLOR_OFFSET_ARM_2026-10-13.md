# Transform color ABI slot (2026-10-13)

Android ARMv7 initializes the transform's packed color to `-1` at
offset `0x48` in both `Transform` constructors (`0x746a0`, `0x74918`).
`Transform::InternUpdate` (`0x75048`) writes its alpha-derived packed
color to the same offset, and `PaintCanvas::TransformSetColor`
(`0x7b410`) writes the same slot. The local `Transform` layout called
`0x48` `id` and called the unrelated `0xf4` slot `color`. Consequently,
both constructors and animation update wrote to `0xf4`, while draw
and world-fog code read or wrote `0x48`.

The field at `0x48` is now named `color`, with its ARM offset asserted;
the unconfirmed `0xf4` slot is neutral `field_0xf4`. Existing render
and fog callers use the corrected name. This changes the writes in
the two constructors and animation update without changing the ABI
layout or the already-correct raw-offset canvas setter.

Native `gof2`, **206/206** ARM translation units, 37 verifier tests
(one skipped), and the recursive canvas draw, canvas wrapper, and
transform release host smokes pass. The focused five-symbol family
remains **85.6% source-shape**; strict fuzzy moves from **39.8%** to
**39.7%**. This package is an ABI/behavior correction, not a match
optimization. The full corpus remains **4,718** comparisons,
**4,323/4,524** covered, **201** missing, **92.5982%** average
source-shape, **2,381 linked-exact**, **1,008 byte-exact**, and 124
extra symbols. Strict fuzzy moves from **80.9288%** to **80.9287%**;
neither prior exact row set loses a member.

Evidence: Android decompiler output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
focused baseline
`A:/Galaxy on Fire new version/_work/transform_color_abi_baseline_20261013.json`,
focused final
`A:/Galaxy on Fire new version/_work/transform_color_abi_focus_20261013.json`,
and full final
`A:/Galaxy on Fire new version/_work/transform_color_abi_full_20261013.json`.
The actual purpose of `0xf4`, full non-null clone ownership, real
rendering output, and ARM32 runtime remain unverified. Next audit the
non-null clone's shared keyframes and child-mesh lifetimes.
