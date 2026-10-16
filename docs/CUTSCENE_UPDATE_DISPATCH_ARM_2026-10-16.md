# CutScene update dispatch (2026-10-16)

In Android ARM, both `CutScene::update(int)` at `0x98e86` and
`CutScene::update()` at `0x99664` are one-instruction tail branches
through the same veneer to `CutScene::process(int)@plt` (`0x6219c`).
The original `process` entry does not preserve the incoming `r1`
argument and obtains frame time through
`ApplicationManager::GetCurrentTimeMillis()`. The recovered `process`
body likewise does not read its `delta` parameter.

Previously the no-argument `update` called `Level::update` directly
and never ran the cut-scene state machine; the integer overload called
that incorrect wrapper. Both now dispatch to `process`. The
no-argument overload passes zero because C++ requires an argument;
the original wrapper leaves `r1` untouched, but `process` ignores it.
The direct `Level::update` path in `render3D` is unchanged.

Focused ARM verification raises `update()` from **1.7% source-shape /
0.0% strict fuzzy** (`12/1` rebuilt/original instructions) to
**100% / 100%** (`1/1`). `update(int)` stays **100% / 100%** (`1/1`).
Neither function is linked- or byte-exact. The native `gof2` build,
**208/208** ARM translation units and 40 verifier tests (one existing
skip) pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_update_full_20261016.json`
compares **4,323/4,524** original functions, with **201 missing**,
**123 extras**, **93.72% / 82.82%** average source-shape/strict
fuzzy, **2,515 linked-exact** and **1,075 byte-exact**. Both exact
`(unit,symbol)` sets have zero gains and zero losses against the prior
`player_ego_setposition_full_20261016.json` report.

This checks the native dispatch but not live cut-scene timing on
ARM32. `CutScene::process(int)` remains nonexact (86.3% source-shape
in the preceding full report); its update and render interactions
need a separate evidence-backed audit. Next priority is the bounded
`CutScene::render3D()` path before changing the large `process` body.
