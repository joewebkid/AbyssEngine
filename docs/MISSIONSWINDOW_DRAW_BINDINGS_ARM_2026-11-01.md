# MissionsWindow draw bindings (Android ARM)

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `MissionsWindow::draw()` at `0x1504dc`. After the wanted
board and star-map branches it sets `Globals::Canvas` to opaque white,
uses `Globals::layout` and `Globals::font`, and fetches text IDs 129
for the header, 555 for the campaign panel and 556 for the freelance
panel (`0x150548`–`0x150716`). The earlier source dereferenced
unbound local pointers for these values and reused one unknown text ID.
The source now uses the observed bindings and the six-argument
`Layout::drawBox` overload used by the ARM calls.

For a nonempty freelance mission, the original draws character parts
through `Globals::imageFactory`, then draws the agent name, station
and mission type with `Globals::font`. The latter two lines are one and
two `Layout::field_0x4` steps below the name (`0x1507f2`–`0x15091e`).
The previous source placed all three strings at the same vertical
coordinate. The three lines now follow the original positions.

Windows native and Android ARM64 static `gof2` targets built. Android
ARMv7 compiled 209/209 translation units with zero failures. Focused
`game/ui/MissionsWindow` kept 9 linked-exact and 2 byte-exact matches;
`draw()` source-shape rose from 34.3% to 85.3% and strict fuzzy from
19.0% to 33.9%. The full ARM corpus covered 4,323/4,524 originals
over 4,752 comparisons, with zero skipped and 201 missing. Average
source-shape rose from 96.02% to 96.03%; strict fuzzy stayed at 85.80%.
Linked-exact stayed at 2,726 and byte-exact at 1,091. Comparing the
complete exact-symbol sets found zero losses.

Touch handling in this screen still has unbound translation-unit
pointers. Actual rendering and input behavior remain unverified while
device testing is deferred at the user's request.
