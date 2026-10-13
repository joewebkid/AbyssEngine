# Layout one-argument window wrapper — Android ARM audit (2026-10-13)

Scope: `Layout::drawWindow(String)`
(`_ZN6Layout10drawWindowEN11AbyssEngine6StringE`) in the Android
2.0.16 ELF at `0xd387c`. Primary evidence is its Thumb disassembly:
the wrapper invokes `String(const String&, bool)` with `false`, then
`Layout::drawWindow(String,bool)` with `drawBG=false`. The independent
Ghidra export at `analysis/gof2_gidra_engine_full_source.c:104399-104417`
shows the same calls. No iOS/PC implementation was used.

The local wrapper used an implicit String copy and forwarded
`drawBG=true`, painting a background that the Android version does
not paint. It now preserves the explicit non-reversing String copy
and passes `false`. The production-method UI smoke checks that this
overload draws the frame and header without the background pattern,
with the inherited screen and top-inset geometry.

Focused source-shape rises from 76.1% to 100.0%, strict fuzzy from
63.2% to 100.0%, adding a linked-exact (not byte-exact) result.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Full corpus: 4,718 comparisons, 4,323/4,524 covered,
201 missing, 92.94% average source-shape, 81.35% strict fuzzy,
2,401 linked-exact and 1,008 byte-exact. Against the preceding
full run, this wrapper is the only linked-exact gain; complete
linked-exact, byte-exact and extra-symbol sets show no losses or
other changes. Real assets, visual output and ARM32 runtime remain
untested. Next priority: audit `Layout::resetWindowDimensions()`
(77.8% source-shape) for behavior and ABI gaps.
