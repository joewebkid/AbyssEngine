# SpaceLounge draw path

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::draw()` at `0x170c04`–`0x170d04`.

The original function has three paths: item list, star map, and lounge.
When the item list shows a 3D ship, it sets the canvas color to opaque
black, fills the complete `GetWidth()` by `GetHeight()` rectangle, then
restores opaque white before drawing the list and footer (`0x170c04`–
`0x170c70`). The previous source dereferenced an unbound local slot and
passed the canvas pointer as a color and rectangle coordinate. The
recovered source uses `Globals::Canvas` with the observed arguments.

The lounge path gets text ID **398** through `Globals::gameText`, copies
it into a temporary `String`, draws the header, draws the lounge,
selects the footer according to `(mode & ~1) == 2`, and draws the choice
window when chat, popup, or choice is active (`0x170cac`–`0x170d04`).
The former text and layout slots were unbound; the source now uses
`Globals::gameText` and `Globals::layout`. The map path still delegates
to `starMap->draw()`.

Focused ARM source-shape for `draw()` improved from **75.4% to 99.2%**;
strict fuzzy improved from **45.7% to 95.1%**. The original and rebuilt
function each have 122 compared instructions. It is not linked-exact
or byte-exact. The focused unit retains 14 linked-exact and 2
byte-exact matches across 42 comparisons. ARMv7 compiles 209/209 units;
Windows and ARM64 static builds pass. Runtime drawing on a phone is
deferred until the device is available.

The remaining major `SpaceLounge` gaps are input actions and dialog
state (`OnTouchEnd`, `onKeyPress`, `startChat`) and parts of `init()`.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs, with **96.10%** mean source-shape and **85.85%** strict
fuzzy. It retains **2,733 linked-exact** and **1,091 byte-exact**
matches, zero skipped, and 201 missing originals. Compared with the
previous full report, the comparison-pair set and both normalized
exact-match sets are unchanged. Reports:
`D:/gof2-arm64-crypto/space-draw-20261102-focused.json` and
`D:/gof2-arm64-crypto/space-draw-20261102-full.json`.
