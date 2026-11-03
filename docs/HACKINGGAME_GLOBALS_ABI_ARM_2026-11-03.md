# HackingGame globals and object layout (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Original `HackingGame::render2D` at `0x14f1e4` reads the exported
`Globals::Canvas`, `Globals::w`, `Globals::h` and `Globals::layout`.
It calls `PaintCanvas::GetImage2DHeight` directly for image sizes.
The reconstructed renderer instead used five private null pointer
or function slots for these values, and `PaintCanvas::gCanvas`,
which has no assignment in the recovered source. Its ARM build had
only 66 instructions against 685 original because the undefined
null reads let the compiler discard most drawing work. The renderer
now reads the confirmed globals and calls the canvas methods.

The original constructor at `0x14ecc8` writes `type` at `+0x124`,
images starting at `+0x4c`, and reward fields at `+0x134..+0x13c`.
`render2D`, `update`, and the rotation functions also use `target`
at `+0x04`, `current` at `+0x1c`, rotation flags at `+0x128/+0x129`,
and won timer at `+0x130`. Two unused 32-bit fields in the recovered
class had shifted all of these offsets by eight bytes. Removing
them restores the 0x320-byte layout, guarded by offset and size
assertions. The constructor now calls `Image2DCreate` on
`Globals::Canvas`, as in the original, instead of calling an unwritten
function pointer.

Original `rotateLeftCW(bool)`, `rotateRightCW(bool)` and `update(int)`
call `FModSound::play` through `Globals::sound`. Three unwritten
private sound pointer slots are replaced with that confirmed global.

Focused ARMv7 verification compiles 209/209 translation units.
`render2D` source-shape improves from 18.7% to 79.2%, strict fuzzy
from 4.5% to 16.5%, and generated instructions from 66 to 492
against 685 original. Both constructor symbols improve from 50.2%
to 78.6% source-shape and from 17.6% to 43.2% strict fuzzy.
The unit gains nine linked-exact and eight byte-exact functions;
`update(int)` becomes linked-exact. `render2D` and the constructors
remain nonexact. Focused report:
`_work/hacking_render_abi_sound_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. These checks
do not demonstrate a live hacking screen or audio playback; both
remain unverified. Redmi testing stays stopped.

The full ARM corpus compares 4,323/4,524 original functions in
4,769 pairs, with 201 missing and zero skipped. Mean source-shape
improves from 96.60% to 96.64%, and strict fuzzy from 86.49% to
86.58%. Linked-exact rises from 2,767 to 2,776 and byte-exact from
1,099 to 1,107. Neither comparison pairs nor exact-match sets lose
entries. Full report: `_work/hacking_globals_abi_full_20261103.json`.

Next priority: recover the separate tile-placement and solved-state
branches inside `render2D` against ARM. Its 492 generated instructions
still fall short of the original 685, and the rendered screen has not
been observed on a permitted device.
