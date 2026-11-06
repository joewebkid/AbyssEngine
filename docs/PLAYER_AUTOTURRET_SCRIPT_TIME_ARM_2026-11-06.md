# PlayerEgo auto-turret script-time gate (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`PlayerEgo::update` (`0x9ce38`) receives `LevelScript* script` as its
second stack argument (`[r7,#12]`). Its auto-turret branch at
`0x9e112` first checks equipment at `PlayerEgo+0x180`, then enabled
state at `+0x355`. If the separate market-system byte at `+0x354`
is set, it reads the signed 64-bit `LevelScript::scriptTime` at
`script+0x08`, compares it with 10,000 and skips auto-turret work
while the value is below the threshold (`0x9e11e`–`0x9e134`). If
the byte is clear, the time check is bypassed. The original does not
null-check `script` in this branch.

The reconstruction already checked equipment and enabled state but
omitted the market/time gate, so auto-turret work could run too early.
It now uses the original condition, with the separately recovered
`field_0x354` and `script->scriptTime`.

Focused ARMv7 verification compiles **211/211** translation units.
`PlayerEgo::update` rises from **90.1% to 90.3%** source-shape and
from **25.1% to 30.1%** strict fuzzy. It remains non-exact. The raw
unmatched-symbol inventory across 211 ARM32 objects stays **229**,
with no added name. Windows native and ARM64 static builds pass, as
does strict ARM64 shared linking.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,799 linked-exact**, **1,113 byte-exact**, 201 missing, zero skipped
and zero wrong-type. Exact-match membership is unchanged. Live
auto-turret behavior and full ARM32 shared linking remain unverified.
