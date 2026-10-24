# Level sentry-gun initialization (2026-10-24)

Primary source: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Level::createSentryGuns` at `0xbc1f4`, cross-checked against the IDA
dump at the same address. When equipment sort `0x27` is present, the
original creates nine objects of types `0x49c0..0x49c2`, adjusts their
radius, friendship and maximum health, then calls the virtual
`KIPlayer::setPosition(float,float,float)` with the same float on all
three axes. Literal word `0x47435000` at `0xbc2d4` is `50000.0f`.
It next calls `KIPlayer::setActive(false)` at `0xbc2a0` before appending
each object to `enemies`.

The previous C++ numerically converted unrelated integer
`0x9923e035` to a float coordinate and omitted the deactivation call.
The recovered source uses `50000.0f` and preserves the original order.
Two intermediary conversions through 32-bit `int` were also removed:
they truncate `Ship *` and created-object pointers on the x64 native
host, while direct typed pointers preserve the ARMv7 ABI.

Focused NDK r18b ARM `-Oz` verification improves from **96.8%
source-shape / 98.2% strict fuzzy** to **100% / 100%**. Both sides have
84 instructions and identical normalized instruction streams, but the
function is **not** linked-exact or byte-exact. Native `gof2` builds,
all 208 ARM translation units build, and 93 verifier tests pass (one
skip). The original call sequence is confirmed; no live sentry-gun
spawn or activation scene has been tested.

The full corpus compares 4,323/4,524 originals across 4,736 pairs,
with 201 missing and zero skipped. Mean source-shape/strict fuzzy is
unchanged at **94.14%/83.47%** to the displayed precision; the number
of 100%-fuzzy comparisons rises from 2,725 to 2,726. The **2,560
linked-exact** and **1,082 byte-exact** pair sets are unchanged, with
no losses. Full report SHA-256:
`b586cc932cebed5c8f6a50be72bbeea6db103e97d265ba9b34500ccf9ff0537a`.
ADB currently lists no device. The first-friend Pan ship-insertion gate
remains the next native runtime priority.
