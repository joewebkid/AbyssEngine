# SpaceLounge keyboard mission offers and credit correction

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)` in the REA/IDA analysis database.

In dialogue mode 2, answer 0 for offers 0 and 5 rejects a type-0
mission when its cargo does not fit, using text 337 and the required
quantity, or a type-11 passenger mission when its passenger count
exceeds the ship limit, using text 338 (`0x16f102`–`0x16f1b0`,
`0x16f5b4`–`0x16f5d2`). Otherwise generic agents use text 865 with
`#C` expanded from reward plus bonus and `#M` from mission type;
story agents use text 863. An existing freelance mission adds text
864 before the confirmation (`0x16f5d8`–`0x16f79e`). The common
path opens a confirming choice, marks `chatActive`, and updates a
generic agent's event (`0x16fda4`–`0x16fddc`).

On confirmation, the original first draws reply text 850–852. In
hardcore mode it requires and deducts one tenth of reward plus bonus;
insufficient funds instead show text 203 with the deposit shortfall,
clear `chatActive`, and return (`0x16e692`–`0x16e758`). An existing
freelance mission is removed: types 0, 3, and 5 remove the first
unsaleable cargo item 116; type 11 clears passengers. A non-generic
old mission agent loses its accepted flag, then the freelance slot
receives `Mission::empty` (`0x16f074`–`0x16fe2c`).

The new type-0 mission adds unsaleable cargo item 116 in the mission
quantity; type 11 sets passengers. Type 12 uses reply text 856;
other types append a random text 853–855. For outside missions, the
original increments `Status+0xe8` and `Status+0xec` when the agent
has not been asked for difficulty and location, respectively. It
assigns the mission and agent, reinitializes the lounge, marks a
hangar update and acceptance, then displays the reply in mode 3
(`0x16fe32`–`0x16ffe6`).

The prior shared transaction charged `sellItemPrice` before handling
offers 6 and 7, so hiring a wingman or restoring standing could
charge twice. The ARM branches at `0x16ecc4`–`0x16ed6e` charge only
their respective agent cost. The shared charge now applies only to
offers 2, 3, and 4; offers 8–10 retain their separate multiplier.

Focused `onKeyPress()` source-shape rises **60.7% → 75.9%** and
strict fuzzy **22.3% → 30.7%**. The rebuilt function has 2,304
instructions versus 3,187 original. ARMv7 compiles 209/209 units;
the focused unit compares 42 symbols and retains **22 linked-exact /
7 byte-exact** matches. Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/space-key-mission-20261102-focused.json`.

Live mission replacement, cargo, passenger, credit and UI behavior
remain unverified while device work is deferred. The recovered
`onKeyPress` return ABI and remaining source-shape differences need
separate review.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.18%** mean source-shape and **85.94%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/space-key-mission-20261102-full.json`.
