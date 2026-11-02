# SpaceLounge keyboard standing offer

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)` in the REA/IDA analysis database.

In dialogue mode 2, answer 0 for agent offer 7 compares credits with
`Agent::getCosts()` (`0x16f4a4`–`0x16f4ae`). When funds are short, it
shows text 203 with `#C` replaced by the shortfall, opens a choice
without buttons, and returns before the generic-agent event update
(`0x16f4ba`–`0x16f53a`). When affordable, it shows text 885 with the
full cost, opens a confirming choice, marks `chatActive`, and reaches
the generic-agent event update (`0x16f7ea`–`0x16f85c`,
`0x16fdc6`–`0x16fddc`).

Confirming that choice selects reply text 850–852, calls
`Standing::rehabilitate(agent->getRace())`, subtracts the agent cost
from credits, marks the offer accepted, and displays the reply in
mode 3 (`0x16e61c`–`0x16e652`, `0x16ed4e`–`0x16ed6e`,
`0x16ff9a`–`0x16ffe6`). The previous keyboard path returned before
either the prompt or transaction. The separate touch prompt was
already recovered.

Focused `onKeyPress()` source-shape rises **49.8% → 52.4%** and
strict fuzzy **13.7% → 14.3%**. The rebuilt function has 1,471
instructions versus 3,187 original. The focused unit compares 42
symbols and retains **22 linked-exact / 7 byte-exact** matches;
ARMv7 compiles 209/209 units. Focused report:
`D:/gof2-arm64-crypto/space-key-offer7-20261102-focused.json`.
Windows and ARM64 static builds pass.

Live purchase and UI behavior remain unverified while device work is
deferred. Keyboard handling for offers 0, 5, and 6 remains incomplete;
the recovered `onKeyPress` declaration also needs a separate return
ABI review.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.17%** mean source-shape and **85.94%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/space-key-offer7-20261102-full.json`.
