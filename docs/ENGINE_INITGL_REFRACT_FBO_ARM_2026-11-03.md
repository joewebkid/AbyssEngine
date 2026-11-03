# Refractive framebuffer creation in Engine::InitGL

Primary reference is original Android ARMv7 `libgof2hdaa.so`
(SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Engine::InitGL` starts at `0x6db20` (IDA also displays an unrelated
primary name for this address). At `0x6dc26–0x6dc34`, the original
checks `Engine::enableShader` and `Engine::EnablePostEffect` as bytes.
At `0x6dc3c–0x6dc6c`, it constructs `FBOContainer` with the name
`refractFBO` and calls `Create(width, height, true, false)`.

The one-byte `EnablePostEffect` symbol is at original data address
`0x2150d8` with initial value `0x01`. The reconstruction instead
initialized it to false, then gated this `InitGL` path on a separate
local `g_Engine_supportsFBO` integer that had no writer. It passed
`refract` as the name and reversed the two `Create` flags. The
recovered source now follows the original gate, data default, name
and call arguments. The unused local gate was removed.

The second `Create` flag controls texture filtering in the current
`FBOContainer` implementation, so this changes the refractive texture
from linear to nearest filtering. The first flag is currently unused
inside that method; its original call value is restored, but any
intended effect of that flag remains unverified.

Windows native and ARM64 static builds pass. The rebuilt native object
places `EnablePostEffect` in initialized data. Live FBO creation and
visible output have not been tested, and the user requested no further
Redmi testing.

Focused ARM verification compiled **209/209** units. The before/after
reports (`_work/initgl_refract_fbo_before_20261103.json` and
`_work/initgl_refract_fbo_after_20261103.json`) show `InitGL`
source-shape **76.4% → 98.1%** and strict fuzzy **56.9% → 76.0%**.
The generated function grew from 96 to 135 instructions against 136
in the original. It remains nonexact. The rebuilt ARM object also
contains a one-byte `EnablePostEffect` symbol in `.data` initialized
to `0x01`, matching the original symbol's size and initial byte.

The full ARM corpus (`_work/initgl_refract_fbo_full_20261103.json`)
compiled **209/209** units and compared **4,323/4,524** originals in
**4,762** pairs, with **96.24%** mean source-shape, **85.99%** strict
fuzzy, **2,741 linked-exact**, **1,096 byte-exact**, 201 missing and
zero skipped. Against `_work/light_material_alpha_full_20261103.json`,
the comparison-pair set and both exact-match sets have zero gains and
zero losses. The mean source-shape gain is 0.01 percentage point.

Next, inspect the original `FBOContainer::Create` flags and attachments
before claiming refractive capture equivalence, then check visible
output on a permitted host when available.
