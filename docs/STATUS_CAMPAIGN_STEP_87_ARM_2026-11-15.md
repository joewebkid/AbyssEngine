# Campaign transition, step 87 (Android ARM32)

Date: 2026-11-15

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb dispatch reaches step 87 at `0xaa97a`. At `0xaa97c` and
`0xaa982` it loads `104` and `10`, then calls
`Ship::removeCargo(int,int)` through the PLT at `0xaa992`. At
`0xaa996`–`0xaa9a0` it loads `Globals::status->systemVisibilities`
and stores the two bytes `0x01, 0x01` at indices 27 and 28. At
`0xaa9ac`–`0xaa9b2` it calls the three-argument `Mission` constructor
with `(4, 0, 109)`. The preceding source instead constructed
`Mission(11, 0, 10)` and omitted the cargo and visibility effects.

## Recovery and checks

The source now removes ten units of cargo item 104, marks both systems
visible, and creates the ordinary mission for station 109. The smoke
runner extracts the production implementation and checks this path at
`-O0` and `-O2`.

- Windows native `gof2`: pass.
- ARMv7 NDK r18b: 211/211 translation units compile.
- ARM64 static `gof2`: pass. Strict whole-archive ARM64 shared link:
  pass, producing an ELF64/AArch64 shared object.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.88%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped or wrong-type, and 151 extra symbols. Compared with the
  pre-change step-83 full report, both exact membership sets have
  **zero losses and zero gains**. The `Status` unit retains 103
  linked-exact and 66 byte-exact functions. Its unresolved-name set
  is unchanged.
- Focused `nextCampaignMission`: source-shape **34.5% -> 34.7%**,
  strict fuzzy **12.9% -> 11.5%**; generated instructions **889 ->
  895** against 2,884 original. This function remains non-exact.

Live campaign progression is unverified. A successful ARM64 link does
not establish a playable Android app. Step 88 is the next bounded
campaign transition to recover.
