# StatusWindow info card — Android ARM audit (2026-10-13)

Scope: the credits, level, playing-time and ship information card inside
`StatusWindow::draw()` at Android 2.0.16 ELF address `0x15831c`. The
original ARM-derived IDA reconstruction gives the text IDs, row spacing,
alignment formulas, and float-to-decimal path. The matching Android
binary is the primary reference; no iOS/PC implementation was used here.

The local code drew credits, level and time at one position; omitted the
space between the level label and number; displayed the ship labels
nowhere; and printed the `getFirePower()` softfp return bits as an integer
with a percent sign. It now draws the three left lines at `y + pad +
{0,1,2}*lineHeight`, right-aligned to the Android half-card boundary.
The right card draws ship name, firepower label `569`, and HP label `570`
at the Android ship-text column, with values right-aligned to the full
card edge. Firepower is decoded from the getter's float bits and joined
as integer part, dot, and up to two fractional characters. The time
string remains the formatter output, not the earlier credits value.

A host smoke extracts the production info-card block and checks all eight
drawn strings, coordinates, ship image index/position, and a 12.5
firepower fixture. The pre-existing draw-prefix and standing-bar smokes
also pass. Native `gof2` and all 206 ARM translation units build; 37
verifier tests pass (one skipped). Focused `draw` source-shape rises from
57.4% to 60.1%, strict fuzzy from 13.8% to 15.9%, with 1,117 local
versus 2,324 original instructions. The 22-row `StatusWindow` family
rises from 89.50% to 89.62% source-shape and 74.06% to 74.16% strict
fuzzy; seven linked-exact and one byte-exact rows remain unchanged.

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.82% average source-shape and 81.21% strict
fuzzy at report precision, 2,395 linked-exact and 1,008 byte-exact.
Complete `(unit,symbol)` exact-set comparisons against the preceding
package show no gains or losses. The edited function remains non-exact.

The smoke uses fixture services, not real game assets. The stats table
below the card, separator encoding for language 9, some ship decimal
corner cases, full UI placement, and ARM32 runtime are not yet verified.
The method remains non-exact. Next priority: recover the two-column
statistics table and its extra direct `Status` fields.
