# Agent mission and offer dialogue text

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`Globals::getAgentMissionText(Agent *)` at `0xe5ee4`.

The old function dereferenced several static null handles before
checking its input and returned almost no useful dialogue. The
recovered source uses the existing `Globals::status`, `gameText`,
`layout`, `rnd`, `generator`, `galaxy`, and `items` objects. A null
agent returns an empty string, matching `0xe6050`. For a generic
agent, the original dispatches on offers **0, 1, 2, 5, 6, 7**
(`0xe5f08`–`0xe5f22`):

- Offer 0 formats mission description, goods, quantity, destination,
  target, stored random line, standing bonus, and reward. Mission
  type 12 overrides the generic text with the reward template 798;
  an unavailable mission uses text 820. The reward bonus arithmetic
  and rounding are checked against ARM instructions
  `0xe73e8`–`0xe742a`.
- Offer 1 seeds the random source from station index, agent race and
  name length, replaces planet/name/ore tokens, then resets the
  random source (`0xe61d6`–`0xe6392`).
- Offers 2 and 5 format item or production goods and credits. Offer 5
  creates its mission if missing; offer 2 uses the per-item price
  (`0xe6626`–`0xe663a`) when the quantity is at least two.
- Offer 6 formats wingman count, costs and optional wingman name.
  Offer 7 selects the standing-dependent rehabilitation price or
  text 883 when the race is not an enemy.

For a non-generic agent, an already-seen event or accepted offer
uses text **858** (`0xe605c`–`0xe608a`). A fresh offer increments
status field `0xd0`; a previously installed mod returns text 858
and reverses that increment (`0xe60c0`–`0xe6136`). Offers 8–10
format their prices with the same options-byte `0x38` multiplier as
the purchase dialog; offer 8 first derives its price from the ship
price and `Agent::getModPricePercentage`. Offers 9 and 10 substitute
item/ship names and quantities. Offer 4 substitutes the system name;
the remaining branch substitutes the blueprint item. A random text
from **841–843** is appended to fresh-offer dialogue.

Focused `getAgentMissionText` source-shape rose from **3.0% to
72.1%** and strict fuzzy from **1.2% to 20.1%**. The rebuilt
function has 2,122 instructions versus 3,318 in the original; it
is neither linked-exact nor byte-exact. The focused `Globals` unit
retains 17 linked-exact and 1 byte-exact matches over 50 comparisons.
Windows and ARM64 static builds pass; ARMv7 compiles 209/209 units.
Focused report: `D:/gof2-arm64-crypto/gamt-20261102-focused.json`.

The remaining instruction gap is substantial. The original's
dialogue details and repeated-string behavior still need runtime
confirmation on device; in particular, the decompiler reuses
temporary string locations in some offer paths. Static builds and
instruction similarity cannot prove the rendered text or all
ownership behavior.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs. Mean source-shape rose **96.14% → 96.15%** while
strict fuzzy remains **85.93%** at reported precision. It retains
**2,741 linked-exact** and **1,096 byte-exact** matches, zero skipped,
and 201 missing originals. The normalized comparison-pair,
linked-exact, and byte-exact sets are unchanged from the previous
full report. Full report:
`D:/gof2-arm64-crypto/gamt-20261102-full.json`.
