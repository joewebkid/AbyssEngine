# MiningGame constructor service and image bindings (2026-10-14)

## Evidence and change

Android 2.0.16 `MiningGame::MiningGame(int,int,Hud*)` begins at
`0x11e7a8`; the shared-workspace IDA export
`analysis/gof2_libgof2hdaa_full_ida.c` covers it at lines
156709–156876. It reads `Globals::w/h`, `Globals::layout`,
`Globals::status` and `Globals::Canvas`, queries the ship's mining
equipment, and passes image-ID field addresses directly to
`PaintCanvas::Image2DCreate`.

The constructor now uses those real service routes instead of five
unassigned holders. Its image-ID fields are typed `uint32_t`, preserving
32-bit layout while allowing direct output references to the canvas
method. The drill image output starts at `-1`, as in the original,
instead of an uninitialized value. Reference-pixel and label-center
halves use the original arithmetic shifts. Layout and canvas globals
are read at the corresponding uses rather than cached across calls.

Focused ARM comparison for both C1/C2 aliases rises from **1.3%
source-shape / 0.6% strict fuzzy** (5 generated versus 311 target
instructions) to **99.1% / 89.1%** (313 versus 311). Neither alias is
linked-exact or byte-exact. The production-constructor host smoke
checks screen/layout coordinates, equipment-derived control and ore
rates, initialized drill image output, direct image handles, marquees,
campaign flag, and both core-image selections. The existing layer
smoke, native `gof2` and 37 verifier tests also pass (one skipped).

The no-equipment path leaves `controlDivisor` unset in both the
recovered source and Android decompilation; whether that path can be
reached in a real game session is not established. The constructor
smoke uses service doubles, not original game assets or ARM32 runtime.
`render2D` retains unassigned service holders; the full native mining
sequence remains untested. That rendering path and the status-loss /
achievement branches are the next priorities.

The full ARM corpus has 4,718 comparisons and 4,323/4,524 original
functions covered (201 missing). Average source-shape rises from
93.02% to 93.06% and strict fuzzy from 81.45% to 81.48%. The 2,403
linked-exact and 1,008 byte-exact pair sets show zero gains or losses;
the 123-symbol extra set is unchanged.
