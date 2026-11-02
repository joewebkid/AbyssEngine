# SpaceLounge keyboard wingman offer

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)` in the REA/IDA analysis database.

In dialogue mode 2, answer 0 for offer 6 first checks
`Achievements::gotAllMedals()` (`0x16f3e2`). Without all medals,
credits below `Agent::getCosts()` cause text 203 with a formatted
shortfall, a choice without buttons, and an early return
(`0x16f3fa`–`0x16f494`). If that check passes, an existing wingman
list causes text 785 and another early return (`0x16f586`–
`0x16f5ae`). Otherwise the confirmation uses text 866 or 867,
replaces `#Q` with friends plus one and `#C` with the full cost,
sets `chatActive`, and reaches the generic-agent event update
(`0x16fcba`–`0x16fddc`). The keyboard gate differs from the
separately recovered touch gate, which checks an existing list before
credits and does not skip the credit check for all medals.

Confirming the keyboard choice draws reply text 850–852 and writes
the wingman count into `Status+0xd4`, copies the name list, stores
the race at `Status+0x2c`, sets the timer at `Status+0x30` to
600,000, and copies five image-part integers into a new array at
`Status+0x28` (`0x16e61c`–`0x16e652`, `0x16ecc4`–`0x16ed20`).
All medals cause a credit addition of the agent cost; otherwise
the cost is subtracted (`0x16ed24`–`0x16ed3e`, `0x16f3c0`–
`0x16f3c8`). The common tail marks the offer accepted and shows
the reply in mode 3. The previous keyboard path returned without
either the prompt or transaction.

Focused `onKeyPress()` source-shape rises **52.4% → 60.7%** and
strict fuzzy **14.3% → 22.3%**. The rebuilt function has 1,754
instructions versus 3,187 original. ARMv7 compiles 209/209 units;
the focused unit compares 42 symbols and retains **22 linked-exact /
7 byte-exact** matches. Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/space-key-offer6-20261102-focused.json`.

Live hiring, credit and UI behavior remain unverified while device
work is deferred. Keyboard mission offers 0 and 5 and the
`onKeyPress` return ABI remain for separate review.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.18%** mean source-shape and **85.94%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the previous report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/space-key-offer6-20261102-full.json`.
