# SpaceLounge keyboard option navigation and short replies

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::onKeyPress(int)`.

In mode **2** without an active confirmation, the original computes
the number of options before processing keys: **5** for a non-null
outside mission, otherwise **4** for offers 2, 3, 8, 9 or 10, and
**3** for other offers (`0x16e3d0`–`0x16e8ee`). Key `0x4000`
selects the previous option with wraparound; `0x8000` selects the
next (`0x16e580`–`0x16e782`). Keys `0x10000` and `0x20000`
dispatch the selected option (`0x16e790`–`0x16e79a`). The old
source instead scrolled text or closed the agent window.

The recovered dispatch covers three short actions:

- Option **1** shows a random reply from game texts **845–849**,
  clears chat scroll, enters mode 3 and increments status field
  `0xe0` (`0x16e9c4`–`0x16ea2a`).
- Option **2** shows `Globals::getAgentMissionText()` under the
  agent's name, clears chat scroll, stays in mode 2 and increments
  status field `0xe4` (`0x16e8f2`–`0x16e93e`).
- Option **4** sets `Agent::wasAskedForDifficulty`, converts the
  mission difficulty to game text `808 + int((difficulty / 10.0f)
  * 5.0f)`, displays it, clears chat scroll and stays in mode 2
  (`0x16e948`–`0x16e9bc`).

Each recovered option continues through the original generic-agent
`setEvent(1)` path (`0x16fdc6`–`0x16fddc`). Options **0** and
**3** remain incomplete and currently have no keyboard action.
`Globals::getAgentMissionText()` itself has a low source-shape score
and unverified semantics; option 2 is structurally restored but not
claimed to work at runtime.

Focused ARM comparison for `onKeyPress()` rose from **14.6% to 20.4%
source-shape** and **5.0% to 7.8% strict fuzzy**. The rebuilt
function has 508 instructions versus 3,187 in the original. It is
neither linked-exact nor byte-exact. The focused unit retains 22
linked-exact and 7 byte-exact matches over 42 comparisons. ARMv7
compiles 209/209 units; Windows and ARM64 static builds pass. Focused
report: `D:/gof2-arm64-crypto/space-key-options-20261102-focused.json`.

Runtime keyboard behavior remains unverified while device testing is
deferred. Next offline priority: mode-2 options 0 and 3, and the
mission-text implementation.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.13%** mean source-shape and **85.93%** strict
fuzzy (unchanged at reported precision). It retains **2,741
linked-exact** and **1,096 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair, linked-exact,
and byte-exact sets are unchanged from the previous full report.
Full report: `D:/gof2-arm64-crypto/space-key-options-20261102-full.json`.
