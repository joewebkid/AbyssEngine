# ModStation station-light recovery (2026-10-05)

Scope: `ModStation::resetLight()` and the two float fields it reads at
`+0x120` and `+0x124`. Primary source is Android 2.0.16
`libgof2hdaa.so:_ZN10ModStation10resetLightEv` at `0x0d70bc..0x0d7200`.
The previous C++ body dereferenced a null placeholder and called unresolved
fragment helpers; it could not correctly set the station light.

The original's GOT references at `0x20fd50` and `0x210374` identify
`Globals::appManager` and `ApplicationManager::GetEngine`. Its direct calls
identify the existing `Engine` lighting methods. The recovered body now:

- sets material ambient to `(0.7, 0.7, 0.7)`;
- passes the negated float components at ModStation `+0x11c`, `+0x120`,
  `+0x124` to light direction index `0x4000`, then sets its diffuse color
  to `(1, 1, 1)`;
- selects the following light ambient color using
  `Status::getSystem()->SolarSystem::getRace()`;
- sets specular color to `(0.5, 0.5, 0.5)` and material shininess to `96`.

| Race | Ambient RGB |
| ---: | --- |
| 1 | `(0.25, 0.55, 0.25)` |
| 2 | `(0.15, 0.15, 0.35)` |
| 3 | `(0.45, 0.25, 0.25)` |
| 8 | `(0.83, 0.28, 0.12)` |
| Other | `(0.25, 0.25, 0.25)` |

The float literals come from the function's literal pool and its Thumb
switch table. The final tail branch goes through an interworking veneer to
`Engine::LightSetMaterialColorShininess(float)`, not to `LightEnable` as the
old placeholder suggested. ARM32 offset assertions protect the three
direction fields. The `+0x11c` field is also used as a scrolling/light
target elsewhere, so its state ownership remains unresolved; the read and
negation in this function are established, not a global field renaming.

## Validation

Focused ARM comparison improves from 28.9% source-shape / 15.2% strict to
100% / 100% over 108 instructions on each side. The normalized instruction
sequences are identical, but the function is **not linked-exact or
byte-exact**. The UCRT64 `gof2` static library and all 206 ARM translation
units build. Reports:
`_work/blueprint-isunlocked/modstation_resetlight_{before,after}.json`.

The full `--fail-on-wrong-type` corpus compares 4,322/4,524 original
functions across 4,675 rows, with 202 absent, zero wrong-signature and zero
skipped. Mean source-shape rises from 90.11% to 90.13%, and strict fuzzy
from 77.25% to 77.27%. Linked-exact remains 2,165 and byte-exact 977;
exact `(unit, symbol)` set comparison finds zero losses or gains. The
complete report is `_work/blueprint-isunlocked/modstation_resetlight_full.json`.

No graphical/in-game light rendering was exercised; lighting state and
OpenGL context integration need a later runtime test. The next bounded
source-recovery priority is `ModStation::enterStation()`, whose current
source still calls a null `getShip` placeholder; that is a separate issue.
