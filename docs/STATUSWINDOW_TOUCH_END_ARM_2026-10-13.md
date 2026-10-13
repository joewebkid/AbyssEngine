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
