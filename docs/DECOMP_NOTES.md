# Galaxy on Fire 2 — Decompilation Project Notes

> Current project status is maintained in [PROJECT_STATUS.md](PROJECT_STATUS.md).
> This file preserves dated investigations and milestone snapshots. Its figures
> are historical unless they explicitly name a newer measurement date.

Goal: a **matching decompilation** of the full *Abyss Engine* + *Galaxy on Fire 2* game
(C++), recompilable back to byte-identical binaries.

> **Milestone (2026-06-22): the verify gate is GREEN.** `cmake --build cmake-build-match --target
> verify` exits 0 — **0 wrong-signature symbols** (`--fail-on-wrong-type` passes). 198 TUs compile,
> 0 failures. 4347 comparisons, avg 66.9%, linked-exact 1444, byte-exact 714; coverage 4153/4524,
> missing 369, extra 161 (the extras are the informational ctor/dtor-alias + `setPosition3`
> cross-hierarchy items tracked in DEFERRED.md, not gated). The last wrong_type
> (`String::~String` D0) was closed by re-modeling `String` to its native binary layout — see
> STRING_REMODEL.md. Remaining work is per-function byte-match refinement and the deferred extras.

---

> **Native build update (2026-06-30): Windows/MSYS2 UCRT64 native build is GREEN.**
> `cmake-build-ucrt/libgof2.a` builds with GCC 16.1.0, Ninja 1.13.2 and CMake 4.3.x.
> This is a native compile/regression gate, not the ARM byte-match verify gate.
> The same pass restored source-backed implementations for `SpriteGun`, `BeamGun`,
> `PlayerEgo::stopBoost`, `Layout::setWindowDimensions`, `ImageFactory::reload`,
> the `MovingStars` constructor/update/translate/render path, `MeshMerger`, `SimpleMeshMerger`, and
> `LodMeshMerger` merge/transform paths; see the workspace note
> `docs/findings/gof2hd_decomp_recovery_status_20260630_ru.md`.

---

> **Upstream merge update (2026-06-30): synced selected confirmed changes through
> `fionera/gof2hd-decomp@fb04493`.** The native build remains green after importing
> confirmed `Radar` ABI offsets (`lockedStation@0x24`, `turretScopeHalfWidth@0x12c`),
> `Mesh::pivotZ`, weapon runtime layout (`Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`,
> `ObjectGun`), FMOD helper routing including `FMOD::EventSystem::init/update`,
> `AERandom::nextInt` helper routing, and small PaintCanvas/SpaceLounge helper routes.
> A follow-up IDA/Ghidra ctor/export audit corrected `SpriteGun` constructor
> emission and kept `AbstractGun`'s destructor inline so we do not invent an
> unobserved base destructor/vtable symbol.
> Remaining upstream `Hud`, `AEGeometry`,
> `ParticleSystem`, `MenuTouchWindow`, and larger PaintCanvas helpers are queued for
> focused confirmation rather than copied wholesale.

---

> **Radar behavior update (2026-06-30):** first body pass is native-build green.
> `Radar` now has 32-bit layout probes, constructor resource/metric/equipment
> initialization, key scanner/plasma/station/turret accessors, and
> `Globals::Canvas` update/draw routing. `PlayerEgo::dockToDockingPoint` was
> corrected to use the recovered `KIPlayer*` navigation/reservation argument
> instead of treating the unused `Radar*` argument as that object. Full
> `Radar::draw` / `Radar::drawCurrentLock`, lock-label string population, and
> `Status::getSystem()` typing remain queued.

---

> **Helper-route update (2026-07-01):** visible upstream master updates were
> checked through top commit `1ab7149`. A focused route package replaced
> selected unresolved `_ae_*` / `_psm_*` call sites with existing local methods:
> `AEGeometry` now routes transform/mesh creation, translation, and
> `moveForward` helpers through `PaintCanvas`, `AEMath`, and `AEGeometry`;
> `ParticleSystemManager` routes mesh rendering/release helpers; and
> `ParticleSystemMesh` routes quad-edge and update helpers to its member
> bodies. Native build remains green. Remaining PaintCanvas/MenuTouchWindow
> helper campaigns are queued separately.

---

> **Upstream byte-match update (2026-07-01):** a fresh upstream clone was
> checked through `fionera/gof2hd-decomp@7995687`. Transferred small fixes for
> `Ship::addMod`, `ListItem::isTab`, `PaintCanvas` text helper calls,
> `MeshCreateFromFile` AEM/V2-V5 magic detection, and the `EngineFlags.cpp`
> split for feature-flag externs. Native build remains green. Large
> `BuildResourceList`, `Globals`, broad helper campaigns, and
> PaintCanvas/MenuTouchWindow chunks remain queued separately.

---

> **BuildResourceList update (2026-07-01):** imported upstream `30ce2d0`.
> The first 2488-resource table now uses a stack-local `Resource *resources[]`
> and inline `ADD_RES` construction so Resource allocation precedes payload
> allocation and field fills. Native build remains green; local ARM byte-match
> is still unclaimed.

---

