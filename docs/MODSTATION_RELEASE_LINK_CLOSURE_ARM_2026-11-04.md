# ModStation release path and ARM64 link closure (2026-11-04)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA decompiled `ModStation::OnRelease` at `0xd9066–0xd9224`:
`ev_e814028d1c23d72d20dac702b9db2f46b28732fb3f7c5dd3530cd5b956d3a942`.
The original destroys each owned window, radio, message array, ticker,
and scroll box before calling `operator delete`, then clears the field.

The reconstructed method already had this order but expressed eleven
destructor/delete operations through helper names with declarations and
no definitions. They blocked the strict ARM64 shared-library link.
Those calls now use the corresponding typed C++ `delete`. The radio
message array is confirmed as `new Array<RadioMessage *>()` in the same
translation unit. A strict `--whole-archive --no-undefined` link now
reports **147** unresolved helpers, down from **158**, with none in
`OnRelease` and no new undefined symbols. The remaining list is in
[ARM64 link gaps](ARM64_LINK_GAPS_2026-11-04.md).

Windows native and ARM64 static `gof2` builds pass. ARMv7 compiles
209/209 translation units. Focused `OnRelease` source-shape changes
from **92.4% to 92.6%**, strict fuzzy from **74.9% to 75.1%**; the
function remains nonexact. The corrected path has not been exercised
inside a newly linked APK. A static-library build does not close the
remaining ARM64 link gaps.

Full ARM corpus (`_work/modstation_release_full_20261104.json`):
**4,323/4,524** original functions, **4,770** comparison pairs,
**96.75%** mean source-shape, **86.75%** strict fuzzy,
**2,780 linked-exact**, **1,108 byte-exact**, 201 missing, and zero
skipped. Compared with `_work/startup_missing_full_20261104.json`,
there are no lost comparison pairs or exact matches. The newly
instantiated `Array<RadioMessage *>::~Array` adds one pair and one
linked-exact match.
