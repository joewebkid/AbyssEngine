# Layout full-screen window wrapper — Android ARM audit (2026-10-13)

Scope: `Layout::drawWindow(String,bool)`
(`_ZN6Layout10drawWindowEN11AbyssEngine6StringEb`) in the Android
2.0.16 ELF at `0xd36f4`. Primary evidence is the original Thumb
disassembly. Its GOT relocations identify the live `Globals::layout`,
`Globals::w` and `Globals::h` symbols; the IDA export's wrapper at
`analysis/gof2_libgof2hdaa_full_ida.c:129984-129992` omits the
argument expressions. No iOS/PC implementation was used.

The local wrapper dereferenced three unassigned holders to obtain
window dimensions and copied the title through a different String
constructor. Android constructs the forwarded String with its
`(const String&, false)` constructor, then invokes the six-argument
`drawWindow` at `(0,0)` with width `Globals::w`, height
`Globals::h - Globals::layout->field_0x8`, and the caller's
background flag. The called function subtracts the top inset again
when calculating the frame height. This double subtraction is
counterintuitive, but is confirmed by the two ARM bodies and is
preserved, not "corrected" speculatively.

Focused source-shape rises from 74.7% to 100.0%, strict fuzzy from
40.0% to 100.0%, adding a linked-exact (not byte-exact) result. The
production-method TouchButton/Layout smoke exercises both background
flags and checks the resulting screen-width, inset and frame-height
geometry. Native `gof2`, all 206 ARM units and 37 verifier tests pass
(one skipped).

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.93% average source-shape, 81.34% strict fuzzy, 2,400 linked-exact
and 1,008 byte-exact. Against the preceding full run, the wrapper is
the only linked-exact gain; complete linked-exact, byte-exact and
extra-symbol sets show no losses or other changes. Real assets,
visual output and ARM32 runtime remain untested. The next bounded
priority is the one-argument `drawWindow(String)` overload (76.1%
source-shape).
