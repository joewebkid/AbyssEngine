# ModStation keyboard lounge and map routes

Date: 2026-11-02. Primary reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`ModStation::OnKeyPress(long long,long long)` at `0xd92a4` in the
REA/IDA analysis database.

Selected button 1 is unavailable before campaign step 13 and at
step 49; it shows text 528 in the primary choice window
(`0xd9662`–`0xd9760`). At ordinary stations it reinitializes an
existing `SpaceLounge` or constructs one, sets music parameter 0 to
2.0, stops sound 122, plays sound 108, and sets the lounge byte at
station offset `0x65` (`0xd9772`–`0xd9828`). Text 627 is a one-time
help prompt (`0xd9834`–`0xd9866`).

Station 88 with a pirate base takes a different ARM path: it creates
a `DialogueWindow` using texts 433 and 1597 and an image-parts
global, then plays agent-specific dialogue sound (`0xd967c`–
`0xd9730`). This route was restored in the subsequent
[hangar/dialogue pass](MODSTATION_KEY_HANGAR_ARM_2026-11-02.md).

Selected button 2 is unavailable before step 9 and at steps 48/49,
showing text 528. Excess ship load shows text 204; step 77 with a
ship other than index 37 shows text 326 (`0xd95e4`–`0xd9612`,
`0xd9878`–`0xd9888`, `0xd9bce`–`0xd9bd8`). The accepted path
reinitializes or constructs `StarMap(false, nullptr, false, -1)`,
enables jump-map mode when equipment 85 or an integrated jump drive
is present, and sets the map byte at offset `0x67`
(`0xd988c`–`0xd9b22`). It stops sound 122, sets music parameter 0
to 0.6, and shows one-time help text 628 from step 16 or text 631
before that (`0xd9b28`–`0xd9bca`). File-static booleans model the
observed process-lifetime help bytes; original symbol ownership
remains unverified.

Focused `OnKeyPress()` source-shape rises **35.4% → 60.7%** and
strict fuzzy **18.0% → 34.5%**, with 496 rebuilt instructions
versus 889 original. ARMv7 compiles 209/209 units; the focused
ModStation unit compares 36 symbols and retains **9 linked-exact /
3 byte-exact** matches. Windows and ARM64 static builds pass.
Focused report:
`D:/gof2-arm64-crypto/modstation-key-lounge-map-20261102-focused.json`.

The button-0 hangar route and the station-88 pirate dialogue were
restored in the subsequent hangar/dialogue pass. Live keyboard,
map and lounge behavior remain unverified while device work is
deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,762**
pairs at **96.19%** mean source-shape and **85.95%** strict fuzzy.
The normalized comparison-pair, **2,741 linked-exact**, and **1,096
byte-exact** sets are unchanged from the preceding report. There are
zero skipped and 201 missing originals. Full report:
`D:/gof2-arm64-crypto/modstation-key-lounge-map-20261102-full.json`.
