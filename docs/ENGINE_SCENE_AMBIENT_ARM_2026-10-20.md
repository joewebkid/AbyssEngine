# Engine scene ambient and StarSystem light gate — 2026-10-20

The original Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
shows `Engine::LightSetGlobalSceneColorAmbient` at `0x85830`. At `0x85836`
it loads `0x3f800000`, then the store at `0x85840` writes that word after
the three input color words at Engine offset `0x288`. This is the IEEE-754
representation of `1.0f`, not the numeric float conversion of integer
`0x3f800000`. The production C++ now assigns `1.0f` to `sceneAmbient[3]`.
The focused ARM test checks the original instruction sequence.

The continuous Pan original-asset Level/GLES gate now crosses the first
real Engine light setter after all five orbit planets, RNG reset and the
real `ApplicationManager::GetEngine` calls. It verifies the ambient RGB
from the Pan sun color, alpha `1.0f`, and the fixed-function
`glLightModelfv(0xb53, values)` call, on Windows SwiftShader and Android
ARMv7/Adreno at `-O0/-O2`. The GLES1 compatibility call is a bounded host
service that checks arguments; the original binary's GLES1-compatible
entry points are inert. The shader-enabled branch is not exercised.

Native `gof2` and ABI/lifetime smoke pass; 208/208 ARM translation units
compile and 89 verifier tests pass with one pre-existing skip.
Focused `Engine::LightSetGlobalSceneColorAmbient` rises from **91.3%/52.2%**
to **93.4%/68.1%** source-shape/strict fuzzy, still nonexact. The full ARM
corpus compares **4,323/4,524** originals in **4,735 pairs** at **93.93%**
mean source-shape and **83.22%** strict fuzzy, retaining **2,551
linked-exact** and **1,079 byte-exact** pairs. Independent `(unit, symbol)`
set comparison with the previous report finds zero exact gains or losses;
skipped units and wrong-type signatures are zero. The ignored report is
`C:/Users/Dima/AppData/Local/Temp/gof2-decomp-20261020/light-ambient-full.json`
(SHA-256 `53beb4739cda71c4412ce6f9d411a979db413be872dd444527b8e43fea3c832f`).

The gate stops before `Engine::LightSetRimColor`. Remaining light setters,
shader-enabled color propagation, fog routing, `StarSystem::initLight`
completion, constructor return and Level stage 0 are not verified. The
next integration step is to execute these real setters with observable
Engine state, then cover fog variants and constructor return.
