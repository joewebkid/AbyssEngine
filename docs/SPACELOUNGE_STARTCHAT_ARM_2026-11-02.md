# SpaceLounge conversation start

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`,
SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::startChat()` at `0x16aaa0`.

The old source read dialogue through an uninitialized static pointer
and invented text IDs `0x100`–`0x133`. It could not safely enter
conversation. The original instead obtains the selected agent, moves
the iPad panel beside that agent's rectangle (`0x16aab4`–`0x16ad48`),
then branches by generic status, event, acceptance, and offer.

The recovered non-generic branch follows the original event/accepted
text `858` (`0x16adb2`–`0x16ae5e`), offer 8 mod-installed gate
(`0x16b164`–`0x16b236`), offer 10 early greeting gate
(`0x16ca58`–`0x16cafa`), and offer 8/9/10/4/blueprint text
construction (`0x16bbb4`–`0x16d53a`). It appends a random reply
`841`–`843`, stores the mission string, and selects conversation
mode 2 as at `0x16d53c`–`0x16d618`. For the mod offer, the four
numeric values `40, 30, 1, 20` come from the original data table
at `0x1fdcf4` (second word of each two-word entry).

The generic-agent fallback now uses the restored
`Globals::getAgentMissionText()` and the original final station/system
name, mode, keypress, and sound calls (`0x16c85a`–`0x16c8d0`).
Its intermediate greeting and individual offer paths remain
incomplete. Neither dialogue text nor state changes have been
checked in a running game because device work is deferred. The
remaining generic branches and offer-specific string temporary
shapes are the next priority.

Focused ARM source-shape for `startChat()`: **4.8% → 34.9%**;
strict fuzzy **2.3% → 7.9%**. The compiled function has 1,317
instructions against 4,856 in the original, so this remains a partial
reconstruction. Focused unit: 42 comparisons, 22 linked-exact,
7 byte-exact; ARMv7 compilation succeeds. Windows and ARM64 static
builds pass. Focused report:
`D:/gof2-arm64-crypto/space-startchat-recovery-20261102-focused.json`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs. Mean source-shape remains **96.16%** and strict fuzzy
**85.94%** at corpus rounding. The **2,741 linked-exact** and **1,096
byte-exact** sets, as well as the normalized comparison-pair set, are
identical to the previous full report. There are zero skipped and 201
missing originals. Full report:
`D:/gof2-arm64-crypto/space-startchat-recovery-20261102-full.json`.

## Known generic agents: accepted offers and mission bonus

The next ARM-backed slice restores the `isKnown() && offer != 7`
branch at `0x16ad70`. Accepted offers show text `857` for offer 5,
`859` for offer 6 or mission type 12, and `858` otherwise, then
enter mode 3 with the original sound gate
(`0x16aeb2`–`0x16ce2a`). The previous fallback instead generated
a new mission description and entered mode 1.

For a known, unaccepted offer-0 mission whose type is neither 8 nor
12, the source now computes the standing bonus, rounds it by the
original remainder formula, stores it on the mission, and formats
the positive bonus in the lounge title
(`0x16c930`–`0x16ca3c`, `0x16d61e`–`0x16d740`). It then uses the
existing mission-text function and the original final station,
system, keypress, and sound path. Text output and reward changes
remain unverified in the running game.

Focused `startChat()` source-shape rises **34.9% → 42.2%**
from the preceding slice; strict fuzzy rises **7.9% → 10.8%**.
The new ARM function has 1,629 instructions versus 4,856 original.
The focused unit retains 22 linked-exact and 7 byte-exact matches
over 42 comparisons, with 209/209 ARMv7 translation units compiled.
Windows and ARM64 static builds pass. Focused report:
`D:/gof2-arm64-crypto/space-startchat-known-20261102-focused.json`.

The subsequent full corpus remains **4,323/4,524** originals and
**4,762** comparison pairs, with **96.16%** mean source-shape,
**85.94%** strict fuzzy, **2,741 linked-exact**, **1,096 byte-exact**,
zero skipped and 201 missing. Normalized pair and exact-symbol sets
are unchanged from the preceding full report. Full report:
`D:/gof2-arm64-crypto/space-startchat-known-20261102-full.json`.
