# AbyssEngine shader constructor and ShaderInit ARM pass (2026-09-29)

The unweighted 1,009-comparison `_ZN11AbyssEngine*` source-shape mean rose
from **89.9388% to 93.2340%** (strict 76.1431% to 80.4917%). This does not
meet the requested 99.9% gate. Comparisons include multiple translation-unit
instances of some symbols; this is the verifier's existing denominator.

## Accepted source forms

- Android IDA constructors for 41 shader classes create a temporary
  `AbyssEngine::String` from a narrow class-name literal, assign it to
  `ShaderBaseStruct::name`, then destroy the temporary. The previous direct
  wide-literal assignments had a different call and lifetime shape. All 41
  constructors were checked with focused ARM reports; 40 now have 100%
  source-shape and `BlurShader` has 99.7%. Each constructor has C1/C2
  comparison aliases, so 82 comparison rows improved across the two batches.
- `DrawFBOShader` is the exception: Android `0x88490` sets the shader
  index but does not assign a name. Its former name assignment was removed.
  Both constructor aliases are now linked-exact.
- `VertexColorAlphaTextureShader` uses its full class name in the Android
  constructor at `0x91ae0`, not the local `VCATShader` abbreviation. Both
  constructor aliases are now linked-exact.
- `GenericShader` contained a write through a null static pointer that is
  absent from Android `0x8e750` and would corrupt runtime execution. It
  was removed; both constructor aliases are now linked-exact.
- `Engine::ShaderInit` follows the 39 concrete allocations, constructors,
  and registrations in Android `0x846ac` rather than cycling through five
  undefined `ShaderCtor_*` calls. The focused result is **7.8% -> 99.5%**
  source-shape, 322/322 ARM instructions. The remaining differences include
  `sizeof` values for recovered classes. Typed `new` preserves adequate
  allocation on the native host; forcing smaller Android byte counts before
  fixing class layouts would risk a heap overflow.

## Rejected and unresolved

The old direct wide-string assignment, `DrawFBOShader` name assignment,
null-pointer `GenericShader` write, and five-constructor `ShaderInit`
cycle are contradicted by Android code and ARM output. No synthetic stack
operations or dummy instructions were added for score. `BlurShader` differs
in its final paired float initialization; `ShaderInit` is not linked- or
byte-exact. Neither the available iOS dumps nor the installed PC binary
provides an attributed independent shape comparison for these functions.

## Gates and next work

Focused reports are `_work/abyss_ctor_*_20260929.json` and
`_work/abyss_engine_shader_init_trial_20260929.json`. Full corpus checkpoints:
`_work/full_arm_abyss_shader_ctors_20260929.json`,
`_work/full_arm_abyss_all_shader_ctors_20260929.json`, and
`_work/full_arm_abyss_shader_init_20260929.json`. The final full run has
4,630 comparisons, zero wrong-type misses, 2,160 linked-exact and 946
byte-exact symbols. All 204 ARM translation units compiled; UCRT64
`libgof2.a` linked after each accepted batch. No namespace source-shape
regressions appeared in the full-corpus comparisons.

The remaining deficit is 6,826.9 source-shape points over the 1,009 rows;
reaching 99.9% requires eliminating at least 6,726.0 of them. Largest unit
deficits: `PaintCanvas` 1,202.3, `ApplicationManager` 903.1,
`Engine` 804.1, `String` 783.4, and core `AbyssEngine` 686.2 points.
Priority bodies include `PaintCanvas::End3d` (0.3%),
`ApplicationManager::OnUpdate` (8.2%), `Engine::RenderMesh` (15.7%),
and `Transform::InsertKeyFrame_old` (41.7%). A green build and high
source-shape score do not establish runtime or byte equivalence.
