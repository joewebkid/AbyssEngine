# All orbit planets and StarSystem light getter — 2026-10-20

## Original ARM evidence

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
keeps a 24-word occupancy table and retries `nextInt(11)+7` when the
candidate is near the sun or occupied (`0x13425e–0x134284`). The selected
station follows the separate distance/scale and rotation branch described
in [the first-planet note](STARSYSTEM_FIRST_PLANET_ARM_2026-10-20.md).

At `0x1348b8–0x1348d0`, `StarSystem::initLight` loads a function address
through GOT slot `0x210374` and calls it. The original ELF's
`R_ARM_GLOB_DAT` relocation at that slot resolves to
`ApplicationManager::GetEngine()`. The former C++ assigned the equivalent
function pointer to `nullptr`, so executing `initLight` would crash. The
production source now calls the real `GetEngine`; a focused ARM test checks
the disassembly and relocation together.

## Executed boundary

The continuous Pan original-asset Level/GLES gate runs through all five
orbit planets on Windows SwiftShader and Android ARMv7/Adreno at `-O0/-O2`.
The selected station (index 7) inherits sector 17 and scales to 23.495;
the next candidate 17 is rejected as occupied and sector 14 is accepted;
the final candidate 14 is rejected and sector 10 is accepted. The gate
checks each corresponding RNG state, scale, pose, copied position, and real
`PlayerStatic` object. It observes 18 meshes, 22 transforms, 44 collision
volumes, and 26/26 closed file opens.

After the planet loop, the extended gate executes the real `AERandom::reset`
and the start of production `StarSystem::initLight`. It confirms that both
the initial and repeated `ApplicationManager::GetEngine` calls return the
fixture's real `Engine`, sets `engine->field_0x32c` to 2, and reads the
expected Pan sun color before the first light setter. The fixture supplies
the already-existing global application-manager service and stops before
unimplemented light methods; those remain fail-closed, not silently mocked
as successful lighting.

Native `gof2` and ABI/lifetime smoke pass; **208/208** ARM units compile and
**88 verifier tests** pass with one pre-existing skip. Focused
`StarSystem::initLight` changes **24.6%/11.0% → 81.3%/44.9%**
source-shape/strict fuzzy, still nonexact. Constructor aliases change
**85.2%/19.2% → 85.6%/40.6%**, also nonexact. The ignored focused report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-getter-star-unit.json`.

The complete ARM corpus compares **4,323/4,524** original functions in
**4,735 pairs**. Mean source-shape rises **93.92% → 93.93%** and strict
fuzzy **83.20% → 83.21%**. It retains **2,551 linked-exact** and **1,079
byte-exact** pairs; independent `(unit, symbol)` set comparisons against
the previous first-planet report find zero exact gains or losses. Skipped
units and wrong-type signatures remain zero. The ignored full report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-getter-full.json`
(SHA-256 `485e71a53bdd1c6627b6bd4de0d92f689e18b67fbb19179a42afffedda6b57e1`).

## Limits and next priority

The gate stops before `Engine::LightSetGlobalSceneColorAmbient`. Real light
setters, fog routing, complete `StarSystem::initLight`, constructor return,
and Level stage 0 remain unverified. The next step is to connect real Engine
light methods in this same continuous original-asset gate and verify their
observable state before allowing StarSystem construction to return.
