# Campaign transitions, steps 88–89 (Android ARM32)

Date: 2026-11-15 (corrected 2026-11-16)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`) dispatches steps 88 and 89 as follows (correcting an earlier
off-by-one reading that assigned these blocks to steps 87 and 88):

- **Step 88 (`0x58`) -> `0xaa97a`:** loads `104` and `10` (`0xaa97c`,
  `0xaa982`), calls `Ship::removeCargo(int, int)` through the PLT at
  `0xaa992`, writes `0x01, 0x01` to `Globals::status->systemVisibilities`
  at indices `27` and `28` (`0xaa996`–`0xaa9a0`), constructs ordinary
  `Mission(4, 0, 109)` at `0xaa9b2`, and branches to `0xa9c30`.
- **Step 89 (`0x59`) -> `0xaa9ba`:** decrements
  `Globals::status->stationsVisited` at offset `0x1d0`
  (`0xaa9ce`–`0xaa9d6`), calls `Galaxy::getVisited()` at `0xaa9dc`,
  clears byte `109` (`0x6d`) of the returned array at `0xaa9e0`–`0xaa9e2`,
  constructs ordinary `Mission(11, 0, 10)` at `0xaa9f4`, and branches to
  `0xa9c30`.

## Recovery and checks

On 2026-11-16, `kSteps[0x58]` was updated to `{0x4, 0, 0x6d, 0}`,
`kSteps[0x59]` was added as `{0xb, 0, 10, 0}`, and the two side-effect
branches were shifted to `step == 88` and `step == 89`. The smoke runner
extracts the production implementation and verifies steps 87, 88, and 89
at `-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have zero losses and zero gains relative to the prior run.
- Focused `nextCampaignMission`: source-shape **35.6% -> 35.2%**,
  strict fuzzy **13.4% -> 11.3%**; generated instructions **928 ->
  915** against 2,884 original. This function remains non-exact.

Live campaign progression and full ARM32 shared linking remain
unverified. An ARM64 shared link does not establish a playable APK.
Next campaign transition to inspect: step 90 (`0x5a` -> `0xaaa38`).
