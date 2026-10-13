# Layout mission reward — Android ARM audit (2026-10-14)

Scope: `Layout::showMissionRewardMessage(int,bool)` and
`Layout::drawMissionRewardMessage(bool)` in the Android 2.0.16 ELF
at `0xd4d40` and `0xd4d74`. The Thumb code is primary evidence,
cross-checked against the IDA export at
`analysis/gof2_libgof2hdaa_full_ida.c:131044-131168` and the
Ghidra export at
`analysis/gof2_gidra_engine_full_source.c:105491-105613`.
No iOS/PC implementation was used.

`showMissionRewardMessage` now plays sound `0x24` through
`Globals::sound` after storing the active flag, timer and amount,
instead of dereferencing an unassigned pointer. Zero reward still
returns without changing state or playing sound.

`drawMissionRewardMessage` now uses `Globals::Canvas`, screen width,
font and localized text, rather than unassigned holders. The
original fades in during timer values 0–1999, holds full strength
through 5000, and fades out from 5001 to 7000, scaling by
`255/2000` and subtracting 256 from the resulting color integer.
The local code used `256/6000`. The text ID is 216 normally and
3206 for the alternate flag; the amount line starts with `"+ "`.
The centered icon uses its full integer Y coordinate and alignment
bytes `0x11, 0x14`, not a truncated byte coordinate. With the
transition flag, the original calls the six-argument `drawBox`
wrapper twice for styles 2 and 8 using empty strings. The Thumb
calls at `0xd4e4e` and `0xd4e7c` target that overload. Android
`.rodata` at `0x1ca472` confirms the empty literal.

The production-method host smoke covers zero/nonzero rewards,
sound/state changes, both text IDs, the formatted `+ 1,234$`
amount, icon position and alignment, both transition boxes,
color restoration, inactive rendering, and fade boundaries 0,
1999, 2000, 5000, 5001 and 7000. Native `gof2`, all 206 ARM
translation units and 37 verifier tests pass (one skipped).
Focused `showMissionRewardMessage` source-shape rises 89.3% to
100.0%, strict fuzzy 51.4% to 55.0%; `drawMissionRewardMessage`
rises 90.6% to 97.3% and 38.7% to 48.5% respectively. Neither
function is linked-exact or byte-exact. ARM32 runtime behavior and
visual output with original game assets remain untested.

Full corpus: 4,718 comparisons, 4,323/4,524 covered, 201 missing,
92.97% average source-shape, 81.41% strict fuzzy, 2,403
linked-exact and 1,008 byte-exact. Rounded corpus averages and
the complete linked-exact, byte-exact and extra-symbol sets are
unchanged from the preceding run; there are no exact-match losses.
Next priority: audit `Layout::Layout()` (8.7% source-shape),
which still dereferences unassigned platform-configuration holders.
