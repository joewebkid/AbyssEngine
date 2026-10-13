# Layout drawBox default-flags wrapper — Android ARM audit (2026-10-13)

Scope: `Layout::drawBox(int,int,int,int,int,String)`
(`_ZN6Layout7drawBoxEiiiiiN11AbyssEngine6StringE`) in the Android
2.0.16 ELF at `0xd48f0`. Primary evidence is its Thumb disassembly:
the wrapper constructs `String(const String&, false)` once and calls
the seven-argument overload with flags `1`. The independent Ghidra
export at `analysis/gof2_gidra_engine_full_source.c:105147-105167`
shows the same calls. No iOS/PC implementation was used.

The local wrapper used an implicit copy followed by another
by-value copy, and forwarded flags `0`. It now forwards one explicit
non-reversing copy and flags `1`. The current Android renderer tests
only bits 1 and 2 of this flag, so bit 0 has no observed visual
effect there; the correction restores the confirmed call contract
without inventing a visual difference. The existing production-method
UI smoke exercises this wrapper and all renderer styles but cannot
distinguish flags 0 and 1. Focused ARM comparison does distinguish
the forwarded value and copy shape.

Focused source-shape rises from 79.2% to 100.0%, strict fuzzy from
50.0% to 100.0%, adding a linked-exact (not byte-exact) result.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Real assets, visual output and ARM32 runtime remain
untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.95% average source-shape, 81.37% strict fuzzy, 2,402 linked-exact
and 1,008 byte-exact. Against the preceding full run, this wrapper
is the only linked-exact gain; complete linked-exact, byte-exact and
extra-symbol sets show no losses or other changes. Next priority:
audit `Layout::formatNumber(int)` (83.6% source-shape) for negative
and grouping edge cases.
