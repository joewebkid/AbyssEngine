# WantedWindow hit-test layout source (2026-10-25)

Android 2.0.16 ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `WantedWindow::getWantedAtPosition(int,int)` at `0xe1fc0`.
At `0xe1fdc..0xe1fee` it loads `Globals::layout`, then reads layout
offsets `+0x0c`, `+0x20`, `+0x5c`, `+0x34`, and `+0x70` to compute the row.
The recovered source instead dereferenced a file-local pointer initialized
to null. The function now reads the confirmed global layout; its callers
and arithmetic are otherwise unchanged.

Focused ARM source-shape/strict fuzzy moves from **15.9%/13.3%** to
**100%/100%**. The function is newly **linked-exact**, not byte-exact.
Native `gof2` and **209/209** ARM translation units build. A source-extracted
host check runs at `-O0/-O2` with first/last row, right-half, scrolled,
out-of-range and empty-list cases.

The full ARM corpus compares **4,323/4,524** original functions over
**4,741** pairs. Mean source-shape/strict fuzzy rises from
**94.63%/84.15%** to **94.65%/84.18%**. Linked-exact rises from
**2,597** to **2,598**; byte-exact stays at **1,082**. Symbol-set
comparison shows zero linked-exact or byte-exact losses. The verifier
suite passes **95** tests with one skip.

The original unsigned bound wraps when the wanted list is empty: the
hit-test may return index zero. This pass preserves that observed behavior;
callers must not index an empty list. The host check substitutes the
`Layout`, `Globals`, `WantedWindow`, and `Array` structures, so it does not
prove a complete screen or touch sequence. `OnTouchBegin`, `OnTouchMove`,
`OnTouchEnd`, draw and initialization still contain separate unresolved
global pointer slots; these are the next bounded recovery priorities.

Local reports: `_work/config_reader_getnewline_full_20261025.json` (before),
`_work/wanted_hit_after_20261025.json`, and `_work/wanted_hit_full_20261025.json`
(after; ignored by Git).
