# Level::flashScreen color source recovery (2026-10-24)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
contains `Level::flashScreen(int)` at `0xc40e0`. Its type-2 path loads
`Level::i_r/i_g/i_b` and multiplies by 1.5. Its default path loads the
same three static floats, multiplies by 5 (or 8 for type 1), converts to
integer, and clamps each channel to 10–255. Type 3 clears RGB and calls
`PlayerEgo::hitCamera`; type 4 sets blue to 255. The method writes a
float 255.0 at offset `0x14c` after RGB.

The previous C++ dereferenced six null placeholder pointers for types
0, 1, 2 and other default-path values. The production source now uses
the confirmed `Level::i_r/i_g/i_b` statics and declares the fourth
flash channel as `float` at its original ABI offset. The color statics
are initialized from the current system's star color in `Level::createSpace`.

`tools/run_level_flash_screen_smoke.py` executes the exact extracted
`flashScreen` and clamp bodies at host `-O0/-O2`, covering types 0–5,
durations, colors, clamping, camera side effect and alpha. The harness
uses a recording `PlayerEgo`, not the production player. Native `gof2`
builds, 208/208 ARM translation units compile, and 93 verifier tests pass
with one existing skip.

Focused source-shape improves **61.5% → 99.7%** and strict fuzzy
**31.2% → 90.8%**; the function is still nonexact. The original has
114 instructions versus 115 in the rebuilt body. The full corpus
compares **4,323/4,524** original functions across **4,736** pairs,
with mean source-shape/strict fuzzy **94.11%/83.43%** (previously
**94.10%/83.42%**). It retains **2,557 linked-exact** and **1,082
byte-exact** pairs, with zero pair-identity losses, zero skipped and
zero wrong-type symbols. Only `flashScreen` changed score. The full
report is `_work/flash_screen_full_20261024.json` (SHA-256
`ef90c7d8d84e0bb0a3022eb4d203850e9aa4122ac98f5f990a68629a8c60266c`).

Unverified: a live Android visual flash, production `PlayerEgo` camera
response, and the subsequent `Level::update` flash-fade branch. That
branch still contains separate null clamp placeholders and must be
recovered before claiming the full flash lifecycle works.
