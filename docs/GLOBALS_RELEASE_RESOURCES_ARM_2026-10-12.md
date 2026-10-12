# Globals releaseResources — Android ARM audit (2026-10-12)

The Android 2.0.16 function at ELF address `0xe479c` calls
`PaintCanvas::ReleaseAllResources()` on `Globals::Canvas`, then tail-calls
`Layout::reload()` on `Globals::layout`. The ARM disassembly confirms the
two global loads and call sequence; the IDA dump names both globals.
The previous reconstruction instead used `PaintCanvas::gCanvas` and
read a null placeholder as a second canvas, conditionally clearing it.

The replacement uses the existing `Globals` members and `Layout::reload`
method. No in-tree direct caller of `releaseResources` was found; its
exported symbol remains part of the recovered API.

Native `gof2` builds, all 206 ARM translation units compile, and the
verifier suite runs 37 tests with 36 passed and one skipped. The focused
function rises from 44.2% to 100% source-shape and 57.1% to 100% strict
fuzzy, becoming linked-exact (not raw byte-exact). The full corpus has
4,714 comparisons, 4,323/4,524 original functions covered, 201 missing,
zero skipped or wrong-type, 92.37% average source-shape, 80.57% strict
fuzzy, 2,360 linked-exact and 1,008 byte-exact matches. A per-symbol
comparison found only this new exact pair and no exact-pair losses.
Report: `A:/Galaxy on Fire new version/_work/globals_release_resources_full_20261012.json`.

No live resource-reload run was performed; the native target is a static
library. `Layout::reload` itself still dereferences several null global
placeholders, so the full call chain is not yet runtime-ready. Its global
bindings and behavior are the next recovery package.
