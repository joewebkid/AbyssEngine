# Level::update flash-fade prefix (2026-10-24)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `Level::update(long long,bool)` `0xc5f58`, the flash-active prefix
`0xc5f78–0xc5fe4` subtracts the 32-bit delta from the signed remaining
duration. It clears the active flag when the result is negative and uses
`vcvt.f32.s32` for the subsequent fade ratio. Relocations at GOT entries
`0x210318`, `0x210310`, `0x210304`, `0x21036c` identify the four lower
color bounds as `Level::i_r`, `i_g`, `i_b`, `i_a` in RGBA order. The
original compares each bound with the scaled current channel and selects
the greater value (`vcgt.f32`/`vbsl`).

The previous C++ dereferenced four null placeholder pointers whenever a
flash was active and converted a negative remaining duration to an
unsigned float. The production prefix now reads the confirmed statics,
preserves 32-bit wrap before signed interpretation, and handles the four
fields directly. The direct form also avoids indexing from the
three-float `Vector` into the adjacent alpha field. The rest of
`Level::update` is unchanged.

`tools/run_level_flash_fade_smoke.py` extracts the production prefix and
runs it at host `-O0/-O2`. It checks an active quarter-step fade,
expiration with signed negative ratio and lower-bound clamping, and a
subsequent inactive update. With `--ndk-root` and `--adb`, the same
fixture compiled as an ARMv7 PIE using NDK r18b and passed on the
connected Android 16 device (arm64-v8a host, 32-bit app support). The
runner removed its temporary device executable. This is a CPU fixture
for the prefix, not a full Level update or visual Android test.

Focused `Level::update` source-shape/strict fuzzy improves **93.8%/65.5%
→ 97.2%/75.7%**; it remains nonexact. Native `gof2`, 208/208 ARM units,
and 93 verifier tests pass (one existing skip). The full corpus compares
**4,323/4,524** original functions across **4,736** pairs, with mean
source-shape/strict fuzzy **94.11%/83.43%** (unchanged when rounded to two
decimals). It retains **2,557 linked-exact** and **1,082 byte-exact**
pairs, with zero pair-identity losses, zero skipped and zero wrong-type
symbols. Only `Level::update` changed score. The full report is
`_work/flash_fade_full_20261024.json` (SHA-256
`c74dcef795e3dd1f36832078e555cc67968dded55314c5724f09174e59041aae`).

Unverified: a real Android frame with the production player and renderer,
and semantic correctness of unrelated branches in the large `update`
method. The original uses NEON vector operations; the restored C++ uses
four scalar expressions to preserve the same per-channel compare/select
semantics without relying on adjacent-object pointer arithmetic.
Next priority for the broader native path is the asset-backed Pan
first-friend `PlayerFighter` construction and array insertion; a real
visual flash can be checked when the production renderer is runnable.
