# ImageFactory portraits and ImagePart — Android ARM (2026-10-13)

`ImageFactory::loadImage` previously dereferenced a translation-unit-local
null ID-table pointer. The ARM optimizer reduced its entire body to one
instruction, although the source looked substantially reconstructed. The
same family still had null canvas, RNG, screen-mode, and coordinate-table
aliases. `ImagePart` constructor/draw also used null canvas aliases.

Android `libgof2hdaa.so` 2.0.16 provides the 13×4 random-choice counts at
`0x201c24`, resource IDs at `0x201cf4`, and 13×4×2 position pairs in
`IMAGE_OFFSETS` (`0x215ba8`), `IMAGE_OFFSETS_IPAD` (`0x215a08`),
`IMAGE_OFFSETS_IPAD_HD` (`0x2156c8`), and `IMAGE_OFFSETS_IPAD_LARGE`
(`0x215868`). All 520 integers in the new source tables were checked
against the ELF. The retina/n9 source at `0x201a84` is byte-identical to
`IMAGE_OFFSETS_IPAD`; the constructor copies it into the mutable base
table as Android `ImageFactory::ImageFactory` at `0x11c54c` does.

The recovered `loadImage` at `0x11c834` uses `Globals::Canvas`, chooses
HD, large, iPad, then base offsets in Android priority order, and creates
`ImagePart` only for nonnegative IDs. Both `createChar` overloads use the
shared `Globals::rnd` and the original per-part counts; the one-argument
overload now returns the descriptor pointer actually returned by ARM at
`0x11c6cc`. Drawing methods use the shared canvas. `ImagePart` construction
and drawing use `Globals::Canvas`, matching ARM at `0xa7a74` and `0xa7aac`.

An extracted-body host smoke exercises descriptor generation, `loadChar`
part swapping, absent parts, all four screen-mode coordinate paths,
retina-table replacement, `ImagePart` dimensions/flags, character drawing,
and ship/item draw routing. The fixture substitutes canvas, random-number,
sprite, and `reload` services; it does not link the whole engine, load actual
game images, execute OpenGL, or run on ARM32. The production `reload` and
`Sprite` implementations still need integrated asset-backed validation.

The 14-symbol `ImageFactory` family rises from **76.31%** to **97.87%**
average source-shape and from **55.33%** to **78.31%** strict fuzzy.
`loadImage` alone rises **0.2% → 93.5%** source-shape; both factory
constructor aliases rise **38.3% → 98.5%**. The two `createChar` functions
become linked-exact. All three changed `ImagePart` constructor/draw
comparisons become linked-exact at **100%** source-shape and strict fuzzy.

Native `gof2` builds, all **206/206** ARM translation units compile, the
portrait smoke passes, and 37 verifier tests pass (one skipped). Full corpus:
**4,720** comparisons, **4,323/4,524** original functions covered,
**201** missing, **92.67655%** average source-shape (was 92.60138%),
**81.04394%** strict fuzzy (was 80.93697%), **2,388** linked-exact
(was 2,383), **1,008** byte-exact (unchanged), zero skipped and
wrong-type symbols. No previously linked- or byte-exact comparison row
was lost; the five new linked rows are the two `createChar` functions and
three `ImagePart` constructor/draw entries.

Evidence: IDA output in
`A:/Galaxy on Fire new version/analysis/gof2_libgof2hdaa_full_ida.c`,
the local Android ELF, baseline
`A:/Galaxy on Fire new version/_work/transform_clone_arrayset_full_20261013.json`,
focused reports `image_factory_portrait_baseline_20261013.json`,
`image_part_portrait_baseline_20261013.json`,
`image_factory_family_focus_final_20261013.json`, and
`image_part_portrait_focus_20261013.json`, plus full final
`A:/Galaxy on Fire new version/_work/image_factory_family_full_final_20261013.json`.
Next audit real `ImageFactory::reload`/`Sprite` image ownership and rendering
with assets when available; a green static-library build or host fixture
alone cannot establish playable portrait behavior.
