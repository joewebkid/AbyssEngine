# StatusWindow medal-hint text — Android ARM audit (2026-10-13)

Scope: `StatusWindow::getMedalHintText(int)` at Android 2.0.16 ELF address
`0x159ba4` (1,132 bytes). The IDA and Ghidra reconstructions agree on the
five medal-index branches, text IDs, shared globals, and literal separators.
No iOS/PC implementation was used for this package.

The old C++ dereferenced an unassigned `GameText**` in every nonempty
branch. It now reads `Globals::gameText`, `Globals::achievements`, and
`Globals::status`, as the Android code does. Android prefixes the detail
header with two newlines and each missing requirement with `"\n- "`;
the local code had used one newline for both. Text IDs remain `0x114`,
`0x594+i`, `0x59f+i`, `0x57e+i`, and `0x4fa+BluePrint::getIndex()` for
medals 2, 3, 9, 13, and 14 respectively. The locked and unproduced
blueprint tests are unchanged.

A host smoke extracts the production method body and checks all five
branches, no-medal/bronze cases, separators, and selected text IDs with
fixture services. It passes. It does not use original text assets or
exercise real touch dispatch. Native `gof2` builds; focused ARM validation
builds 206/206 translation units. The method's source-shape rises 90.6%
to 92.1% and strict fuzzy 44.7% to 52.1%; it remains non-exact. The
22-row `StatusWindow` family rises 86.7% to 86.8% source-shape and 72.1%
to 72.4% strict fuzzy, retaining seven linked-exact and one byte-exact.
The verifier's 37 tests pass (one skipped).

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.80% average source-shape and 81.20% strict fuzzy
at report precision, 2,395 linked-exact and 1,008 byte-exact. Comparing
the complete `(unit,symbol)` exact sets finds no gains or losses; this
package improves a previously non-exact function.

String temporary lifetimes and the full medal-selection path remain
unverified on ARM32 and in a running game. Next priority: audit
`StatusWindow::reInit`, which still uses an unassigned image-factory
holder and declared-only achievement helpers.
