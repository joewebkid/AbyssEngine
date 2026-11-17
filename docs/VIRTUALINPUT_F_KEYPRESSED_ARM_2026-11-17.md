# `VirtualInput` `F` and `keyIsPressed` recovery (Android ARM32)

Date: 2026-11-17

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`F(unsigned int)` starts at `0x6b2d4`; `keyIsPressed()` starts at
`0x6a668`.

## Original-binary evidence

- **`F(unsigned int)` (`0x6b2d4`–`0x6b308`, 19 instructions):** loads
  the Blowfish S-box table `S` (`0x20fdc4`, `extern "C" unsigned int
  S[4][256]` defined in `BlowfishSubkeyFile.cpp`) and computes
  `(S[2][byte1] ^ (S[0][byte3] + S[1][byte2])) + S[3][byte0]`. The
  prior source dereferenced an uninitialized internal `g_hashTable`
  (`nullptr`), causing Clang `-Oz` to fold `F` to a single trap
  instruction (`0.0%` fuzzy, `21.1%` source-shape).
- **`keyIsPressed()` (`0x6a668`–`0x6a690`, 21 instructions):** loads
  `keys[0..3].pressed` (`+0x14`, `+0x6c`, `+0xc4`, `+0x11c`) and
  `g_mouseCaptured` (`0x217994`), ORs all five words (`orrs`), and
  returns `1` when non-zero. The prior source tested an unused internal
  `g_actionKeyState` variable and called `ArrowKeyPressed()`, folding to
  a single tail-branch (`0.0%` fuzzy, `21.0%` source-shape).

## Recovery and checks

`src/engine/input/VirtualInput.cpp` now references `extern "C" unsigned
int S[4][256]` in `F`, removes the uninitialized `g_hashTable` and
`g_actionKeyState` placeholders, and evaluates the bitwise OR of
`g_arrowKeys[0..3].pressed` and `g_mouseCaptured` in `keyIsPressed()`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,776**
  pairs, mean source-shape **96.96%** (from **96.92%**), strict fuzzy
  **87.06%** (from **87.02%**), **2,808 linked-exact** (from **2,806**,
  +2), **1,116 byte-exact**, 201 missing, zero skipped or wrong-type,
  and 151 extra symbols. Both exact membership sets have zero losses.
- Focused `F` and `_Z12keyIsPressedv`: both rise to **100.0%
  source-shape**, **100.0% strict fuzzy**, and **linked-exact**
  (`19/19` and `21/21` instructions).
