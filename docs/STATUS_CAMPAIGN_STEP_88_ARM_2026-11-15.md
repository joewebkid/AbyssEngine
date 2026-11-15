# Campaign transition, step 88 (Android ARM32)

Date: 2026-11-15

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb dispatch reaches step 88 at `0xaa9ba`. The code at
`0xaa9ce`–`0xaa9d6` decrements `Globals::status->stationsVisited`
at offset `0x1d0`. It calls `Galaxy::getVisited()` at `0xaa9dc`
and clears byte 109 of the returned array at `0xaa9e0`–`0xaa9e2`.
The PLT call at `0xaa9f4` constructs `Mission(11, 0, 10)`. The prior
source instead constructed `Mission(4, 0, 109)` and omitted the
counter and visited-array changes.

## Recovery and checks

The source now decrements the counter, clears station 109's visited
byte, and creates the ordinary mission at station 10. The smoke
runner extracts the production implementation and checks this path
at `-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Compared with the
  preceding step-87 report, both exact membership sets have zero
  losses and zero gains.
- Focused `nextCampaignMission`: source-shape **34.7% -> 35.6%**,
  strict fuzzy **11.5% -> 13.4%**; generated instructions **895 ->
  928** against 2,884 original. This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
