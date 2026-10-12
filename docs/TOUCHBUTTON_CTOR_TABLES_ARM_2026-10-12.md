# TouchButton constructor contract and resource tables — Android ARM audit (2026-10-12)

## Evidence and correction

The Android 2.0.16 `TouchButton` constructors and `init` at ELF addresses
`0x1684f0` through `0x1690e4` identify the `init` argument order as
`subId, kind, medalId, medalStage, x, y, requestedWidth, flags0, flags1`.
Six of seven local constructors had shifted arguments; one already used the
right order. The constructors now forward those roles and use the live
`Globals::font` and `Globals::Canvas`, while `init` reads the confirmed
achievements, layout, and device globals instead of null placeholders.

The icon texture/image IDs, small medal IDs, and two device frame-ID tables
were recovered from Android `.rodata` at `0x1fb888..0x1fb8c8`, `0x2060bc`,
`0x206170`, and `0x206464`. A direct readback of the original ELF found no
differences in the 8 icon texture IDs, 8 icon image IDs, 45 medal IDs, or
216 explicit frame entries. The remaining frame rows are zero in the
original and are zero-initialized here. These are IDs, not copied game
assets. The former unresolved `TB_*` helper references are now internal
lookups backed by these tables.

## Verification and limits

- Native `gof2` static-library build: pass. ARM build: 206/206 translation
  units. Verifier suite: 37 run, 36 passed, one skipped.
- Focused `TouchButton` comparison: 44 symbols; 29 linked-exact and 14
  byte-exact. Five constructor C1/C2 pairs newly become linked-exact.
  `TouchButton::init` rises from 76.1% to 79.8% source-shape and from
  15.8% to 28.2% strict fuzzy; it is not exact.
- Full corpus: 4,714 comparisons, 4,323/4,524 original functions covered,
  201 missing, zero skipped or wrong-type. Average source-shape rises
  92.39% to 92.41%; strict fuzzy rises 80.58% to 80.67%. Linked-exact
  rises 2,360 to 2,370; byte-exact stays 1,008. Per-symbol comparison
  found no exact-pair losses. Reports:
  `A:/Galaxy on Fire new version/_work/touchbutton_ctor_tables_focused_20261012.json`
  and `A:/Galaxy on Fire new version/_work/touchbutton_ctor_tables_full_20261012.json`.

No live UI run was performed. `init` still contains unverified image
measurement arguments (several calls currently pass handle `0`), and
other drawing paths remain incomplete. The exact constructor matches do
not establish that a full button can be created or drawn correctly at
runtime. Next inspect `init`'s per-kind image handles and `setText` against
the ARM original, then run a focused UI smoke where linkage permits.
