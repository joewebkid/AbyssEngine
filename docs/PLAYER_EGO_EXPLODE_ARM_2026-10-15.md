# PlayerEgo::explode Android ARM recovery (2026-10-15)

## endExplosion follow-up (2026-10-15)

Android Thumb `_ZN9PlayerEgo12endExplosionEv` at `0xa15c6–0xa15d2`
loads `PlayerEgo+0x8c` and tail-calls `Explosion::reset()` only when it
is non-null. The former C++ instead called `update(0, nullptr)`; this
does not perform the same animation reset. The extracted-production
smoke now checks both the null and non-null paths. No direct source
callers were found, so the live route remains unknown.

Focused source-shape/strict fuzzy improves **59.2%/36.4% →
100%/100%** and becomes linked-exact, not byte-exact. Native `gof2`,
206/206 ARM units, adjacent smokes and 37 verifier tests (one skipped)
pass. Full report `_work/player_end_explosion_full_20261015.json`
compares **4,323/4,524** functions at **93.43%** average source-shape
and **82.33%** strict fuzzy, with **2,485 linked-exact**, **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped. The
linked-exact set gains only `endExplosion`; neither exact set loses a
member versus `_work/player_explode_full_20261015.json`.

## explode evidence and behavior

Primary target: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`_ZN9PlayerEgo7explodeEv` at Thumb `0xa13e0`. REA/IDA pseudocode was
cross-checked against the ARM instructions.

- `0xa13ea–0xa1402`: enable particle emission at `PlayerEgo+0x2fc`
  on every call; if the explosion pointer at `+0x8c` is already set,
  return without restarting the remaining effects.
- `0xa1404–0xa1424`: deactivate the follow camera and player, allocate
  a 0x68-byte `Explosion(0)`, and store it at `+0x8c`. An ARM32 size
  assertion now guards the reconstructed class layout.
- `0xa1428–0xa147a`: stop ten FMOD events through `Globals::sound`, in
  order: its current music event, the ship sound ID at `+0x1c`, then
  `27`, `35`, `2261`, `2260`, `2252`, `1095`, `1096`, `1097`.
- `0xa147c–0xa1488`: call `setExhaustVisible(false)`.

The prior C++ called through uninitialized null function/object globals
instead of `FModSound::stop`, did not hide exhaust, and immediately
updated the new explosion. Those were behavior and crash risks, not only
source-shape differences. `MGame::gameOverCheck` calls `explode` on the
player-death path.

## explode checks

An extracted-production smoke covers first and repeated calls, emission,
creation and identity of the explosion, camera/player activation, ordered
audio stops, and exhaust visibility. Native `gof2`, 206/206 ARM units,
the adjacent `revive` smoke and 37 verifier tests (one skipped) pass.
Focused source-shape/strict fuzzy improves **61.7%/49.5% →
100%/100%** with 73 instructions on both sides; the function is still
neither linked-exact nor byte-exact.

The full report `_work/player_explode_full_20261015.json` compares
**4,323/4,524** original functions at **93.42%** average source-shape
and **82.32%** strict fuzzy, with **2,484 linked-exact** and **1,065
byte-exact**, **201 missing**, **123 extras** and zero skipped. Average
source-shape/strict fuzzy rises from **93.41%/82.31%**; the count of
100%-fuzzy functions rises by one to **2,641**. Both exact symbol sets
are unchanged from `_work/player_revive_final_full_20261015.json`.

## Limits and next work

The smoke uses host stubs, not a live Android ARM32 game or FMOD runtime.
REA's IDA adapter cannot certify complete function-body ranges or all
indirect call edges. iOS death-flow behavior has not been compared.
Next: controlled ARM32 runtime/ABI exercise of the `explode`/`revive`
transition and, where possible, an iOS guest trace.
