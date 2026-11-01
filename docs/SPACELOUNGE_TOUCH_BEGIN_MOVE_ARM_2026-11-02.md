# SpaceLounge touch begin and move

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchBegin(int,int)` at `0x17181c`–`0x17191c` and
`SpaceLounge::OnTouchMove(int,int)` at `0x171940`–`0x171a3e`.

Both original functions forward touches to `Globals::layout` after
their mode-specific choice, map, list, or agent/button handling. The
previous source dereferenced two zero-initialized local slots instead
of that global and would fail on normal lounge touches. The source now
calls the actual layout object. `OnTouchBegin` also shares its layout
and scroll-window forwarding after the list and non-list branches, as
the original does; this removes duplicate calls from the source while
preserving when they run.

Focused ARM source-shape for `OnTouchMove` rose from **92.3% to 94.3%**
and strict fuzzy from **36.1% to 48.9%**. `OnTouchBegin` rose from
**92.5% to 94.4%** source-shape and **55.0% to 58.1%** strict fuzzy.
The rebuilt functions have 111 and 114 instructions respectively,
versus 110 and 113 in the original. Neither is exact. The focused unit
retains 14 linked-exact and 2 byte-exact matches across 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

This only repairs the layout forwarding path. The much larger
`OnTouchEnd`, `onKeyPress`, and `startChat` behaviors still need
reconstruction. Live touch behavior remains unverified while device
testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.10%** mean source-shape and **85.86%** strict
fuzzy. It has **2,733 linked-exact**, **1,091 byte-exact**, zero
skipped, and 201 missing originals. The normalized comparison-pair and
both exact-match sets are unchanged from the preceding full report.
Reports: `D:/gof2-arm64-crypto/space-touch-begin-move-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-begin-move-20261102-full.json`.
