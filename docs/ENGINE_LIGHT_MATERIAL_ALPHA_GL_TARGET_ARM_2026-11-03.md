# Material-alpha GL call target on Android ARM

Primary reference: original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Engine::LightSetMaterialColorAlpha` at `0x856b4`. At
`0x856c0–0x856e2`, the function loads the resolved `glMaterialfv`
address and calls it; it calls the same target again at `0x856f8`
and tail-branches to it at `0x85714`.

The recovered function had the same instruction shape, but its local
function pointer `g_Engine_glMaterialfv` was never initialized. The
ARM object placed it in `.bss` and the function loaded that local
address. `SetColor` calls this function in the non-shader path when
lighting is enabled, so that path could call a null pointer. The
pointer is now initialized to the project's existing `glMaterialfv`
compatibility function, and its C++ type follows that declaration.
The rebuilt ARM object places the pointer in `.data` with an
`R_ARM_ABS32` relocation to `glMaterialfv`; the Windows object likewise
contains a data relocation to that function.

An exact instruction or linked match alone did not expose this data
initialization error. The original binary uses its own resolved GL
address, while this project binds an internal compatibility function;
the call's target behavior is restored without claiming identical
dynamic-linker storage.

Windows native and ARM64 static builds pass. Live execution of the
lit non-shader path has not been tested, and the user requested no
further Redmi testing.

Focused ARM before/after reports
(`_work/light_material_alpha_before_20261103.json` and
`_work/light_material_alpha_after_20261103.json`) remain
**100.0% source-shape / 100.0% strict fuzzy**, with this function still
linked-exact and not byte-exact.
The after-change ARM build compiled **209/209** units.
Those code metrics are unchanged because the fix is in data
initialization, not the function instructions.

The full ARM corpus (`_work/light_material_alpha_full_20261103.json`)
compiled **209/209** units and compared **4,323/4,524** originals in
**4,762** pairs at **96.23%** mean source-shape and **85.99%** strict
fuzzy, with **2,741 linked-exact**, **1,096 byte-exact**, 201 missing
and zero skipped. Against `_work/reset_light_param_full_20261103.json`,
the complete pair set and both exact-match sets have zero gains and
zero losses. This data repair does not change the reported code metrics.

Next, audit other call targets and data initialization in native
rendering, then verify the lit path on a permitted host when available.
