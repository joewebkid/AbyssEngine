# MissionsWindow game text binding (Android ARM)

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
reads `Globals::gameText` in `MissionsWindow::init()` at `0x14fb3e`,
`0x14fb84` and subsequent title/button calls. The earlier source
used a file-static `g_mw_gameText` initialized to null and never bound.
The same placeholder was used by `draw()` and touch handling. All 16
remaining accesses in this translation unit now use
`Globals::gameText`, and the dead placeholder has been removed.

Windows native and Android ARM64 static `gof2` targets built. Android
ARMv7 compiled 209/209 units with zero failures. Focused
`game/ui/MissionsWindow` kept 7 linked-exact and 2 byte-exact matches;
`init()` source-shape rose from 8.8% to 9.2%, `draw()` from 33.2% to
34.3%, and `OnTouchEnd()` from 86.4% to 87.5%. The full ARM corpus
covered 4,323/4,524 originals over 4,752 comparisons, with zero
skipped and 201 missing. Average source-shape and strict fuzzy stayed
at 96.00% and 85.79%; linked-exact stayed at 2,724 and byte-exact at
1,091. Complete exact-symbol set comparison found zero losses.

`MissionsWindow::init()` still has other unbound translation-unit
pointers (layout, device flags, dimensions, campaign visibility and
image factory). This binding alone does not make the screen operational.
Live testing is deferred at the user's request.
