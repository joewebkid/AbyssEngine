# Level Particle Systems Recovery

## Scope

This packet covers the player-engine configuration loop in
`Level::initParticleSystems()`. The previous C++ body allocated `field_a8` but
omitted the Android loop that creates and configures a particle system for each
engine-nozzle geometry in `field_a4`.

## Primary Evidence

| Source | Location | What it establishes |
| --- | --- | --- |
| Android ARMv7 `libgof2hdaa.so` | `Level::initParticleSystems`, `0x000bd6f8` | Complete control flow and call arguments. |
| IDA dump | `analysis/gof2_libgof2hdaa_full_ida.c`, function at source line 111871 | Loop writes, constants, slot stride, and atlas-variant lookup. |
| Ghidra dump | `analysis/gof2_gidra_engine_full_source.c`, function at source line 88528 | Typed recovery of `field_80`, player matrix `Player+0x04`, and particle-set `0x1d + i` arguments. |
| Android rodata | `0x1fc3d0`, `0x1fdba0`, `0x1fdbd0`, `0x1fdc00`, `0x1fdc30` | 64-entry ship-to-atlas map and four nine-entry UV tables. |
| Android symbol table | `_ZN21ParticleSystemManager9addSystemEPKN11AbyssEngine6AEMath6MatrixEN16ParticleSettings11ParticleSetEb` at `0x183925` | `addSystem(matrix, ParticleSet, bool)` call contract. |

The local original binary used for inspection remains ignored under
`_work/bins/`; this document and the recovered source contain no game binary or
asset data.

## Confirmed Android Behavior

When `Level::player` and `field_a4` exist, the native body:

1. Allocates `field_a8` with one integer handle per `field_a4` entry.
2. For nozzle `i`, creates `field_80->addSystem(&player->transformMatrix,
   ParticleSet(0x1d + i), false)` and stores the handle in `field_a8[i]`.
3. Mutates the matching `ParticleSettingsRef::cur[0x1d + i]` slot, whose ARM
   stride is `0xa0`: nozzle local position, scale-derived lifetime and size,
   `count = 20`, `flLifetime = 8.0`, `posBase = -1000`, scale-derived
   `velDir`, color/fade values, and sprite UVs.
4. Uses a 64-entry, one-based ship atlas selector. A zero selector deliberately
   retains the default UV rectangle; selectors 1 through 9 choose one of nine
   Android UV rectangles.

`Level::setPlayerEngineColor(short)` is part of the same data path. It writes
the packed RGB value into `color0` for the same dynamic slots beginning at
`ParticleSettingsRef::cur + 0x1254` (`slot 29`, `color0 + 0x34`). The previous
local null `g_engineColorBase` stand-in was replaced with this direct table
route.

## Local Representation

`Level.cpp` now uses `EngineParticleSetArm`, a small fixed-offset view over
`ParticleSettingsRef::cur`. This is intentional: host UCRT64 pointer widths do
not preserve the Android `String` layout, while the original settings storage
and all writes in this function are explicitly ARM `0xa0`-byte slots.

The view has compile-time checks for the stride and the offsets used by this
body (`count`, `color0`, `posBase`, `velDir`, and `uvU0`). It is a layout view,
not a claim that the whole host-side `ParticleSettings` class is ABI-correct.

## Validation

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

Result on 2026-07-21: `libgof2.a` linked successfully. Existing
pointer-to-`long` warnings in `Level::renderBG` remain outside this packet;
the restored particle loop emitted no new warning.

## Remaining Work

- `ParticleSettingsRef` and the broader host-side particle runtime still need
  their own layout/runtime audit; this pass does not claim a runnable particle
  renderer.
- No ARM matching build was run for `Level::initParticleSystems`; the code is
  source-backed, not byte-matched.
- The later sky, pirate-base, supernova, and generic manager setup branches of
  `initParticleSystems` were already present and were not reinterpreted here.
