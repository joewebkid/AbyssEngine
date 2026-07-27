# ParticleSettingsRef ABI and Runtime Audit

## Scope

This package audits the Android ARMv7 particle-definition storage used by
`ParticleSettingsRef`, `IParticleSystem`, `ParticleSystemManager`, and the
player-engine loop in `Level`. It replaces host-only byte buffers and null
global stand-ins with the recovered `cur` and `init` runtime contract.

## Primary Evidence

| Source | Location | Established fact |
| --- | --- | --- |
| Android ARMv7 `libgof2hdaa.so` | static construction `0x694dc` | Constructs both `ParticleSettingsRef::init` and `ParticleSettingsRef::cur`. |
| Android ARMv7 `libgof2hdaa.so` | `ParticleSettings::ParticleSettings`, `0x183fe8` | Iterates `48` entries at a `0xa0` stride. |
| Android ARMv7 `libgof2hdaa.so` | `ParticleSettingsRef::initialize`, `0xe3964` | Calls `cur.init()`, then `init.init()`, then stores `42` in `assertInit`. |
| Android ARMv7 `libgof2hdaa.so` | `ParticleSettings::multiplyAll`, `0x193e68` | Reads baseline values from `init` and writes scaled values to `cur`. |
| Android ARMv7 `libgof2hdaa.so` | `ParticleSettings::Interpolate`, `0x193f4c` | Blends two `init` entries into a `cur` entry. |
| Android ARMv7 `libgof2hdaa.so` | `ParticleSystemManager::initSprites`, `0x1936c8` | Reads the default live UV rectangle at offsets `0x88..0x94`. |
| Android ARMv7 `libgof2hdaa.so` | `IParticleSystem` call sites | Addresses live definitions as `ParticleSettingsRef::cur + set * 0xa0`. |

The checked-in code and this document contain no original game binary.

## Confirmed ABI

`ParticleSettings` contains `48` `SetDefinition` slots. Each slot is exactly
`0xa0` bytes and has these confirmed offsets:

| Offset | Field |
| --- | --- |
| `0x00` | opaque Android `String` storage (`12` bytes) |
| `0x0c` | flags |
| `0x10` | particle count |
| `0x34` | first packed color |
| `0x70` | directional velocity |
| `0x88..0x94` | atlas UV rectangle |

`ParticleSettingsRef::cur` is the live mutable table. `init` is the baseline
table retained for scaling and interpolation. Both are real static
`ParticleSettings` objects of `0x1e00` bytes, not pointer-sized host buffers.

## Runtime Changes

- `ParticleSettingsRef::initialize()` now invokes the two real objects in the
  Android order and stores `0x2a` in `assertInit`; the prior null pointers were
  removed.
- `ParticleSettings::multiplyAll` and `Interpolate` now follow the Android
  `init -> cur` data flow rather than treating an arbitrary host object as the
  table.
- `IParticleSystem` and `ParticleSystemManager` no longer dereference null
  definition pointers. The sprite manager now forwards all four default UV
  values (`U0`, `V0`, `U1`, `V1`) instead of two values and two fabricated
  zeroes.
- `Level` consumes the shared type rather than maintaining a private duplicate
  ABI struct.
- The zero-filled 392 KB name-string placeholder was removed. It never wrote
  names into the recovered slots and only introduced rodata bloat.

## Host Adaptation Boundary

The first `0x0c` bytes of a slot are preserved as opaque storage. Android has
a 32-bit `String` there, but the host `String` has a different pointer width.
No current particle renderer consumer reads the name. This is a deliberate ABI
adapter, not a claim that original particle names or their string table are
recovered.

The full particle renderer still carries unrelated shim symbols and cannot yet
link as a complete host executable. This audit verifies the independent
definition-table runtime only.

## Validation

```powershell
C:\msys64\ucrt64\bin\cmake.exe -S . -B cmake-build-ucrt -G Ninja `
  -DGOF2_BUILD_PARTICLE_SETTINGS_SMOKE=ON
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt `
  --target gof2_particle_settings_smoke
.\cmake-build-ucrt\gof2_particle_settings_smoke.exe
```

The smoke test calls `ParticleSettingsRef::initialize`, checks static-table
defaults and separation, then exercises `multiplyAll(2.0f)` and
`Interpolate(0, 4, 0.25f, 42)`. Result on 2026-07-21:

```text
ParticleSettingsRef ABI/runtime smoke: OK
```

The normal `gof2` UCRT64 static-library build also links successfully.

## Remaining Work

- Extract and map the original particle-name string payload for rodata and
  name-level parity.
- Recover the remaining particle render shim bodies, then test the full
  manager/renderer path in a host executable.
- Run a dedicated ARM source-shape and byte-match pass after the runtime body
  has broader coverage.
