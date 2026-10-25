# WantedWindow touch-end source recovery (2026-10-25)

Primary reference: Android 2.0.16 ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow::OnTouchEnd(int,int)` begins at `0xe22ac`. Its map-return
branch reads `Globals::iPad`, `iPadHD`, `iPadLarge`, `w`, and `h`
(`0xe22e0..0xe2508`). The ordinary branch reads `Globals::appManager`
and `galaxy` for map navigation (`0xe23c4..0xe23f0`), and
`Globals::layout` and `gameText` for closing and help
(`0xe2460..0xe24b0`, `0xe25c0..0xe25ce`). The help path constructs a
`String` directly from `GameText::getText(0x27b)`.

The recovered source previously dereferenced ten null-initialized
file-local pointer slots in this function and used separate manager and
galaxy aliases. It now reads the observed `Globals` members and uses the
observed string constructor. Gesture, selection, and map-control branches
remain in their existing order.

Focused ARM source-shape/strict fuzzy is **82.6%/51.5%** before and
**93.7%/61.2%** after; the function remains nonexact. Native `gof2`,
**209/209** ARM translation units, and **95** verifier tests (one skip)
pass. A source-extracted host check runs at `-O0/-O2` and covers map
return, all four screen-size variants, scrolling and button dispatch,
help, and reopening the map through an active Wanted entry.
Both reuse of an existing map and construction of a new map are exercised.

The full ARM corpus remains at **4,323/4,524** original functions over
**4,741** pairs, **94.67%/84.20%** mean source-shape/strict fuzzy,
**2,599 linked-exact**, and **1,082 byte-exact**. Direct comparison of
the linked-exact and byte-exact symbol sets shows no losses or gains;
the focused improvement is below the full-corpus rounding precision.

The fixture substitutes UI, application, world, and text objects. It
does not prove live game startup or the complete Wanted screen.
`selectedWanted`/`highlightedWanted` validity and the ownership of a
newly created map still need runtime validation. `draw` and `init`
contain separate unresolved pointer slots and are the next priorities.
The currently connected Android 16 handset is 64-bit-only, so it cannot
install the original `armeabi-v7a` APK for a live screen check.

Local verifier reports: `_work/wanted_end_before_20261025.json`,
`_work/wanted_end_after_ctor_20261025.json`, and
`_work/wanted_end_full_20261025.json` (not committed).
