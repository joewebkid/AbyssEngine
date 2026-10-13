# MiningGame update and layer speeds (2026-10-14)

## Evidence and recovery

Android 2.0.16 `MiningGame::update(int)` is at `0x11ec00` in the
original ARM ELF; the shared-workspace IDA export is
`analysis/gof2_libgof2hdaa_full_ida.c`, lines 156966–157145.
The exported 28-byte `LAYER_SPEEDS` object at ELF `0x215d48` contains
seven 32-bit floats: 5, 8, 12, 17, 23, 30, 38. Both `update` and the
constructor now use that recovered table.

`update` now obtains random, layout, sound, status and achievements
through the original `Globals` routes, and calls their typed methods
instead of dereferencing unassigned holders. The random service is
reloaded for each call as in the ARM body. The ore-label alpha reset
uses integer ore boundaries rather than every floating-point increase;
the animation threshold preserves the original `>= 1.0f` behavior for
NaN. Completion writes the original single byte at `Status+0x128`.
The outside-layer branch is ordered like the original, with its own
early returns.

Focused ARM comparison of `update` moves from **14.1% source-shape /
14.1% strict fuzzy** (36 generated versus 376 target instructions)
to **95.1% / 82.1%** (357 versus 376). It is neither linked-exact nor
byte-exact. Constructor C1/C2 remain at 1.3% source-shape and 0.6%
strict fuzzy: table recovery does not resolve their other unassigned
service holders.

The production-method host smoke checks the table endpoints, layer
membership boundaries, integer ore-alpha transitions, animation NaN,
random drift, sound transition, next-layer speed and core-layer win.
Native `gof2` and 37 verifier tests pass (one skipped). The smoke
does not cover the status-loss and achievement-medal branches, and no
ARM32 runtime mining sequence has been executed. The constructor and
`render2D` still contain unassigned service holders. These remain the
next priority before claiming a runnable native mining sequence.

The full ARM corpus has 4,718 comparisons and 4,323/4,524 original
functions covered (201 missing). Average source-shape rises from
93.00% to 93.02% and strict fuzzy from 81.43% to 81.45%. All 2,403
linked-exact and 1,008 byte-exact pairs are preserved, with zero
losses or gains; the 123-symbol extra set is also unchanged.
