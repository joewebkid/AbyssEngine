# Layout fade and full-screen fill — Android ARM audit (2026-10-12)

## Evidence and correction

Android 2.0.16 `Layout::drawFade` at ELF `0xd5038` reads
`Globals::Canvas`, saves its color, fills `(0, 0, Globals::w,
Globals::h)`, then restores that color. Both the active fade and
`fillScreen` branches use the same canvas and dimensions. The local
source previously dereferenced four never-set null aliases and
passed zero height to both fills.

The ARM branch at `Layout + 0x401` inverts the progress fraction when
the stored flag is **false**. The local `fadeOut` condition did the
opposite. ARM also initializes the capped fraction to `1.0` and
replaces it only when the computed fraction is less than `1.0`;
this matters for `0/0` (a zero-duration fade at zero progress), where
the original retains full alpha. The corrected source follows both
branches. `Layout::startFade` at ELF `0xd5014` stores its boolean
argument at `+0x401`, confirming the flag mapping.

The host smoke extracts the unchanged production `drawFade` body and
checks increasing/decreasing alpha, both fill branches, screen
dimensions, color restoration, the return flag, and the zero-duration
edge case with fixture canvas services. It does not use real game
rendering or original ARM execution.

## Verification and limits

Native `gof2` and 206/206 ARM translation units build. The focused
comparison rises from 85.3% to 94.5% source-shape and from 57.8% to
68.0% strict fuzzy; it remains non-exact. Focused reports:
`A:/Galaxy on Fire new version/_work/layout_draw_fade_before_20261012.json`
and
`A:/Galaxy on Fire new version/_work/layout_draw_fade_final_focused_20261012.json`.
The complete host UI smoke passes: `TouchButton/Layout host smoke: OK`.
The verifier suite runs 37 tests: 36 pass and one is skipped.

The full ARM corpus reports 4,714 comparisons and 4,323/4,524
original functions covered, with 201 missing and zero skipped or
wrong-type symbols. Average source-shape and strict fuzzy retain
their rounded 92.42% / 80.68%; linked-exact remains 2,373 and
byte-exact remains 1,008. Comparing the exact symbol sets with the
preceding full report found no gains or losses. Final report:
`A:/Galaxy on Fire new version/_work/layout_draw_fade_final_full_20261012.json`.

The full game fade sequence, actual display output, platform service
initialization, and frame timing remain untested.
