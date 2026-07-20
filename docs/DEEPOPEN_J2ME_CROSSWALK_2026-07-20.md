# DeepOpen J2ME Cross-Version Crosswalk

Date: 2026-07-20

## Scope And Handling

[BaalNetbek/DeepOpen](https://github.com/BaalNetbek/DeepOpen) is a
decompiled and deobfuscated Java/J2ME reconstruction of Galaxy on Fire 2
v1.0.4. This audit used commit
[`d300f93`](https://github.com/BaalNetbek/DeepOpen/tree/d300f93fdb43fc0dd017d53a0d70b3a854345c50).

It is an external, cross-version reference only. It is not vendored,
submoduled, or treated as a build dependency here. The upstream tree contains
J2ME `res/` files and declares no repository license, so copying its source or
resources into this public Android HD reconstruction would not be appropriate.

DeepOpen can clarify broad intent, class ownership, and stable menu semantics.
It cannot prove Android ARMv7 layouts, offsets, bit masks, timing constants,
compiler shape, or byte identity. Android `libgof2hdaa.so` and its matching
IDA/Ghidra evidence remain authoritative for those claims.

## Immediate Crosswalk

| Area | DeepOpen evidence | Android HD evidence and conclusion | Status |
| --- | --- | --- | --- |
| HUD action-menu ownership | `GOF2/Main/MGame.java::handleKeystate` calls `Hud::handleActionMenuKeypress`, then owns pause state and resumes player weapon timing after close. | `MGame::OnTouchEnd` at `0x17a144` consumes touch-menu masks emitted by `Hud::touchEnd`; this corroborates the ownership split: HUD collects UI intent and MGame applies world effects. Touch masks and exact pause fields remain Android-native evidence. | Cross-version corroborated |
| Orbit/autopilot menu order | `GOF2/AutoPilotList.java` has five logical rows: programmed station, current-system jump gate, station, asteroid waypoint, and active route waypoint. | These are the exact five concepts represented by Android HD HUD menu mode 3 and the `0x00200000..0x02000000` action range in `MGame::OnTouchEnd`. This corroborates semantic order only; Java does not prove the Android mask values. | Cross-version corroborated |
| `Hud::hudAction` | J2ME uses `Hud::handleActionMenuKeypress` for keyboard navigation. | Android `Hud::hudAction` at `0x16650c` directly returns zero. This is a platform/input-architecture difference, not a missing Android implementation. Do not port the J2ME handler into Android `Hud::hudAction`. | Confirmed divergence |
| Radar scanner, asteroid, and dead-cargo intent | `GOF2/Radar.java` maintains context/target pairs, considers up to five front-facing asteroid candidates, resets scan time when the candidate changes, and routes dead cargo to tractor-beam behavior. | Android HD already has source-backed scanner and dead-cargo branches. The Java code supports their overall topology but does not identify Android fields such as `Radar+0x19c` or prove a five-element native candidate array. | Intent corroborated; native field mapping needs confirmation |
| Resource graph | `AE/AEResourceManager.java::getGeometryResource` resolves a mesh resource id, lazy-loads geometry, assigns its linked texture resource, caches it, and returns a clone after initial load. | This agrees with the Android HD `PaintCanvas` resource chain documented in `AEM_AEI_NATIVE_LOADER_SPEC.md`: resource id -> mesh/material -> texture. It does not prove HD material flags, AEI encoding, or resource-record layout. | Architectural corroboration |

## Rules For Future Use

1. Cite the DeepOpen commit and the exact Java path whenever it is used as
   supporting evidence.
2. Label an outcome `cross-version corroborated` or `intent corroborated`,
   never Android `source-backed`, until a native Android anchor independently
   supports it.
3. Do not rename Android fields, hard-code Java constants, or claim an ARM
   match based only on DeepOpen.
4. Keep DeepOpen external. Do not commit its Java sources, `res/` contents,
   JARs, or generated build products to this repository.

## Focused Follow-Ups

- Trace Android `Radar::draw` candidate selection and the timer at
  `Radar+0x19c` before assigning it the J2ME `asteroidScanPassedTime` name.
- Audit the Android `MGame` pause/modal prefix separately. DeepOpen is useful
  for state ownership, but its key-based control flow is not a direct touch
  implementation.
- Use the Java resource-manager design only as a checklist while validating
  native `PaintCanvas` resource ids, material links, and AEM/AEI payloads.
