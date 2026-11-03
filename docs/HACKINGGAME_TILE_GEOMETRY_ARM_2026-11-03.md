# HackingGame tile and arrow placement (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `HackingGame::render2D` at `0x14f1e4` places each current
tile at a vertical row offset equivalent to `(row - 2) * tileHeight`
before adding half the screen height, subtracting half the main-image
height, and applying its animation delta and layout offset `+0x310`.
The reconstructed source used `row * tileHeight`, shifting both tile
rows downward by two tile heights. The row term now matches ARM.

For both left and right arrows, original ARM subtracts the **full**
tile height from half the screen height, then subtracts half the
main-image and arrow heights and adds layout offset `+0x310`. The
reconstruction subtracted only half the tile height. Both arrow Y
coordinates now use the full height. The solved-state image-flash
branch was also checked: the alternative image set is selected on
even `wonTimer / 200` intervals, as the source already does.

Focused ARMv7 verification compiles 209/209 translation units.
`render2D` source-shape changes from 79.2% to 79.4%, and strict fuzzy
from 16.5% to 14.8%. It remains nonexact, with 490 reconstructed
instructions against 685 original. The `HackingGame` unit retains
11 linked-exact and 10 byte-exact functions. Focused report:
`_work/hacking_tile_arrow_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. No permitted
device rendering test has been run; live tile and arrow placement
remains unverified. Redmi testing stays stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
remains 96.64% and mean strict fuzzy 86.58% at two-decimal rounding.
Linked-exact remains 2,776 and byte-exact remains 1,107. Neither
comparison pairs nor exact-match sets lose entries. Full report:
`_work/hacking_tile_geometry_full_20261103.json`.

Next priority: audit the remaining `render2D` draw-call sequence and
resource sizing against ARM; a live screen check on a permitted
device is still needed.
