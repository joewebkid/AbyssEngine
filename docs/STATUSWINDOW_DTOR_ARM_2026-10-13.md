# StatusWindow destructor ownership — Android ARM audit (2026-10-13)

Scope: `StatusWindow::~StatusWindow()` at Android 2.0.16 ELF address
`0x158164` (`D1`/`D2` aliases). The primary source is
`analysis/gof2_libgof2hdaa_full_ida.c:202727-202773`, cross-checked
against the original ARM instructions.

Android calls `ArrayReleaseClasses` for each non-null tab-button,
medal-button, image-part and detail-line array, then destroys the array
and clears its field. The local destructor called `ArrayRemoveAll` between
release and destruction for all four arrays. In the local `Array` helper,
`ArrayReleaseClasses` already frees the contents and backing storage;
the extra call allocated new storage only to destroy it immediately.
Those four calls are removed. An audit of `getRelativeScrollHeight`
found its branches already match the Android source, so no metric-only
scroll change was made.

A production-destructor host smoke checks that owned objects and all
four arrays are destroyed once, including a null-field case. The
existing native ABI/lifetime smoke also passes, as do native `gof2`, all
206 ARM translation units and 37 verifier tests (one skipped). The
fixture uses tracked stand-ins, not the original ARM32 allocator.

Both destructor aliases rise from 86.6% to 100.0% source-shape and from
90.9% to 100.0% strict fuzzy. Both become linked-exact, but neither is
raw-byte-exact. The `StatusWindow` unit now has 21 comparisons rather
than 22: it no longer emits its unused non-exact
`ArrayRemoveAll<String*>` instantiation. The remaining unit family
averages 97.1% source-shape and 78.8% strict fuzzy, with nine
linked-exact and one byte-exact symbol.

Full ARM corpus: 4,719 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.85% average source-shape and 81.23% strict
fuzzy at report precision, 2,397 linked-exact and 1,008 byte-exact.
Complete `(unit,symbol)` exact-set comparison against the previous
package shows two gains and zero losses. The extra-symbol set remains
123. The report is
`A:/Galaxy on Fire new version/_work/status_dtor_full_20261013.json`.
Actual `StatusWindow` destruction under ARM32, real assets and full UI
lifecycle remain unverified. Next priority: remaining non-exact blocks
of `StatusWindow::draw` (84.6% source-shape, 2,324 original ARM
instructions), approached in bounded source-backed sections.
