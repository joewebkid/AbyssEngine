# SpaceLounge mission-location touch action

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)` at `0x172030`–`0x1723e4`.

In mode 2, a consumed button at index **3** marks the selected agent's
byte at offset `0x25` (`wasAskedForLocation`). It reads the mission
target station. If this is the current station, the scroll window
receives the agent name and game text **805**. If it is a different
station in the current solar system, it receives text **806**. Both
replies clear `chatScroll` and set mode **2** (`0x172030`–`0x172168`).

For a station outside the current system, the original gets
application module **5** and reads its `StarMap*` at offset `0x10`.
It reinitializes an existing map with `(true, mission, false, -1)` or
constructs one with those arguments and stores it back in the module.
It then marks `mapVisible` (`0x172180`–`0x1723e4`). The existing
`MGame::OnRelease()` code independently shows module 5 as owner of
this pointer and deletes it on release. The new `SpaceLounge` path
uses the same module slot, leaving ownership with that module.

Focused ARM comparison for the whole `OnTouchEnd()` function rose from
**27.5% to 31.9% source-shape** and **17.1% to 18.1% strict fuzzy**.
The rebuilt function has 672 instructions versus 2,630 in the
original. It is not linked-exact or byte-exact. The focused unit
retains 22 linked-exact and 7 byte-exact matches across 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

Live map and touch behavior remains unverified while device testing
is deferred. The remaining mode-2 index-0 offer action and the large
`onKeyPress`/`startChat` paths need reconstruction.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair and exact-match
sets are unchanged from the previous full report. Reports:
`D:/gof2-arm64-crypto/space-touch-location-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-location-20261102-full.json`.
