# Hangar social-credit reward table (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`HangarWindow::OnTouchEnd` calls `Status::changeCredits` with the
indexed table at virtual address `0x202844` after the corresponding
free-credit JNI action. The original table contains six 32-bit words:
`{0, 7500, 5000, 5000, 5000, 7500}`. The touch loop at
`0x14cffc–0x14d0ee` visits five buttons, indices `0..4`; the sixth
table entry is retained in the binary but not reached by this loop.

The recovered table had only five entries and gave index 4 **7500**
instead of **5000**. It is used both to format the offer amount in the
hangar and to credit the player after the fourth visible social action.
The table now has all six original values. The ARMv7 object emits a
24-byte local `.rodata` symbol; all 24 bytes were compared directly
with the original ELF table and match. The existing offerwall callback
`ndk_checkPlaytimeAndSpendOfferwallCredits` is already linked-exact in
the current corpus and was not changed.

Focused `HangarWindow::OnTouchEnd` remains **98.0% source-shape /
70.3% strict fuzzy**; the 40-row unit remains **99.5% / 90.8%**,
with **18 linked-exact / 6 byte-exact** symbols. The verifier metrics
do not change because this packet corrects data bytes, not the
function's instruction form. Windows and ARM64 static builds pass;
ARMv7 compiles **209/209** units. Live reward delivery and persistence
remain unverified while device work is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,752**
pairs: **95.55% mean source-shape**, **85.16% mean strict fuzzy**,
**2,687 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Complete `(unit, symbol)` exact-set comparison against the
preceding revision finds no linked-exact or byte-exact gains or losses.
Reports are stored outside Git under `D:\gof2-arm64-crypto`.
