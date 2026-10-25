# Wanted scrollbar fractions (2026-10-25)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow::getRelativeScrollStartPos` at `0xe17a8..0xe17d2`
returns `0.0f` when `scrollOffset > 0`. The height method at
`0xe17d8..0xe1824` returns `0.0f` when `visibleHeight > contentHeight`.
Both facts are visible in the original ARM `VLDR S0, =0.0` branches.

The recovered C++ instead returned floats made from literal bit
patterns `0x4650a903` and `0x4605e009` on those branches. Both now
return zero, matching the ARM behavior and the corresponding
`StatusWindow` scrollbar rules. Focused source-shape/strict fuzzy for
the start method remains **100%/100%**, and it becomes **byte-exact**.
The height method remains **84.8%/57.1%**, nonexact; this patch changes
its return value without changing the comparison score.

Native `gof2`, the AArch64 static archive, and **209/209** ARMv7
translation units build. The source-extracted scrollbar smoke passes
positive overscroll, middle, past-bottom and short-content cases at
host `-O0/-O2`. Live Android Wanted-screen rendering and scrolling
remain unverified. The next Wanted-specific gate is an interactive
screen run with the recovered renderer and actual input.

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** pairs with zero skips. Mean source-shape/strict fuzzy stay
**94.71%/84.21%**. Linked-exact increases **2,600 → 2,601** and
byte-exact **1,082 → 1,083**; the start-position method is the only
gain, and neither exact set loses a symbol.
