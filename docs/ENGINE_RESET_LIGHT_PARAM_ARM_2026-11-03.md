# Engine light defaults on Android ARM

Primary evidence is the original Android ARMv7 `libgof2hdaa.so`
(SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Engine::ResetLightParam` at `0x84348`. ARM stores at `0x84384–0x84412`
and the literal vectors at `0x844e0–0x8450f` establish the values and
32-bit offsets below; the GL calls at `0x8441a–0x8449a` follow those
stores. The existing source is in `src/engine/render/Engine.cpp`.

The reconstruction previously assigned integer `0x3f800000` to float
material channels. That C++ conversion produces about 1.065 billion,
where the original stores the **bit pattern** for `1.0f`. It also left
some material, light and shader-side fields untouched. The recovered
function now initializes:

- diffuse material RGBA to `(0.8, 0.8, 0.8, 1)`;
- ambient material RGBA to `(0.2, 0.2, 0.2, 1)`;
- specular material RGBA to `(0, 0, 0, 1)` and shininess to zero;
- primary diffuse/specular light RGBA to `(1, 1, 1, 1)`, primary ambient
  light and the intervening 16-byte RGBA slots to `(0, 0, 0, 1)`;
- scene ambient to `(0, 0, 0, 1)`, four shader-side vectors to zero,
  the vector at `0x2fc` to `(0.8, 0.8, 0.8)`, and the vectors at
  `0x308` and `0x314` to zero.

The original calls the seven fixed-function GL setters after those
stores when shaders are disabled, then resets two direction vectors
and their dirty flags. The recovered order now follows that sequence.
ARM32 compile-time checks cover key material and light offsets used by
these assignments. The meaning of the intervening RGBA slots and
unnamed vectors remains unconfirmed; their bytes and values are
directly observed, but no new semantic field names are claimed.

Focused ARM verification compiled **209/209** units, including the
offset checks. The before/after reports
(`_work/reset_light_param_before_20261103.json` and
`_work/reset_light_param_after_20261103.json`) show source-shape
**78.8% → 86.6%** and strict fuzzy **28.0% → 30.9%**. The function
remains nonexact. Windows native and ARM64 static builds pass with the
corrected source. Live lighting and visible rendering have not been
tested; Redmi testing remains stopped at the user's request.

The full ARM corpus (`_work/reset_light_param_full_20261103.json`)
compiled **209/209** units and compared **4,323/4,524** original
functions in **4,762** pairs, with **96.23%** mean source-shape,
**85.99%** strict fuzzy, **2,741 linked-exact**, **1,096 byte-exact**,
201 missing and zero skipped. Compared with
`_work/render_mesh_fixed_function_full_20261103.json`, the complete
comparison-pair set and both exact-match sets have zero gains and zero
losses. The overall averages are unchanged at two decimal places.

Next, inspect the shader-side consumers of these defaults and check
visible output on a host the user permits when one is available.
