# SpaceLounge second-button touch action

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)` at `0x171c5e`–`0x172412`.

In mode 2, the original tests each button. When button index **1**
accepts the release, it sets the scroll text to the selected agent's
name and a randomly selected game text ID from **845–849**. It then
sets `chatScroll` and `mode` to zero and increments the 32-bit Status
field at offset `0xe0` (`0x172092`–`0x1720f8`). The field's meaning
is not established here; the source retains its existing `field_e0`
name. The former source had no action for this button.

After a consumed button, the original checks whether the selected
agent is generic and sets event **1** (`0x1723f4`–`0x172412`). The
former source guarded that event with `index >= 5`, although this
button array has five entries. The recovered path applies the event
test to each consumed button, including index 1. Actions for the other
button indices remain incomplete.

Focused ARM comparison for the whole `OnTouchEnd()` function rose from
**16.1% to 18.9% source-shape** and **13.5% to 14.7% strict fuzzy**.
The rebuilt function has 404 instructions versus 2,630 in the
original. It is not linked-exact or byte-exact. The focused unit
retains 14 linked-exact and 2 byte-exact matches across 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

Live behavior remains unverified while device testing is deferred.
The main remaining gap is the other mode-2 button actions, followed by
the large `onKeyPress` and `startChat` paths.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.10%** mean source-shape and **85.86%** strict
fuzzy (unchanged at reported precision). It retains **2,733
linked-exact** and **1,091 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair and exact-match
sets are unchanged from the previous full report. Reports:
`D:/gof2-arm64-crypto/space-touch-end-button1-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-end-button1-20261102-full.json`.
