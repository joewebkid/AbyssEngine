# HackingGame dimension queries during rendering (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `HackingGame::render2D` at `0x14f1e4` queries image
dimensions again before drawing the mirrored top panel, mirrored
bottom panel and second target mark. The reconstruction reused the
values from their first draw. It now expresses the six additional
queries in the original order: main and top image heights, main
image height, then mark width, main image height and mark height.

Focused ARMv7 verification builds 209/209 translation units and
compares 18 `HackingGame` symbols. `render2D` source-shape improves
from 79.4% to 80.3%; strict fuzzy rises from 14.8% to 16.1% and the
generated instruction count from 490 to 532 against 685 original.
The generated call-count metric falls from 87.2% to 82.1%, so the
remaining call sequence needs further work. The other 17 functions
in this unit retain their metrics, including 13 linked-exact and 10
byte-exact comparisons. Focused report:
`_work/hacking_render_requery_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The screen has
not been checked on a permitted device; visual behavior remains
unverified. Redmi testing stays stopped.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Rounded means stay 96.65%
source-shape and 86.60% strict fuzzy; linked-exact stays 2,778 and
byte-exact stays 1,107. No comparison pair or exact match was lost.
Full report: `_work/hacking_render_requery_full_20261103.json`.

Next priority: compare the remaining dimension and draw-call order
in `render2D` against ARM, then run the screen on a permitted device.
