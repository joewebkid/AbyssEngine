# ModStation keyboard hangar route

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`ModStation::OnKeyPress(long long,long long)` at `0xd92a4` in the REA/IDA
analysis database. This pass changes only selected button 0.

The original denies hangar entry before campaign step 5 and at steps
48, 49, and 56, showing text 528 in the primary choice window
(`0xd93f8`–`0xd9400`). Ordinary stations create and initialize the
hangar on first use. A dirty lounge takes priority over a dirty
missions window: it destroys and recreates the hangar and clears that
source's flag; otherwise a dirty missions window does the same
(`0xd98a8`–`0xd9948`). This priority matters when both windows have
pending updates.

The route records up to ten hangar tab-button positions, records the
full button count, and sums cargo item amounts for indices 132–153
into station offset `0xd0` (`0xd9950`–`0xd9a3c`). It sets music
parameter 0 to 1.0, stops sound 122, plays sound 95, saves the
engine's post-effect state at station offset `0x12c`, activates the
hangar byte at offset `0x66`, and offers one-time help text 622
(`0xd9a58`–`0xd9ac8`). A file-static boolean models the observed
process-lifetime help byte; the original variable's ownership is
unverified.

Station index 4 with a pirate base follows a distinct path: a
`DialogueWindow` with texts 432 and 1609, portrait parts, and
agent-specific dialogue sound (`0xd9416`–`0xd9730`). The original
portrait array at `.data` address `0x215690` starts with five
32-bit values `{2, 0, 0, 0, 0}` (file offset `0x214690`). The
station-88 lounge path uses texts 433 and 1597 and a distinct
portrait array at `.bss` address `0x217bf8`, which starts as zero.
REA xrefs show that second array is read by `OnKeyPress`; they do
not show a direct write. Both paths create a temporary `Agent`,
query `Globals::getDialogueSoundId`, play the result and delete
the agent. They open a dialogue only when none exists and set the
dialogue byte at station offset `0x69`. The source now implements
these routes. The original portrait arrays' full runtime mutation
behavior remains unverified.

Focused `OnKeyPress()` source-shape rises **60.7% → 93.1%** and strict
fuzzy **34.5% → 48.7%**, with 865 rebuilt instructions versus 889
original. ARMv7 compiles 209/209 units; the focused ModStation unit
compares 36 symbols and retains **9 linked-exact / 3 byte-exact**
matches. Windows and ARM64 static builds pass. Focused report:
`D:/gof2-arm64-crypto/modstation-key-hangar-pirate-20261102-focused.json`.

Live keyboard, hangar, and dialogue behavior remains unverified
while device work is deferred. Next offline priority: investigate
the remaining `OnKeyPress` shape and unverified portrait-array
mutation using ARM xrefs and independent platform evidence.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.20%** mean source-shape and **85.95%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/modstation-key-hangar-pirate-20261102-full.json`.
