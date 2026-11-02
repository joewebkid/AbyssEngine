# SpaceLounge wingman offer gate and confirmation

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

For mode-2 offer **6**, the original first checks whether
`Status::getWingmen()` returns a non-null pointer (`0x1724e4`–
`0x1724ee`). It does not inspect the array's count. If the pointer
exists, it opens game text **785** without choice buttons, sets
`choiceVisible` at offset `0x1b`, and returns before the common
generic-agent event (`0x1730c0`–`0x1730e6`).

Otherwise, it compares credits to `Agent::getCosts()`
(`0x1724f2`–`0x172504`). If the balance is too low, it formats
**cost minus credits**, substitutes `#C` in game text **203**, opens
the window without choice buttons, sets `choiceVisible`, and returns
(`0x1731b4`–`0x173242`, then `0x172f90`–`0x172f9e`).

With sufficient credits, `Achievements::gotAllMedals()` selects game
text **867** when true and **866** otherwise (`0x172508`–`0x172532`).
The original substitutes `Agent::getWingmanFriendsCount() + 1` for
`#Q` and the formatted agent cost for `#C` (`0x17253c`–`0x172604`).
The common tail opens `ChoiceWindow` with buttons and marks
`chatActive` at offset `0x19` (`0x1729ba`–`0x1729d2`).

Focused ARM comparison for `OnTouchEnd()` rose from **63.4% to 70.8%
source-shape**, while strict fuzzy moved from **27.0% to 26.6%**.
The rebuilt function has 1,720 instructions versus 2,630 in the
original. It is neither linked-exact nor byte-exact. The focused
unit retains 22 linked-exact and 7 byte-exact matches across 42
comparisons. ARMv7 compiles 209/209 units; Windows and ARM64 static
builds pass.

Offer actions outside the recovered trade and wingman paths remain
incomplete. Runtime wingman hiring remains unverified while device
testing is deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-wingman-offer-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs. Mean source-shape rose **96.12% → 96.13%**; strict
fuzzy remains **85.93%** at reported precision. It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-wingman-offer-20261102-full.json`.
