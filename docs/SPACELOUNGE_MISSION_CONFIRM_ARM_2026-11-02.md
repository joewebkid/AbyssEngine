# SpaceLounge mission confirmation after capacity checks

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

For offers **0 and 5**, after the cargo/passenger capacity gates, the
original tests `Agent::isGenericAgent()` (`0x172a08`). A generic agent
starts from game text **865**, substitutes the formatted mission reward
plus bonus for `#C` (`0x172a1c`–`0x172a96`), and substitutes game text
`Mission::getType() + 354` for `#M` (`0x172ad2`–`0x172b14`). In
hardcore mode, it appends a newline, game text **27**, `": "`, and a
formatted **(reward + bonus) / 10** (`0x172b2e`–`0x172bbc`). The
division is visible in the ARM instructions at `0x172b8a`–`0x172b8e`
(`MOVS R1,#10; BLX __aeabi_idiv`); the decompiler pseudocode omits
that operation. A non-generic agent uses text **863** (`0x172be2`–
`0x172bf0`).

For either agent type, a nonempty freelance mission appends a space
and text **864** (`0x172bf8`–`0x172c40`). The resulting message opens
the choice window with buttons and marks `chatActive`; it continues to
the existing generic-agent event path. Capacity failures remain on
their own no-buttons path and are not overwritten.

Focused ARM comparison for `OnTouchEnd()` rose from **78.3% to 86.1%
source-shape** and **27.6% to 28.9% strict fuzzy**. The rebuilt
function has 2,267 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report:
`D:/gof2-arm64-crypto/space-touch-mission-confirm-20261102-focused.json`.

Live mission dialogue is unverified while device testing is deferred.
The remaining `OnTouchEnd()` instructions and mode-specific behavior
still need separate recovery and verification.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-mission-confirm-20261102-full.json`.
