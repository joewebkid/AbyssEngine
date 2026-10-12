# Globals getItemName — Android ARM audit (2026-10-12)

The Android 2.0.16 function at ELF address `0xe4b68` loads
`Globals::gameText`, calls `GameText::getText(item + 0x4fa)`, then invokes
`AbyssEngine::String::String(const String&, false)` for the return value.
Direct ARM disassembly confirms the offset and explicit false argument;
the IDA dump identifies the global. The recovered function instead
dereferenced a null pointer placeholder and used the inline copy path.

The replacement is a single return expression using the live global and
the matching constructor. No in-tree direct caller was found, but the
original exported function remains part of the recovered `Globals` API.

Native `gof2` builds, all 206 ARM translation units compile, and the
verifier suite runs 37 tests with 36 passed and one skipped. The focused
function rises from 82.0% to 100% source-shape and 66.7% to 100% strict
fuzzy, and is now linked-exact (not raw byte-exact). The full corpus has
4,714 comparisons, 4,323/4,524 original functions covered, 201 missing,
zero skipped or wrong-type, 92.36% average source-shape, 80.55% strict
fuzzy, 2,358 linked-exact and 1,008 byte-exact matches. A per-symbol
comparison found only this new exact pair and no exact-pair losses. Report:
`A:/Galaxy on Fire new version/_work/globals_item_name_full_20261012.json`.

No live text lookup was run; the native target is a static library. The
initialization and lifetime of `Globals::gameText` are separate open work.
