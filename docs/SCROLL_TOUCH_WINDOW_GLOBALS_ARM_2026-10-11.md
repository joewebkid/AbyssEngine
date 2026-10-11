# ScrollTouchWindow global wiring — Android ARM (2026-10-11)

## Evidence and correction

`ScrollTouchWindow.cpp` previously had six translation-unit-local pointers
initialized to null for the canvas and layout. They are never assigned,
so drawing and both constructors would dereference null in a native run.
The Android 2.0.16 IDA dump at `0x174128`, `0x1741c0`, and `0x174330`
uses `Globals::layout` in the constructors and drawing paths, and
`Globals::Canvas` for color handling. The original ELF's `drawTextBG`
at `0x17448c` loads GOT slot `0x210224`; its relocation resolves to
`_ZN7Globals6layoutE`. The canvas GOT slot `0x2101f4` resolves to
`_ZN7Globals6CanvasE`. The source now uses those globals directly.

## Verification

- Native `gof2` static-library build: pass.
- ARM object build: 206/206 translation units.
- Verifier suite: 36 run, 35 passed, one skipped.
- Focused family: 16 functions, average source-shape 71.96% to 96.51%.
  `drawTextBG` improves 0.3% to 81.3%; the two constructor aliases of
  each overload improve 23.0% to 93.8% and 28.5% to 95.0%; `draw`
  improves 50.2% to 87.4%.
- Full corpus: 4,713 comparisons; 4,323/4,524 original functions
  compared; 201 missing; zero skipped units or wrong-type symbols.
  Average source-shape 92.24% to 92.32%, strict fuzzy 80.43% to 80.46%,
  linked-exact 2,353 and byte-exact 1,008 unchanged. Per-symbol comparison
  against the previous full report found zero exact-pair losses.
  Current report: `A:/Galaxy on Fire new version/_work/scrolltouch_globals_full_20261011.json`.

## Limit and next priority

The verifier checks ARM machine-code shape, not visual behavior. The
native target is a static library and the full game executable still has
unresolved dependencies, so no UI runtime smoke was possible. Continue
auditing the other null global aliases in UI rendering against the
original ELF, but treat each family separately and preserve exact pairs.
