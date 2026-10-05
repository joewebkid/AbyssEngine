# AbyssEngine Transform source-shape pass (2026-09-29)

The same 1,009 `_ZN11AbyssEngine*` ARM symbols now average **89.9388%**
source-shape, up from **89.7886%**. This is below the requested 99.9% gate.
The baseline is `_work/full_arm_abyssengine_20260929_final.json`; the accepted
build is `_work/full_arm_abyss_transform_20260929.json`. Neither report is
a claim of runtime or byte equivalence for the namespace.

## Accepted forms

| Function | Source before -> after | Strict before -> after | Evidence |
| --- | ---: | ---: | --- |
| `Transform::SetAnimationState` | 35.8 -> 100.0% | 15.4 -> 100.0% | Android IDA `0x74c3c` and Ghidra `0x84c3c`: mode 0 clears `animating`, mode 3 sets it and copies `rangeStart` to `currentTime`; other modes store at `+0x58`. |
| `Transform::Update` | 5.9 -> 93.2% | 1.2 -> 37.7% | Android IDA `0x74cc8` and Ghidra `0x84cc8`: advance animation time, clamp or wrap it, call `InternUpdate`, then rebuild the bounds from mesh and child spheres when requested. |

For looping mode 2, the native `sub_19ae54` call is a signed 64-bit
division/remainder helper. The original passes `currentTime` and `rangeEnd`,
then adds the remainder to `rangeStart`. A zero `rangeEnd` skips division.
For animated meshes, native ARM updates the attached `Transform` before
copying the mesh bounds and rotating the center by its `rotationMatrix`.
Child transforms follow the same update/rotate/merge sequence. The local
implementation skips a null mesh, whereas the original's null branch appears
to dereference it; this remains a deliberate runtime guard, not byte-match.

## Rejected and remaining forms

The first `Update` form populated the reusable sphere before checking the
mesh animation. It reached **88.5% source-shape / 54.4% strict** with 145
local versus 160 target instructions, but created an extra sphere assignment
and disagreed with the native call order. The accepted form stages the sphere
after animation update: **93.2% source-shape / 37.7% strict**, 148 local
instructions. Strict fell because of different register/temporary scheduling;
this is recorded rather than hidden. Focused reports:
`_work/abyss_transform_update_trial_20260929.json` and
`_work/abyss_transform_bounds_order_trial_20260929.json`.
The former empty `SetAnimationState` body was rejected as contradicted by
both decompilers and the original ARM.

`Transform::Update` is not linked- or byte-exact. The remaining diff includes
stack placement, mesh null handling, and bounds temporary lifetime. The
available iOS dumps have no independently attributed matching bodies here;
the installed PC binary lacks a matching symbol/body map. Android IDA,
Ghidra, and the original ARM are the direct evidence, not three independent
platform confirmations.

## Validation

- Focused `Transform` verification: 28 comparisons; only the two functions
  above changed source-shape. `SetAnimationState` is now byte-exact.
- All 204 ARM translation units compiled.
- Full `--fail-on-wrong-type` corpus: 4,630 comparisons, zero wrong-type
  misses, 2,082 linked-exact and 946 byte-exact functions. No source-shape
  regression among the 1,009 AbyssEngine symbols.
- UCRT64 `gof2` linked `libgof2.a`.

The next substantial deficits remain `PaintCanvas::End3d`,
`ApplicationManager::OnUpdate`, `Engine::ShaderInit` and other large bodies.
Their recovery requires separate call/ownership and platform-contract audits;
this pass does not extrapolate the two Transform results to them.
