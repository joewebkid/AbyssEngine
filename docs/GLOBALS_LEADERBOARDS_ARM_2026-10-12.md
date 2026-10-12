# Globals reportLeaderboards — Android ARM audit (2026-10-12)

The Android 2.0.16 function at ELF address `0xe4b90` calls
`Status::getKills()` on `Globals::status` and stores the result in
`g_android_leaderboard_scores[0]`. Direct ARM disassembly confirms the
call/store sequence; the IDA dump identifies both global symbols. The
recovered function instead read `Status::gStatus` and wrote through a null
placeholder. Both status pointers are set together during `Globals::init`,
but the source identity matters for matching and lifetime analysis.

The destination array already exists in `JniBridge.cpp`; its JNI reader
and resetter use the same storage. `ModStation::checkMedals` calls
`reportLeaderboards`, so this is an observable score handoff, not an
isolated metric adjustment. The replacement only changes this function's
source and its external declaration.

Native `gof2` builds, all 206 ARM translation units compile, and the
verifier suite runs 37 tests with 36 passed and one skipped. The focused
function rises from 80.7% to 100% source-shape and from 69.6% to 100%
strict fuzzy, and becomes linked-exact (not raw byte-exact). The full
corpus has 4,714 comparisons, 4,323/4,524 original functions covered,
201 missing, zero skipped or wrong-type, 92.36% average source-shape,
80.56% strict fuzzy, 2,359 linked-exact and 1,008 byte-exact matches.
A per-symbol comparison found only this new exact pair and no exact-pair
losses. Report:
`A:/Galaxy on Fire new version/_work/globals_leaderboards_full_20261012.json`.

No live Android leaderboard flow was run; the native target is a static
library. JNI integration and score submission beyond this handoff are not
proven by this comparison.
