# Engine fixed-function mesh rendering on Android ARM

The primary reference is the original Android ARMv7 `libgof2hdaa.so`
(SHA-256 `6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Engine::RenderMesh` at `0x85f24`. The recovered code is in
`src/engine/render/Engine.cpp`.

In the original fixed-function branch, vertex-format bit `2` controls
the texture-coordinate array. If the material has a second texture,
the function skips both `glTexCoordPointer` and the initial
`AEClientState(0x8078, true)` call. After drawing that material it
disables the texture-coordinate client state and calls
`glClientActiveTexture(0x84c0)`. The recovered branch now follows the
same transitions. `glClientActiveTexture` already had a compatibility
implementation; its missing declaration was added to the local GLES1
header.

For indexed fixed-function draws, original ARM checks engine byte
`0xfe` (`statsEnabled`) before updating counters. If byte `0xfd`
(`statsBucketFlag`) is zero, it adds unsigned `indexCount / 3` to the
client triangle count at `0x58` and increments draw count at `0x64`.
Otherwise it updates `0x5c` and `0x60`. Non-indexed draws and draws
with statistics disabled skip these updates. The recovered branch now
has these same gates and counter destinations.

The ARM instructions at `0x8609e–0x860e0` show the statistics gate and
counter writes; `0x860f0–0x8612c` show the material check, client-state
reset and texture-channel call. The decompiler's C-like output is used
only to navigate these instructions, not as original source.

Focused ARM verification compiled **209/209** units. `RenderMesh`
source-shape improved from **90.0% to 93.0%**, and strict fuzzy from
**58.3% to 60.9%**. It remains nonexact. Windows native and ARM64
static builds pass.

The full ARM corpus (`_work/render_mesh_fixed_function_full_20261103.json`)
compiled **209/209** units and compared **4,323/4,524** originals in
**4,762** pairs, with **96.23%** mean source-shape, **85.99%** strict
fuzzy, **2,741 linked-exact**, **1,096 byte-exact**, 201 missing and
zero skipped. Against `_work/render_mesh_bit_tests_full_20261103.json`,
the comparison-pair set and both exact-match sets have zero gains and
zero losses. The corpus mean is unchanged at two decimal places.

The fixed-function GL entry points are compatibility stubs in this
project, as they are in the original binary. This patch restores the
source-backed state transitions and counters; it does not establish
visible rendering. The user requested no further Redmi testing.
Next, inspect the remaining `RenderMesh` shader-path differences and
verify visible output on a permitted host when one is available.
