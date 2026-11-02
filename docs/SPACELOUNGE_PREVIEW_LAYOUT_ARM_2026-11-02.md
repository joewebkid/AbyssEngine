# SpaceLounge ARM32 layout and item preview

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Primary functions: `SpaceLounge::SpaceLounge()` at `0x169ae6`–
`0x169f06`, `init()` at `0x16a774`–`0x16a7e4`, `update()` at
`0x17075a`, `OnTouchBegin()` at `0x171804`, and `OnTouchEnd()` item
preview path at `0x171f9e`–`0x172398`.

The original constructor places `title` at `0xa4` and writes the
initialization byte at `0xb0`. The original `init()` stores the
current post-effect state at `0xb1` and later marks initialization at
`0xb0`; `update()` reads `0xb0`. The touch handlers write `touchDown`
at `0xb2` and touch coordinates at `0xb4`/`0xb8`. Mode is at `0x14`,
chat at `0x19`, and list visibility at `0x1c`. The previous C++ class
was four bytes short from mode onward and did not model the byte at
`0xb1`. The recovered class adds the unknown word at `0x10`, overlays
the word at `0x18` with its three observed flag bytes, and adds the
post-effect byte. ARM32 `offsetof` assertions lock these positions.
The ARM compiler's record layout was inspected before accepting the
change; it now matches the observed offsets.

In mode 2, button index **2** lazily constructs a `ListItemWindow`.
Offers **2** and **9** preview an `Item` selected by the agent's sell
item index. Offer **10** first saves whether the engine post effect is
active, disables effect `0x1400000`, and previews a `Ship` from the
ship table. Other offers search the player's blueprints for the
agent's sell blueprint index and preview the corresponding item. The
original passes zeros to `ListItemWindow::set` and marks the list
visible after these branches (`0x171f9e`–`0x172398`); the recovered
source follows this path. `init()` now also saves the initial post
effect state and sets its completion flag, which were missing from
the prior source.

Focused `OnTouchEnd()` source-shape rose from **21.7% to 27.5%** and
strict fuzzy from **15.5% to 17.1%**. `init()` rose **70.4% to
72.6%** source-shape; `OnTouchBegin()` rose **94.4% to 96.8%** and
`OnTouchMove()` **94.3% to 96.7%** as a result of the corrected layout.
The focused `SpaceLounge` unit gained eight linked-exact and five
byte-exact functions, with no exact-match losses. It now has **22
linked-exact** and **7 byte-exact** matches over 42 comparisons.
ARMv7 compiles 209/209 units; Windows and ARM64 static builds pass.

The exact role of the unknown word at `0x10` is unconfirmed. Live
preview and post-effect restoration have not been checked on a device.
Button indices 0 and 3 and the large `onKeyPress`/`startChat` paths
remain incomplete.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.12%** mean source-shape and **85.93%** strict
fuzzy, up from **96.10% / 85.86%**. It has **2,741 linked-exact** and
**1,096 byte-exact** matches, zero skipped, and 201 missing originals.
The normalized comparison-pair set is unchanged. The exact-match
sets have no losses and gain precisely eight linked-exact and five
byte-exact `SpaceLounge` functions. Reports:
`D:/gof2-arm64-crypto/space-touch-preview-20261102-focused.json`
and `D:/gof2-arm64-crypto/space-touch-preview-20261102-full.json`.
