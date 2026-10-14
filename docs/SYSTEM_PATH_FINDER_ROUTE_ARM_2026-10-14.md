# SystemPathFinder route state and node release (2026-10-14)

Android `SystemPathFinder::getSystemPath` at `0x11b7a8` reads route
visibility through `Status::getSystemVisibilities(Globals::status)`.
The recovered body instead dereferenced an unassigned translation-unit
pointer. It now uses `Globals::status`, the same service consumed by
its `StarMap` and `Status` callers.

The Android success tail calls `ArrayReleaseClasses<Node *>` on the
allocated node array before destroying it. The former source deleted
only the nodes in the returned path and leaked the other nodes and
array. The successful-path cleanup now releases all nodes after copying
their indices into the integer result. The original no-route early
return does not show this cleanup; that path and the search scratch
arrays still need a separate ownership audit.

`tools/run_system_pathfinder_smoke.py` compiles the production
`SystemPathFinder.cpp` and `Node.cpp` against minimal service fixtures.
It checks a three-system, two-jump route, `getJumpDistance == 2`, and
exclusion of an invisible destination. This is a host test, not a live
StarMap or Android runtime test. Native `gof2`, 206/206 ARM translation
units, and 37 verifier tests (one skipped) pass.

Focused `getSystemPath` source-shape improves **95.4% → 96.0%**;
strict fuzzy falls **67.6% → 59.1%**. The function is not exact.
The full report `_work/system_pathfinder_full_20261014.json` compares
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,480 linked-exact** and **1,065
byte-exact**. Against the preceding save-preview report, no exact pair
is lost; one linked-exact `ArrayReleaseClasses<Node *>` specialization
is newly emitted. The 123-extra-symbol set is unchanged and no unit
is skipped.

Next: audit the pathfinder's no-route allocation lifetime and the
shared `Node` type before asserting repeated-query memory behavior;
live StarMap navigation and ARM32 runtime remain unverified.
