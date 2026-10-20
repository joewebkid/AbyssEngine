# Level gas-cloud recovery (2026-10-20)

## Primary evidence

The locally held Android 2.0.16 `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA analysis of `_ZN5Level15createGasCloudsEv` at `0xafd2c`
(evidence `ev_e001ec5ced93c3ead6faec372dc23b030d1c801c9068ba93c4518003b071b6c6`)
is decompiler output, not original C++ source. Relevant ARM instructions
independently anchor the following recovered behavior:

- The function reads `Globals::galaxy` and `Globals::status`, obtains plasma
  probabilities before checking for ship equipment sort `0x21`, and returns
  early without that equipment or in alien orbit.
- With equipment it accepts the non-`0xcc`/routed-system cases, allocates an
  `Array<KIPlayer *>`, then reads `Globals::rnd` and calls `nextInt(4)`. The
  count is the truncated nonnegative value of `boss ? 3 : 0` plus
  `prob[1] / 100.0f * (roll + 4)`.
- Each candidate position has three `nextInt(160000)-80000` components. It
  must be farther than 25,000 from the origin and 30,000 from the asteroid
  center. The first boss cloud is then placed at `(92000, 0, 36000)`.
  Direct assembly recheck: `ev_e160b12e9d386bfe7f5eb585d6fc4ce7f9137c38fd17d986840ce00a01b1a1f5`.
- ARM allocation sizes are `0xc0` for `AEGeometry` and `0x16c` for
  `PlayerGasCloud`. The constructor at `0x1763cc` writes `center` at `+0x128`,
  `modelGeometry` at `+0x134`, `itemId` and the full-width spark-mesh ID at
  `+0x160/+0x164`, and the cloud-mesh ID at `+0x168` (REA/IDA assembly
  `ev_ed76bb67bc547cfb3092a1e09191892e9527d54d2245cfb9fcf6397dfd8e0e86`).
  The reconstructed base ends at `+0x124`. An opaque four-byte slot now
  preserves the observed derived layout; its meaning and initialization are
  not established. The spark-mesh field is now `int`, matching the original
  32-bit store; its 16-bit use by `AEGeometry` is an implicit conversion.

This replaces the prior implementation's truncated host pointer, unbounded
`nextInt()`, `/1.0f` probability divisor and player-relative placement helper.
The original routine does not show probability-buffer cleanup; no new cleanup
was inferred for this recovery.

## Executable boundary

`tools/run_level_stage1_gas_clouds_pan_gate.py` continues the original-asset
Pan `Level::init` path through the entire asteroid field and production
`createGasClouds`, stopping before mission dispatch. It inserts a synthetic
sort-`0x21` item into the existing empty equipment slot and supplies a bounded
`0xc9, 100%` plasma-probability buffer. This deliberately tests one equipped
branch, not the real Galaxy probability calculation. It checks 4–7 clouds,
their level/player/geometry links, mesh IDs, health and both position bounds.
Afterward it removes the item and directly calls the same production function
to verify the no-equipment return leaves the existing cloud array unchanged.
It also checks that a distinct alien-orbit station leaves that array unchanged,
then selects campaign mission `0x8e` and station `0x4f` to check the boss
count (7–10 with this fixture) and the first cloud's fixed position. The
synthetic alien station is retained for the test process lifetime because the
earlier station-catalog fixture instruments its five destructors.
Only material IDs `20111` and `29045`, observed from the registered original
assets for these clouds, were added to the fail-closed host material allowlist.

Windows SwiftShader and physical Android ARMv7 passed at `-O0` and `-O2`
with the final layout and full-width mesh ID. The native `gof2` build and
all 208 ARM translation units pass with canonical NDK r18b; the header asserts
the original offsets and `0x16c` size. The previous corpus report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/explosion-globals-full.json`
(SHA-256 `4e60e6fae9369f893d984612bd969311fd99a196dfbf250c6a830035963b7deb`):
4,323/4,524 originals, 4,735 pairs, 93.96% mean source-shape, 83.24% strict
fuzzy, 2,551 linked-exact and 1,079 byte-exact, zero skipped. The current
full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/gas-r18b-full.json`
(SHA-256 `4f5108c02981aed8e8d0181737d43dcd112a2387cf7cd00461aea8ddebe85a5d`):
4,323/4,524 originals, 4,735 pairs, 93.97% mean source-shape, 83.29%
strict fuzzy, 2,554 linked-exact and 1,082 byte-exact; zero skipped and zero
wrong-type symbols. Comparing exact-pair identities with the previous report
shows no losses and three gains in both sets:
`PlayerGasCloud::hasExploded`, `setSparkInSight` and `getSparks`.
Focused `createGasClouds` improves from 88.3%/41.5% to 93.9%/55.2%
source-shape/strict fuzzy, but remains nonexact. The 15-function
`PlayerGasCloud` focused family improves from 95.42%/76.03% to
97.65%/89.21%, with linked-exact 4 to 7 and byte-exact 0 to 3. The
alien/boss gate extension changes only a test runner and documentation.
The 93-test verifier unit suite passes with one existing skip when temporary
files use D:. Its earlier two linker failures with C: full were environmental.

## Open work

Identify the semantics of the opaque `PlayerGasCloud+0x124` slot; exercise real plasma distributions and
additional alien/boss conditions, then continue Pan stage one through mission
dispatch and return. Cloud update, collision, rendering and a playable frame
are still outside this gate.
