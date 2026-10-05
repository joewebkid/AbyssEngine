# AbyssEngine ARM source-shape: canvas/performance package (2026-09-29)

The unweighted `_ZN11AbyssEngine*` source-shape mean across the same 1,009
functions rose from **89.306% to 89.789%** (strict 75.608% to 76.023%).
This is a measured gain, not completion of the requested 99.9% target.
There were no source-shape regressions among those 1,009 functions.

## Accepted Android-backed forms

| Function | Source before -> after | Strict after | Evidence and behavior |
| --- | ---: | ---: | --- |
| `PaintCanvas::CheckNUseRefractFBO` | 0.9 -> 100.0% | 100.0% | Android `0x7d504`: gate on `Engine::enableShader` and `Engine::EnableRefract`, then test `IsPostEffectActivated` and activate the refraction FBO. |
| `PaintCanvas::ChangeCubeTexture` | 0.5 -> 93.8% | 48.6% | Android `0x7c288`: shader gate, cube-texture bounds/restore flag, `CubeMapSetIndex`, GL texture unit 7 bind and reset to unit 0. The local and original differ in resource-pointer reload scheduling. |
| `PaintCanvas::RemoveAllMatsForGlow` | 15.6 -> 100.0% | 100.0% | Android `0x7d484`: five typed `ArrayRemoveAll` calls, including three `Matrix` arrays. Clearing only the counts was not equivalent. |
| `PaintCanvas::TransformAddMesh` | 6.0 -> 88.9% | 53.2% | Android `0x7b0b0`: create mesh, append to transform, merge bounds, propagate animation length/state, collect animation data. The ARM call at state change passes mode 2 and null context. |
| `PaintCanvas::SetMatForGlow` | 39.0 -> 61.5% | 20.0% | Android `0x7d24c`: append mesh, three `Matrix` values and an unsigned value to the five glow arrays. Behavior is source-backed, but the 148-instruction local loop is not the original 132-instruction call shape. |
| `ApplicationManager::EnablePerformanceTest` | 20.5 -> 100.0% | 67.4% | Android `0x82334`: ARM uses `r0` as the signed count. The method is static despite its mangled member name. Reset four 64-bit global counters and set the actual performance flags. |
| `ApplicationManager::SetCurrentApplicationModule` | 69.7 -> 94.9% | 50.9% | Android `0x823b0`: update the same global module-switch count and end the test at its configured limit. |

The previous local zero-initialized shader/refract flags made two rendering
bodies compile to a single `bx lr`. The previous local performance counters
were disconnected from the engine globals and made the test initializer
compile to `bx lr`. Those forms and the untyped glow/mesh shims were rejected.
No dummy instructions, volatile scratch state, or synthetic branches were
introduced to inflate the metric.

## Validation

- Focused reports: `_work/abyss_paintcanvas_cube_refract_trial_20260929.json`,
  `_work/abyss_paintcanvas_glow_trial_20260929.json`,
  `_work/abyss_paintcanvas_mesh_trial_20260929.json`,
  `_work/abyss_paintcanvas_glow_populate_trial_20260929.json`, and
  `_work/abyss_appmanager_perf_trial_20260929.json`.
- Final full corpus: `_work/full_arm_abyssengine_20260929_final.json`:
  4,630 comparisons, 89.82% whole-corpus source-shape, 2,081 linked-exact,
  945 byte-exact, zero `missing_wrong_type`.
- All 204 ARM translation units compiled. UCRT64 `gof2` linked `libgof2.a`.
  The native build still emits pre-existing unrelated pointer-cast warnings.

## Still open

`PaintCanvas::End3d` remains at 0.3% (1 local versus 1,182 original ARM
instructions). `Transform::SetAnimationState` still has an empty body, so
the new call in `TransformAddMesh` does not yet restore that behavior.
`ApplicationManager::OnUpdate` is 8.2% (119 versus 1,682 instructions).
The largest remaining namespace deficits are PaintCanvas (1,202.3 aggregate
source-shape points), ApplicationManager (903.1), Engine (895.8), String
(783.4), core AbyssEngine (686.2), and Transform (469.7). This package does
not establish ABI/runtime safety for those bodies.

The Android IDA body and original ARM instructions are the direct evidence
for this package. The available iOS dumps do not expose matching attributed
bodies, and the installed PC build does not provide a matching symbol/body
map. They are not counted as independent confirmation of ARM instruction
shape. The next packages should recover the large bodies and resolve their
platform/render contracts before attempting a 99.9% namespace gate.
