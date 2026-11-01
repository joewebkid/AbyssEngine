# MissionsWindow update (Android ARM)

## Original evidence

The Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `MissionsWindow::update(int)` at `0x1509b0`. It forwards the
incoming `dt` to the wanted board or star map, then to both mission scroll
windows in the ordinary mode. The source had passed zero to the first two.

For production missions (types 167 and 174), the original tests the two
visibility bytes of `Globals::options` at offsets `0x35` and `0x37` when
the game has been won. The earlier source dereferenced an uninitialized
translation-unit pointer instead. The original reads title text from
`Globals::gameText`, indexes a 164-word table at ELF address `0x20285c`,
and replaces the `#Q` token with the outstanding production amount.
The source now uses those bindings and `Status::replaceHash`. All 164
table words were compared directly with the mapped ELF bytes and match.

## Verification

- Windows native and Android ARM64 static `gof2` targets built.
- Android ARMv7 compiled 209/209 translation units with zero failures.
- Focused `game/ui/MissionsWindow`: 18 comparisons, 7 linked-exact and
  2 byte-exact. `update()` source-shape rose from 53.0% to 87.1%; strict
  fuzzy rose from 41.6% to 61.8%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons; zero skipped, 201 missing. Average source-shape rose
  from 95.99% to 96.00%, strict fuzzy from 85.78% to 85.79%.
  Linked-exact stayed at 2,724 and byte-exact at 1,091. Comparing the
  complete exact-symbol sets with the preceding report found zero losses.

This restores the selected update path, not the whole `MissionsWindow`.
Its `init()` and other methods still contain unbound translation-unit
pointers, so live operation of this screen is unverified. Device testing
is deferred at the user's request.
