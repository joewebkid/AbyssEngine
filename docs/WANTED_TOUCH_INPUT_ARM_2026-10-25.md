# WantedWindow touch input source recovery (2026-10-25)

Primary reference: Android 2.0.16 ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow::OnTouchBegin` at `0xe2104` loads `Globals::layout` at
`0xe215a..0xe2166`. `WantedWindow::OnTouchMove` at `0xe21a0` loads
`Globals::layout`, `Globals::h`, `Globals::w`, and `Globals::mouse_wheel`
at `0xe21be..0xe21fa`; it reloads `Globals::w` at `0xe2210..0xe2220`
and the layout for forwarding at `0xe2248..0xe2250`. These are observed
data references and calls in the original assembly, not field-name guesses.

The reconstructed functions dereferenced six null-initialized file-local
pointer slots for those values. They now use the corresponding `Globals`
members directly. The remaining touch routing and arithmetic are unchanged.

Focused ARM comparison (source-shape / strict fuzzy):

| Function | Before | After | Exact |
| --- | ---: | ---: | --- |
| `OnTouchBegin` | 96.7% / 95.8% | 100% / 100% | newly linked-exact, not byte-exact |
| `OnTouchMove` | 13.5% / 5.3% | 99.0% / 90.5% | nonexact |

Native `gof2` and **209/209** ARM translation units build. The verifier
suite passes **95** tests with one skip. A source-extracted host check runs
the production `OnTouchBegin`, `OnTouchMove`, and `getWantedAtPosition`
bodies at `-O0/-O2`, covering map forwarding, list touch routing,
scroll/drag and mouse-wheel behavior, right-side forwarding, active and
inactive detail buttons, and highlight cancellation after dragging.

The full ARM corpus compares **4,323/4,524** original functions over
**4,741** pairs. Mean source-shape/strict fuzzy rises from
**94.65%/84.18%** to **94.67%/84.20%**. Linked-exact rises from
**2,598** to **2,599**; byte-exact stays at **1,082**. Direct symbol-set
comparison shows zero linked-exact or byte-exact losses.

The host check substitutes UI objects and does not establish a complete
live game touch sequence. Neither function protects against an invalid
`selectedWanted` when a detail button exists; that indexing is present in
the original and requires caller-state validation. `OnTouchEnd`, draw,
and initialization still contain separate unresolved pointer slots.

Local reports: `_work/wanted_touch_before_20261025.json`,
`_work/wanted_touch_after_20261025.json`, and
`_work/wanted_touch_full_20261025.json` (ignored by Git).
