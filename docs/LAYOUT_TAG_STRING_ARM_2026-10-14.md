# Layout color-tag string — Android ARM audit (2026-10-14)

Scope: `Layout::tagString(AbyssEngine::String)`
(`_ZN6Layout9tagStringEN11AbyssEngine6StringE`) in the Android
2.0.16 ELF at `0xd31b0`. The Thumb call sequence is primary
evidence, cross-checked with the IDA export at
`analysis/gof2_libgof2hdaa_full_ida.c:129735-129764` and the
Ghidra export at
`analysis/gof2_gidra_engine_full_source.c:104069-104108`.
No iOS/PC implementation was used.

The original constructs `"<c:"`, the decimal representation of
`-8388353`, `">"`, the input string, and `"<c:>"` in that order.
The local source instead used empty literals and dereferenced
`g_tagBaseString`, a null pointer never assigned in this source.
It could crash if invoked and could not produce the original tag.
The method now constructs the confirmed string directly.

The Android ABI has a hidden return pointer in `r0`, `this` in
`r1`, and the input String in `r2`. The old header incorrectly
declared the method `static`; removing `static` restores the
instance-method ABI. There are no production source call sites
in this tree. A production-method host smoke now checks that
`layout.tagString(String("SOL"))` produces
`<c:-8388353>SOL<c:>`.

Focused source-shape rises from 92.1% to 100.0%, and strict fuzzy
from 49.8% to 100.0%. The function is not linked-exact or
byte-exact. These scores compare compiled code, not original
ARM32 runtime output. Native `gof2`, all 206 ARM translation units,
the production-method host smoke, and 37 verifier tests pass
(one skipped). Actual rendering of the tag with original assets
and runtime remains untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.95% average source-shape, 81.39% strict fuzzy, 2,402
linked-exact and 1,008 byte-exact. Against the preceding full run,
the complete linked-exact, byte-exact and extra-symbol sets are
unchanged. The corpus strict fuzzy average rises from 81.38% to
81.39%; rounded source-shape remains 92.95%. Next priority: audit
`Layout::reload()` (90.4% source-shape) for actual resource-loading
and control-flow gaps, not metric-only changes.
