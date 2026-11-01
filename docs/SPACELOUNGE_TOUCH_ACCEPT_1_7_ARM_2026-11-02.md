# SpaceLounge offer acceptance: variants 1 and 7

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

In mode 2, a consumed button at index 0 dispatches on the selected
agent's offer. Offer 1 clears mode and returns immediately
(`0x172edc`); the generic-agent event path is bypassed.

Offer 7 compares the player's credits with the agent's cost
(`0x172420`–`0x17243c`). If affordable, it formats the **full cost**,
substitutes `#C` in game text 885, opens `ChoiceWindow` with buttons,
and marks `chatActive` (`0x17243c`–`0x1729ba`). The handler then reaches
the common generic-agent event path. If unaffordable, it formats the
**shortfall** (cost minus credits), substitutes `#C` in game text 203,
opens the window without buttons, marks `choiceVisible` at offset
`0x1b`, and returns before the generic-agent event
(`0x173114`–`0x1731b2`, then `0x172f94`). The two
`ChoiceWindow::set(String const&,bool)` forms share the existing
title and button setup.

Focused ARM comparison for `OnTouchEnd()` rose from **31.9% to 35.7%
source-shape**, while strict fuzzy moved from **18.1% to 17.4%**. The
rebuilt function has 768 instructions versus 2,630 in the original.
It is neither linked-exact nor byte-exact. The focused unit retains
22 linked-exact and 7 byte-exact matches across 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

Only offer variants 1 and 7 are recovered in this package. Other
variants and the large `onKeyPress`/`startChat` paths remain incomplete.
Runtime touch and purchase behavior remains unverified while device
testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. Comparison-pair coverage is unchanged, and
the normalized linked-exact and byte-exact sets have no losses or
additions. Reports:
`D:/gof2-arm64-crypto/space-touch-accept-1-7-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-accept-1-7-20261102-full.json`.