> **AEM/AEI loader note (2026-06-30):** source/call-site tracing through
> `MeshCreateFromFile`, `MeshReadData`, `Mesh::ReadEnhancedDataFromFile`,
> `ImageCreateFromFile`, `ImageCreateRegionFromFile`, and `PaintCanvas`
> resource creation has been summarized in `docs/AEM_AEI_NATIVE_LOADER_SPEC.md`.
> Important importer correction: V4 AEM files begin with `V4AEMesh\0`,
> `vertexFormat`, `topLevelMeshCount`, then a 12 byte `f32[3]` pivot before
> `indexCount`; offset `0x18` is not a universal vertex count.
> Follow-up importer pass: `tools/aem_to_glb_fx.py` now consumes the recovered
> C++ resource table shape for `ResourceMesh -> ResourceMaterial ->
> ResourceTexture` joins, writes joined material/texture metadata into GLB
> extras and `*.native.json`, exports `COLOR_0`, supports explicit UV policy,
> and emits glTF translate/scale channels for known enhanced animation groups.

---

## 1. Source inventory & triage

| Source | File | Format / Arch | Encrypted | Named funcs | Toolchain | Verdict |
|---|---|---|---|---|---|---|
| **iOS 1.1.4 HD** | `Original IPA/...HD_1.1.4_ios_4.3.ipa` → `GalaxyOnFire2 HD` | Mach-O **armv7** | **No** (`cryptid 0`) | 664 (653 C++ methods, 27 AE classes) | Xcode SDK 6.0, **llvm-gcc/clang + libstdc++** | **Co-primary** (unencrypted; iOS platform layer + statically-linked FMOD) |
| **Android 2.0.16 HD** | `Original APK/...2.0.16_APKPure.apk` → `lib/armeabi-v7a/libgof2hdaa.so` | ELF **ARMv7** | No | **4524** (4075 C++ methods, 133 AE classes) | **NDK r18, clang 7.0.2 (LLVM 7.0.2) / GCC 4.9** | **Primary** (richest symbols; reproducible toolchain) |
| iOS 1.0.5 | `Original IPA/...1.0.5_ios_3.0.ipa` → `GalaxyOnFire2` | Mach-O universal armv6+**armv7** | No | 162 (155 methods, 25 AE classes) | older Apple GCC | Secondary cross-ref (matches DeepOpen 1.0.1 era) |
| Windows Steam | `Original Windows/.../GOF2.exe` | PE32 **x86** | No | 0 (stripped) | MSVC (vc10) | **Reference only** — cut content/features; ModApi offsets validate struct layouts |
| KiritoJPK Android | `KiritoJPK_.../GOF2FHD_APK.7z` + OBB | (7z, not yet extracted) | — | — | — | Modded/repacked Android — assets/OBB reference only |
| DeepOpen | [BaalNetbek/DeepOpen at `d300f93`](https://github.com/BaalNetbek/DeepOpen/tree/d300f93fdb43fc0dd017d53a0d70b3a854345c50) | **Java (J2ME)**, v1.0.4 | — | — | — | **External logic reference (not byte-match)** — deobfuscated/refactored Java decomp of the *same codebase lineage*: classes map ~1:1 to the C++ (`AEMath`/`AEMatrix`/`AEVector3D`, `AECamera`, `TargetFollowCamera`; `PlayerEgo`, `KIPlayer`, `Status`, `Ship`, `Mission`, `LevelScript`, `Globals`, `Item`, `Agent`, `Radio`…). Use it to understand **what a murky decompiled function does** (algorithms, control flow, field semantics), then verify against Android HD. Wrong language + earlier version => never codegen/matching. It is deliberately not vendored: the upstream tree includes J2ME resources and declares no repository license. See `DEEPOPEN_J2ME_CROSSWALK_2026-07-20.md`. Also `extras/gof2-1.0.1-ios-symbols` (naming ref). |

### Why these targets
- **Both iOS binaries are NOT FairPlay-encrypted** (`LC_ENCRYPTION_INFO cryptid 0`) → `__TEXT` is fully readable. This is what makes iOS usable at all.
- **Android `.so` has by far the richest symbol table**: 4524 exported functions (no `-fvisibility=hidden`), demangling to **133 `AbyssEngine` classes** + the full game class hierarchy. It is the best map of the engine+game structure.
- **The recovered C++ source is shared across all platforms** (Abyss Engine + game). We name/structure from the richest binary (Android), cross-validate against iOS, and pick a matching-build target per platform.

### Code architecture (from symbols)
- Engine namespace: **`AbyssEngine`** (~1001 methods) — `Engine`, `Mesh`, `Transform`, `Camera`, `Material`, `String`, `Array<T>`, `Image2D`, `SpriteSystem`, `AEMath::Matrix`, file I/O (`AEFile`, `AEPakFileEntry`, `AELowLevelFile`), sound (`AESoundInterface`), shaders…
- Game lives in top-level classes (Android-only symbols, 4392 unique): **`PlayerEgo`** (player ship, 168), `Status`, `Player`, `Ship`, `Level`/`LevelScript`, **`KIPlayer`** (enemy AI; "KI" = German for AI), `Item`, `Mission`, `Hud`, `Agent`, `Radio`/`RadioMessage`, weapons (`BombGun`, `MineGun`), `Achievements`, and the **`NFC::`** namespace (store / DLC, e.g. `NFC::iap_buy_dlc_valkyrie()`).
- iOS-only delta (538): mostly **FMOD** (392, statically linked on iOS; a separate `libfmodex.so` on Android) + template instantiations + a few file classes.
