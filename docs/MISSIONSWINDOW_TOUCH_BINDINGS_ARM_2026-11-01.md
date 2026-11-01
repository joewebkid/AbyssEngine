# MissionsWindow touch and map-return bindings (Android ARM)

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `MissionsWindow::OnTouchBegin` at `0x150bdc`,
`OnTouchMove` at `0x150ca8`, and `OnTouchEnd` at `0x150d74`.
The first two call the corresponding method of `Globals::layout`
(`0x150c5c`, `0x150d28`). The source previously dereferenced two
unbound translation-unit layout pointers.

`OnTouchEnd` calls `StarMap::OnTouchEnd` directly at `0x150e62` and
restores the window dimensions from `Globals::iPad`, `iPadHD`,
`iPadLarge`, `w`, and `h` after leaving the map. On a non-iPad device
the dimensions are the actual screen width and height, not zero.
The layout reset and footer/help handling use `Globals::layout`.
When a freelance mission is rejected, the original installs
`Mission::empty` at `0x1510b4`; storing null would leave later code
without the expected empty mission object. The source now follows
these bindings and removes the unbound `g_mwt_*` pointers and the
32-bit `StarMap_OnTouchEnd` shim.

Windows native and Android ARM64 static `gof2` targets built. Android
ARMv7 compiled 209/209 translation units with zero failures. Focused
`game/ui/MissionsWindow` kept 9 linked-exact and 2 byte-exact matches.
`OnTouchBegin` and `OnTouchMove` source-shape rose from 95.8% to 98.3%
each, and `OnTouchEnd` rose from 89.3% to 95.6%; strict fuzzy for
`OnTouchEnd` rose from 43.0% to 60.6%. The full ARM corpus covered
4,323/4,524 originals over 4,752 comparisons, with zero skipped and
201 missing. Average source-shape and strict fuzzy stayed at 96.03%
and 85.80%; linked-exact stayed at 2,726 and byte-exact at 1,091.
Complete exact-symbol set comparison found zero losses.

Static builds and ARM comparison do not prove working screen input.
Live testing remains deferred at the user's request.
