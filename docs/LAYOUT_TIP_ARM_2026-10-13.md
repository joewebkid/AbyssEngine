# Layout tip — Android ARM audit (2026-10-13)

Scope: `Layout::drawTip()` in the Android 2.0.16 ELF at `0xd4a14`
(`_ZN6Layout7drawTipEv`). The primary evidence is the original Thumb
disassembly and the IDA export in the reference checkout. No iOS/PC
implementation was needed for this bounded change.

The local method dereferenced seven unassigned static holders for the
canvas, color, dimensions, layout metrics, font and globals service.
The original instead uses `Globals::Canvas`, `Globals::layout`,
`Globals::w`, `Globals::h`, `Globals::font` and `Globals::globals`.
It sets white, draws a centered box using layout field `0x78`, places
the tip image with alignment flags `0x11, 0x44`, then draws the lines
centered at the original line-height offset. When `tipLines` is null,
it does nothing. The original constructs a temporary empty `String`
directly for `drawBox`; introducing a named local caused an extra copy
and destructor in the ARM build.

Focused verification changed from 12.9% source-shape / 10.0% strict
fuzzy, non-exact, to 100.0% / 100.0%, linked-exact but not byte-exact.
The ARM build compiles all 206 translation units. Native `gof2`, the
existing TouchButton/Layout host smoke and 37 verifier tests pass
(one skipped). Linked-exact is a machine-code comparison, not a claim
that visual output has been observed on a device.

Full ARM corpus: 4,719 comparisons, 4,323/4,524 original functions
covered, 201 missing, 92.88% average source-shape and 81.25% strict
fuzzy, 2,398 linked-exact and 1,008 byte-exact. Against the preceding
corpus report, complete `(unit,symbol)` comparisons show one new
linked-exact symbol (`drawTip`), no linked- or byte-exact losses, and
the same 123 extra symbols. The reports are
`A:/Galaxy on Fire new version/_work/layout_tip_before_20261013.json`,
`layout_tip_after_20261013.json` and `layout_tip_full_20261013.json`.
Android ARM32 runtime, real texture assets and visual placement remain
untested. The next priority was `initTip`, subsequently covered in
[the initialization audit](LAYOUT_INIT_TIP_ARM_2026-10-13.md).
