# SpaceLounge mission reply and capacity gates

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)`.

For offers **0 and 5**, a set `singleOffer` byte at ARM offset
`0x36` takes an early reply path (`0x172242`–`0x172248`). The
original writes mode **2**, clears `singleOffer`, and calls
`Globals::getAgentMissionText()` for the selected agent
(`0x172fa8`–`0x172fca`). If that agent has a nonempty mission,
it substitutes the formatted **reward plus bonus** followed by
`SpaceLounge::title` for `#C` (`0x172fd0`–`0x173056`). It displays
the text with the agent's name, clears `chatScroll` at offset
`0x2c`, and returns before the common generic-agent event
(`0x17307a`–`0x1730bc`).

Without `singleOffer`, mission type **0** tests
`Ship::spaceAvailable(getProductionGoodAmount())` (`0x17224c`–
`0x172a02`). On failure it substitutes that amount for `#Q` in
game text **337**. Mission type **11** compares the same mission
amount to `Ship::getMaxPassengers()` (`0x17225c`–`0x17228a`);
when too large it uses text **338** with the amount at `#Q`.
Both capacity failures open `ChoiceWindow` without buttons, mark
`choiceVisible` at offset `0x1b`, and continue through the common
generic-agent event path (`0x172c5a`–`0x172cfc`). The recovered
source follows these two gates. The subsequent mission confirmation
for a ship with sufficient capacity remains incomplete.

Focused ARM comparison for `OnTouchEnd()` rose from **70.8% to 78.3%
source-shape** and **26.6% to 27.6% strict fuzzy**. The rebuilt
function has 1,978 instructions versus 2,630 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches across 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass.

Runtime dialogue and capacity behavior remain unverified while device
testing is deferred. Focused report:
`D:/gof2-arm64-crypto/space-touch-mission-reshow-capacity-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report:
`D:/gof2-arm64-crypto/space-touch-mission-reshow-capacity-20261102-full.json`.
