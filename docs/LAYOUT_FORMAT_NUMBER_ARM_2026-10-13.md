# Layout number formatting — Android ARM audit (2026-10-13)

Scope: `Layout::formatNumber(int)` (`_ZN6Layout12formatNumberEi`)
in the Android 2.0.16 ELF at `0xd32b4`. The Thumb disassembly is
primary evidence, cross-checked against the IDA export at
`analysis/gof2_libgof2hdaa_full_ida.c:129766-129840` and the
Ghidra export at `analysis/gof2_gidra_engine_full_source.c:104118`.
No iOS/PC implementation was used.

Android negates a negative input in 32-bit arithmetic, constructs
the digit string with `String(int)`, and selects `","` for language
IDs 0, 10 and 11 (`0xc01` mask), `"."` otherwise. It takes each
three-digit group with `SubString(oldLength - 3, oldLength)`, prepends
that group and then a separator only when a previous group exists,
and finally prepends the remaining one or two digits. A negative
input gets `"-"` prepended.

The local code had empty separator and sign literals, selected
`SubString(i, originalLength)` on every iteration, and combined
groups as `separator + group + output`; numbers of six or more
digits were therefore malformed. It also default-constructed and
assigned several temporary Strings where ARM constructs results
directly. These are corrected. Unsigned subtraction avoids C++
signed-overflow undefined behavior while reproducing the ARM
32-bit negation bit pattern. For `INT_MIN`, that bit pattern is still
negative when passed to `String(int)`, then the formatter prepends
another minus. The host smoke observes `--2,147,483,648`; actual
ARM32 runtime output at that edge remains untested.

The production-method host smoke checks 0, 999/1000, six- and
seven-digit grouping, negative values, language IDs 0/1/10/11/12,
and `INT_MIN`. Focused source-shape rises from 83.6% to 98.9%,
strict fuzzy from 36.6% to 80.1%; the function remains non-exact.
Native `gof2`, all 206 ARM units and 37 verifier tests pass (one
skipped). Real localized text, visual output and ARM32 runtime
remain untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.95% average source-shape, 81.38% strict fuzzy, 2,402 linked-exact
and 1,008 byte-exact. Against the preceding full run, complete
linked-exact, byte-exact and extra-symbol sets are unchanged. Next
priority: audit `Layout::tagString(String)` (92.1% source-shape),
which still dereferences an unassigned holder.
