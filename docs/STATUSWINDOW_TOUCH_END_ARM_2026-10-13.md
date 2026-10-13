# StatusWindow touch release — Android ARM audit (2026-10-13)

Scope: `StatusWindow::OnTouchEnd(int,int)` in the Android 2.0.16 ARM ELF
at `0x15a1c4` (768 bytes), cross-checked with the IDA listing at that
address and the Ghidra reconstruction. The neighboring move/begin methods
were recovered earlier; no iOS/PC implementation was used here.

The local release path dereferenced six unassigned static pointer holders.
The Android path reads `Globals::layout`, gates tab release on
`Globals::iPad`, and uses `Globals::status`, `Globals::gameText`,
`Globals::globals`, and `Globals::font` to prepare medal detail and help
text. Those shared objects now replace the disconnected holders. The
medal's `Achievements::getValue` result is an integer: Android constructs
a `String` from the integer before `Status::replaceHash`. The former local
code reinterpreted that integer as a `String*`, an invalid dereference;
it now calls the existing two-String overload directly.

Focused `StatusWindow` verification builds all 206 ARM translation units.
`OnTouchEnd` rises from 82.7% to 89.5% source-shape and 39.7% to 48.4%
strict fuzzy; it remains non-exact. The 22-row family rises 86.4% to
86.7% source-shape and 71.7% to 72.1% strict fuzzy, retaining seven
linked-exact and one byte-exact comparison. Native `gof2` builds. A native
ABI smoke now checks the actual integer `String` constructor and passes;
the object references the defined `Status::replaceHash` overload rather
than the old declared-only helper. The verifier's 37 tests pass (one
skipped).

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions
covered, 201 missing. Average source-shape remains 92.80% and strict fuzzy
81.20% at report precision. Linked-exact remains 2,395 and byte-exact
1,008. A full `(unit,symbol)` exact-set comparison found no gains or
losses; the improvement is to a previously non-exact function.

No interactive medal/touch session or ARM32 runtime test was run. The
remaining difference in the sizable release method includes String and
array lifetime shape; the non-exact result must not be described as full
behavioral verification. Next priority: recover the separate
`getMedalHintText` game-text binding and then exercise medal selection with
a focused host fixture. That method and other `StatusWindow` draw helpers
still have independent unresolved bindings and were not changed.

## Follow-up: achievements service and detail-array lifetime

When a medal is selected, Android obtains medal state through
`Globals::achievements`. The local method instead used the duplicate
`Achievements::gAchievements` holder. Android also releases the old
detail strings, destroys their array, clears the field and creates a new
array. The local method called `ArrayRemoveAll` after
`ArrayReleaseClasses`, redundantly allocating fresh storage between
release and destruction. The replacement uses the primary global service
and removes that extra step. Tab selection, locked/elite medal handling,
scroll inertia and help-window IDs remain unchanged.

A production-method host smoke checks layout interception, phone/iPad
tab handling, repeated and locked/elite medal selection, detail text,
scroll state and both help IDs. Its strings, buttons and array are fixture
services; it does not test the original assets or full `Array` ABI.
Native `gof2`, all 206 ARM translation units and 37 verifier tests pass
(one skipped). Focused source-shape rises 89.5% to 90.1%; strict fuzzy
rises 48.4% to 48.7%. The method remains non-exact, with 319 local
versus 293 original ARM instructions. The 22-row `StatusWindow` family
remains 96.0% source-shape and 77.7% strict fuzzy at report precision,
with seven linked-exact and one byte-exact symbol.

The full ARM corpus has 4,720 comparisons, 4,323/4,524 original
functions covered, 201 missing, 92.85% average source-shape and 81.23%
strict fuzzy at report precision, 2,395 linked-exact and 1,008
byte-exact. Complete `(unit,symbol)` exact-set comparisons against the
constructor package show no gains or losses; the 123 extra-symbol set
is unchanged. The report is
`A:/Galaxy on Fire new version/_work/status_touch_end_full_20261013.json`.
Full UI interaction, actual array ownership under ARM32 and asset-backed
rendering remain unverified. Next priority: the `StatusWindow` scroll
geometry helpers, starting with `getRelativeScrollHeight` (84.8%
source-shape), followed by the remaining non-exact draw blocks.
