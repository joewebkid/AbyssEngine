# Main-menu 2D rendering (2026-11-05)

The original Android ARMv7 `ModMainMenu::OnRender2D()` begins at
`0x175c00` in `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
Its Thumb code sets the member canvas color to `0xffffffff`, renders
the cutscene, then draws either the touch window or the logo. In the
logo path it sets the calculated fade color on the member canvas and
calls `DrawImage2D` on `Globals::Canvas` with the logo image and two
`0x44` placement flags.

For the text pulse, the original calls `GetSystemTimeMillis` and `Sinf`
twice. It compares the first sine result with zero and selects the
positive or negative second result before multiplying by 255. The
result is the alpha byte in `SetColor(255, 255, 255, alpha)`; the prior
source passed an unrelated canvas address as the red byte. The code
then calls `GameText::getText(0xc7)` twice, gets the width of the
second result with `Globals::font`, and draws the first result centered
at `((w / 2) - (textWidth / 2), (h / 2) + (logoHeight / 2) + 10)`.
The final DrawString boolean is false.

Relocations identify the observed global roots: `Globals::Canvas`
at GOT `0x2101f4`, `Globals::appManager` at `0x20fd50`,
`Globals::font` at `0x21029c`, `Globals::gameText` at `0x2102a0`,
`Globals::w` at `0x210278`, and `Globals::h` at `0x210280`.
The recovered source uses those roots and removes four unwritten
pointer holders. On 64-bit hosts the member canvas field is only a
32-bit ARM layout slot, so the code uses `Globals::Canvas` there.

Focused ARMv7 verification compiles **209/209** translation units.
Source-shape rises **75.8% -> 98.2%**, and strict fuzzy rises
**45.7% -> 63.9%**. The function now has 145 generated instructions
versus 146 in the original, with full call-count and control-flow
matches, but it is not exact. Windows native and ARM64 static builds
pass; strict ARM64 shared linking succeeds. The recovered rendering
path has not been exercised on a device.

Full ARMv7 verification compares **4,323/4,524** original functions
in **4,770** pairs: mean source-shape **96.79%**, strict fuzzy
**86.78%**, **2,784 linked-exact**, **1,108 byte-exact**, 201 missing,
zero skipped and zero wrong-type matches. Compared with the preceding
full corpus, there is no lost comparison pair or exact-match member.
