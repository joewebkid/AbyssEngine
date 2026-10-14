# StarSystem flare handoff (2026-10-14)

Android `StarSystem::render2D` at `0x1354e8` obtains the current camera
and its local matrix through `Globals::Canvas`. It adds each component
of `lightDirection * 65536.0f` to matrix entries 3, 7 and 11, calls
`LensFlare::update(0)`, and draws the flare only when
`PaintCanvas::GetScreenPosition` succeeds. The previous source instead
used `PaintCanvas::gCanvas` and ignored the projection result.

The production-body host smoke uses distinct global and static canvas
pointers. It covers the absent-flare path, failed projection (update
without draw), successful projection, camera matrix arithmetic, projected
coordinates and tint color. The native `gof2` target, 206/206 ARM
translation units and 37 verifier tests (one skipped) pass. Focused
source-shape rises **95.6% → 98.6%**; strict fuzzy falls **67.2% →
64.4%**. The function remains neither linked- nor byte-exact.

Full report `_work/starsystem_flare_full_20261014.json` compares
**4,323/4,524** original functions, with average source-shape
**93.29%**, strict fuzzy **82.21%**, **2,482 linked-exact** and
**1,065 byte-exact** pairs, **123 extras** and zero skipped units.
Against `_work/lens_flare_render_full_20261014.json`, the linked-exact,
byte-exact and extra-symbol sets are unchanged.

The host smoke does not exercise the actual `GetScreenPosition`
implementation, a live graphics context, assets or ARM32 runtime.
Next: validate a real StarSystem-to-LensFlare frame and investigate the
remaining compiler-shape gap without altering confirmed behavior.
