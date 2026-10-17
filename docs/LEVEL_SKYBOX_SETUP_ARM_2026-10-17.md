# Level skybox setup (2026-10-17)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`Level::createSpace` at `0xae5b8`, especially `0xae5dc`–`0xaea00`.

The previous source created only the background mesh in a normal orbit.
It stored all three skybox texture results in a scratch word, left the main
skybox mesh at `-1`, wrote zero to the post-effect field after looking up that
invalid handle, and forced all rotations to zero. The ARM body instead stores
the background and main texture handles at `Level+0x19c/+0x198`, creates the
main mesh by system texture index, and only zeros rotations in a fog skybox.
All canvas calls use `Globals::Canvas`, as in the ARM body; the host smoke
deliberately points `PaintCanvas::gCanvas` at a different test canvas.

The restored branch covers the intro resource pair (`0x458b/0x2754`),
ordinary system-index and texture-index variants, alien resources
(`0x45bc/0x2768`, `0x4592/0x275b`), supernova mission 89 and later
resource paths, optional animated supernova layers, system 24/25/26 flare,
planet-ring and storm transforms, the post-effect word `1046562734`, and
station-seeded skybox rotations followed by RNG reset. Each resource call
writes to the corresponding Level field, matching the ARM output address and
the fields read by `renderBG`. No speculative resource IDs were introduced.

The production prelude passes source-extracted host tests at `-O0` and `-O2`
for ordinary, alien, intro, supernova-89, supernova-106, supernova-158,
ring/storm/flare, post-effect, fog/RNG (with and without a station), and
already-created branches.
Prior station, jumpgate, wormhole-tail, StarSystem-tail, entry-position and
constructor smokes also pass. Native `gof2`, 208/208 ARM TUs and 40 verifier
tests pass (one existing skip). Focused `createSpace` source-shape rises
71.5% → 86.1%; strict fuzzy rises 39.0% → 46.2%, still nonexact.
Focused reports:
`A:/Galaxy on Fire new version/_work/level_skybox_before_20261017.json`
and `level_skybox_after_20261017.json` in the same directory.

The full ARM report
`A:/Galaxy on Fire new version/_work/level_skybox_full_20261017.json`
compares 4,323/4,524 original functions in 4,730 rows, with 201 missing,
zero skipped and zero wrong-type. Mean source-shape/strict fuzzy remains
93.86%/83.04% after rounding. Linked-exact/byte-exact remains
2,528/1,076, with zero exact-pair losses against
`level_entry_starsystem_final_full_20261017.json`.

The host smoke uses canvas, status and RNG test doubles. It does not load the
original game resources or prove a full `Level::init` → `createSpace` →
`renderBG` run. That asset-backed gate is the next priority. Exact matching
is tracked separately from behavioral readiness. The constructor's initial
state for `supernovaFlareActive` also needs an independent binary/runtime
check; this package did not change it.
