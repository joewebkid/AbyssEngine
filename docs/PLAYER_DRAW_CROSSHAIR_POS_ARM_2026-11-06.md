# PlayerEgo draw crosshair coordinates (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `PlayerEgo::draw`, all four `DrawImage2D` paths load the same GOT
slot at `0x21027c`. Its `R_ARM_GLOB_DAT` relocation names the
12-byte static `PlayerEgo::crosshairPos` at `0x217b9c`. The original
loads its first two floats, converts each to an integer, and passes
them as the image coordinates. The relevant paths are the locked
plasma reticle (`0xa1e06`), unlocked plasma reticle (`0xa1e9c`),
blinking reticle (`0xa1e1c`) and normal reticle (`0xa1e66`). The
canvas is read from `Globals::Canvas` through GOT slot `0x2101f4`
for the color and draw calls.

The reconstruction used four distinct null float pointers for these
coordinates, so entering any of those paths could dereference null.
All four now read the existing static `crosshairPos`. The calls also
use the typed canvas pointer directly, removing the `void* -> long
-> PaintCanvas*` conversion that could truncate a pointer on 64-bit
Windows. All five canvas calls now use the confirmed
`Globals::Canvas` source.

Focused ARMv7 verification compiles **211/211** translation units.
`draw` rises from **88.5% to 95.7%** source-shape and from **61.2% to
88.6%** strict fuzzy; it remains non-exact at 214 original versus
197 reconstructed instructions. Remaining differences include local
matrix initialization order and register allocation. Windows native
and ARM64 static builds pass, as does strict ARM64 shared linking.
The raw 211-object ARM32 unmatched-symbol inventory stays **229**,
with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: **96.81%** mean source-shape, **86.88%** strict fuzzy,
**2,799 linked-exact**, **1,113 byte-exact**, 201 missing, zero skipped
and zero wrong-type. Exact-match membership is unchanged. Live
reticle output and full ARM32 shared linking remain unverified.
