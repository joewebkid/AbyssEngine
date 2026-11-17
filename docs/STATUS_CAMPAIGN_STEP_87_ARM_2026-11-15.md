# Campaign transition, step 87 (Android ARM32)

Date: 2026-11-15 (corrected 2026-11-16)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb-2 branch table (`0xa9ac6: tbh [pc, r1, lsl #1]`, base
`0xa9aca`) dispatches step 87 (`0x57`) out-of-line to `0xaa240`. That
block allocates 120 bytes (`0xaa24a`), calls `Mission(11, 0, 10)`
through the PLT at `0xaa256`, and branches to `0xa9c30` to append an
ordinary mission with no cargo or visibility side effects.

An earlier linear reading after step 86 (`0xaa95c`) mistakenly
attributed `0xaa97a` (`removeCargo(104, 10)`, `systemVisibilities[27..28]`,
`Mission(4, 0, 109)`) to step 87; `tbh[88]` actually dispatches `0xaa97a`
to step 88.

## Recovery and checks

On 2026-11-16, `kSteps[0x57]` was restored to `{0xb, 0, 10, 0}` and the
`0xaa97a` side effects were moved to step 88. The smoke runner extracts
the production implementation and verifies at `-O0` and `-O2` that step
87 creates ordinary `Mission(11, 0, 10)` without removing cargo or
marking systems `27`/`28` visible.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Both exact membership
  sets have **zero losses and zero gains** relative to the prior run.
- Focused `nextCampaignMission` (after correcting steps 87–89 together):
  source-shape **35.6% -> 35.2%**, strict fuzzy **13.4% -> 11.3%**;
  generated instructions **928 -> 915** against 2,884 original. This
  function remains non-exact.

Live campaign progression is unverified. A successful ARM64 link does
not establish a playable Android app. See
[step 88/89 evidence](STATUS_CAMPAIGN_STEP_88_ARM_2026-11-15.md) for the
corrected `0xaa97a` (`step == 88`) and `0xaa9ba` (`step == 89`) blocks.
