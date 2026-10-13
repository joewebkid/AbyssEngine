# StatusWindow reInit — Android ARM audit (2026-10-13)

Scope: `StatusWindow::reInit()` at Android 2.0.16 ELF address `0x158044`
(288 bytes). The IDA reconstruction and independent Ghidra output both
identify the image factory, status/standing calls, canvas calls, and rank
image selection. No iOS or PC source was needed for this package.

The old C++ dereferenced an unassigned `ImageFactory**`, passed a one-int
descriptor to `ImageFactory::loadChar(int*)` (which reads five ints), and
called declared-only `Achievements_get` / `Achievements_isUnlocked` helpers.
The Android function instead passes the global image factory and an Android
`.bss` descriptor at `0x21839c` to `loadChar`, then calls
`Status::getStanding()->isEnemy(0..3)` on the global status. The local
descriptor now supplies the five zero-initialized words consumed by
`loadChar`; the ARM `.bss` span from `0x21839c` to the next exported symbol
at `0x2183b0` accommodates those words, without proving the complete
original symbol boundary. Canvas calls now use `Globals::Canvas` as in the
original. Existing rank IDs and branch precedence were retained.

A host smoke extracts the production method body and checks the five-word
descriptor, four standing queries, rank-image IDs and precedence, and
width/height reads from the first rank handle. It passes. Native `gof2`
and all 206 ARM translation units build; 37 verifier tests pass (one skipped).
The method rises from 92.0% to 97.5% source-shape and from 65.0% to 89.1%
strict fuzzy (106 original / 105 local instructions). It is not exact.
The 22-row `StatusWindow` family rises from 86.76% to 87.01% source-shape
and 72.4% to 73.5% strict fuzzy, retaining seven linked-exact and one
byte-exact symbols.

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.81% average source-shape and 81.21% strict
fuzzy, 2,395 linked-exact and 1,008 byte-exact. Comparing complete
`(unit,symbol)` exact sets against the prior package finds no gains or
losses. The edited method is still non-exact.

The fixture does not load real image assets or run an ARM32 UI. Character
descriptor ownership, live asset resolution, and the much larger
`StatusWindow::draw` remain unverified. Next priority: audit `draw` in a
separate bounded package, starting with its unassigned global holders.
