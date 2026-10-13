# StatusWindow constructor globals — Android ARM audit (2026-10-13)

Scope: `StatusWindow::StatusWindow()` at Android 2.0.16 ELF address
`0x157d80` (`C1` and `C2` aliases). The primary source is
`analysis/gof2_libgof2hdaa_full_ida.c:202553-202660`, checked against
the original ARM instructions in the focused verifier.

Five unassigned file-scope holders previously supplied text, layout,
screen width, active tab and screen height. The constructor dereferenced
them and could crash before drawing. Android instead reads
`Globals::gameText`, `Globals::layout`, `Globals::w`, `Globals::iPad` and
`Globals::h`. It also reads medals through `Globals::achievements` and
creates the three images through `Globals::Canvas`.

The second tab is constructed at its final X coordinate using the
*first* tab's width; the former code created it at zero and then used
its own width. The iPad branch, rather than the phone branch, adds the
header/padding to medal content height. Text IDs 168, 577 and 1507–1551,
medal count 45 and image IDs `0x48e`, `0x48f`, `0x48d` follow the Android
constructor. A production-constructor host smoke checks both device
branches, tab placement, all medals, images and dimensions with fixture
services. It is not an original-asset or ARM32 runtime test.

Native `gof2`, all 206 ARM translation units and 37 verifier tests pass
(one skipped). Focused constructor source-shape rises from 38.5% to
96.0% for both aliases; strict fuzzy rises from 16.6% to 53.1%. Each
alias has 251 local versus 242 original ARM instructions and remains
non-exact. The 22-row `StatusWindow` family averages 95.96% source-shape
and 77.73% strict fuzzy, with seven linked-exact and one byte-exact
symbol.

The full ARM corpus has 4,720 comparisons, 4,323/4,524 original
functions covered, 201 missing, 92.85% average source-shape and 81.23%
strict fuzzy. Linked-exact remains 2,395 and byte-exact 1,008. Complete
`(unit,symbol)` exact-set comparisons against the previous medal-grid
package show zero losses and zero gains; the 123 extra-symbol set is
unchanged. The report is
`A:/Galaxy on Fire new version/_work/status_ctor_full_20261013.json`.
Interactive construction with real assets, object destruction and ARM32
runtime remain unverified. Next priority: `StatusWindow::OnTouchEnd`
(89.5% source-shape, 48.4% strict fuzzy, 293 original instructions).
