# Level::createPlayer weapon factor bits — 2026-10-20

Original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `Ship::getDamageFactor` and `Ship::getFireRateFactor` inside
`Level::createPlayer`. At `0xaf47c–0xaf480`, `0xaf49a–0xaf49e` and
`0xaf4d4–0xaf4dc` each returned `r0` word is moved directly to VFP `s0`.
The negative-damage-factor branch then compares `s0` as a float, and the
rate path multiplies it as a float. There is no integer-to-float conversion.

The existing `Ship` getters expose raw IEEE-754 storage bits as `int`, but
production `Level::createPlayer` had used numeric `(float)` casts. That
would turn ordinary factors such as `1.25f` or `0.8f` into billion-scale
numbers before calculating primary-gun damage and cooldown. The three
call sites now use the file's existing `bitsToFloat` helper.

A focused ARM test checks the original `vmov`/float operations. An extracted
host branch test compiles the actual two `Ship` getter bodies, the existing
bit-decoding helper and the corrected weapon-factor branch at `-O0/-O2`;
it covers ordinary and negative-factor cases. Native `gof2` and ABI/lifetime
smoke pass. All **208/208** ARM translation units compile; **91 verifier
tests** pass with one pre-existing skip.

Focused `Level::createPlayer` strict fuzzy rises **34.5% → 35.5%**;
source-shape stays **86.0%** at one decimal place. It remains nonexact.
The complete ARM corpus compares **4,323/4,524** originals in **4,735
pairs**, at **93.93%** mean source-shape and **83.22%** strict fuzzy. It
retains **2,551 linked-exact** and **1,079 byte-exact**; independent
`(unit, symbol)` set comparisons show zero gains or losses. Skips and
wrong-type signatures are zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/level-player-factors-full.json`
(SHA-256 `79c04bc41f531c75cceb997f4e3304229011790690c5dffef864c1442e2dc50a`).

This proves the factor calculation, not full `Level::createPlayer` execution.
The continuous Level gate still stops before that method; real active Ship,
equipment, weapon-position loading, `PlayerEgo` construction and Level
stage-0 return remain to be integrated.
