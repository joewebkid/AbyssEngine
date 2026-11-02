# SpaceLounge keyboard map and preview exit

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

In dialogue mode **3**, the confirm keys clear `chatAnswer` and, for
a non-offer-1 agent with the byte at lounge offset `0x18` set, open a
StarMap for `Agent::getSellSystemIndex()`. The original obtains
application module **5**, reuses its map with
`StarMap::init(false, nullptr, true, index)` or constructs that map
with the same arguments (`0x16e486`–`0x16e5fc`). It clears the byte
at offset `0x18` and sets `mapVisible` at offset `0x34`
(`0x16e600`–`0x16e608`). The original sets the offset-`0x18` byte
when accepting offer **4**, after revealing its system (`0x16ec7e`–
`0x16ec8e`). That acceptance path is still absent from the current
source, so this map branch cannot yet be exercised in the current
native build. Recovering that transaction is the next bounded step.

In mode **2**, when `listVisible` at offset `0x1c` is set, the back
key `0x40000` clears it and restores the saved post-effect state
through `Engine::SetPostEffect(0x1400000, savedPostEffect)`
(`0x16e438`–`0x16e466`). Other keys leave the preview active.

Focused ARM comparison for `onKeyPress()` rose from **5.6% to 8.8%
source-shape** and **1.4% to 4.3% strict fuzzy**. The rebuilt
function has 225 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-map-preview-20261102-focused.json`.

Runtime keyboard and preview behavior remains unverified while device
testing is deferred. Most `onKeyPress()` option handling is still
absent, especially mode-2 choice navigation and acceptance.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-map-preview-20261102-full.json`.
