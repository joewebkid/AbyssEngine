# ModStation station-entry ticker construction (2026-10-11)

Android 2.0.16 `ModStation::OnInitialize` at `0xd5708`, `state 60` branch,
creates a `NewsTicker` at original dump lines 132494–132520. It passes the
station's `+0xa8` word as X, computes Y as screen height minus
`Layout+0x10` minus `PaintCanvas::GetTextHeight(Globals::font)`, sets width
to screen width minus X, and passes the current system race and campaign
mission. The constructor is stored at `ModStation+0x1c`. The ARM branch then
zeros `+0x30/+0x34`, `+0xb1/+0xb2`, `+0xd0/+0xd4/+0xd8`, creates a
`ChoiceWindow` at `+0x70`, and loads a faction image into `+0x4c` before
entering state 40. The ELF `.rodata` at `0x1fe0a8` gives resource IDs
`0x4a6, 0x4a3, 0x4a5, 0x4a4`, also present in the recovered `StarMap` and
HUD tables. These steps now replace the unresolved
`NewsTicker_build_oiImpl` call. ARM-only offset assertions cover the ticker
pointer, the station X boundary, and the image-output slot. The preceding
`state 80` branch writes `Layout+0xcc` to station `+0xa8` (original dump
line 132540), so the recovered field is now a signed coordinate, not a
geometry pointer. Touch handling uses the same boundary. The unrelated
`OnUpdate` geometry dereference is from `CutScene+0x20` in the original,
not from station `+0xa8`, and now uses `CutScene::rootGeom`.

Focused ARM verification builds **206/206** units. `OnInitialize` rises
from **57.9%** to **61.4%** source-shape; strict fuzzy is **16.9%** and
the function is not exact. Native `gof2` and 36 verifier tests pass (one
skipped). Full-corpus metrics and the retained JSON are recorded in
`PROJECT_STATUS.md`.

This is a branch restoration, not a complete station runtime test. Other
states of `OnInitialize` still use unresolved `*_oi` placeholders, and
`NewsTicker::replaceTokens` remains a stub. Next priority: validate station
entry/exit with controlled state, then recover the token substitutions.
