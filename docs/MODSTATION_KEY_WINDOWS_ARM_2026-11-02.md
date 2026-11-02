# ModStation keyboard routes to missions and status

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`ModStation::OnKeyPress(long long,long long)` at `0xd92a4` in the
REA/IDA analysis database.

The original checks for level-up through `Globals::status` before
dispatch (`0xd92d4`). When the medal choice is active at station
offset `0x6a`, confirm invokes `ModStation::checkMedals()`
(`0xd9342`–`0xd9368`); the previous source called an unbound
`leaveStation` function. The back key on the station screen calls
the actual `ModStation::leaveStation()` method. With the secondary
menu byte at `0x62` set, the original does not toggle the main-menu
byte at `0x61` on key `0x20000` (`0xd9370`–`0xd94e0`). These
bindings and the branch were corrected. Out-of-range button indices
now return without calling unbound fragment functions, as the
original switch default does (`0xd93b6`).

For selected button 3, campaign steps through 8 show text 528 in
the primary choice window and set its visible byte at `0x63`.
Later steps initialize or create `MissionsWindow`, set the missions
window byte at `0x64`, lower the music parameter to 0.6, and show
help text 635 once (`0xd95e4`–`0xd97e4`). For selected button 4,
the original creates or reinitializes `StatusWindow`, sets its byte
at `0x68`, applies the same music parameter, and shows help text
640 once on non-iPad devices (`0xd9542`–`0xd95ba`). The previous
handler performed none of these actions. The two one-time help
markers are represented by file-static booleans; their process
lifetime follows the observed global bytes, while their precise
original symbol ownership remains unverified.

Focused `OnKeyPress()` source-shape rises **14.2% → 35.4%** and
strict fuzzy **5.4% → 18.0%**, with 266 rebuilt instructions
versus 889 original. ARMv7 compiles 209/209 units; the focused
ModStation unit compares 36 symbols and retains **9 linked-exact /
3 byte-exact** matches. Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/modstation-key-windows-20261102-focused.json`.

The remaining button routes 0–2 still need recovery. Live keyboard,
medal, help and window behavior remain unverified while device work
is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.18%** mean source-shape and **85.94%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/modstation-key-windows-20261102-full.json`.
