# Project Status

Last reviewed: 2026-08-17

## How To Read Progress

Three different measures are tracked here:

1. **Buildability**: whether the host-side static library compiles.
2. **Source recovery**: whether a class, layout, or behavior has direct evidence.
3. **ARM matching**: whether a matching-toolchain build equals or resembles the
   original instructions for a function.

They are related but not interchangeable. A function can compile while still
being a placeholder; a source-backed body can be useful while not
byte-identical; a strong instruction match does not prove every semantic name.

## Measured Snapshot

The committed `report.json` snapshot reports:

| Metric | Value |
| --- | ---: |
| Original functions discovered | 4,522 |
| Functions compared by exact mangled name | 4,507 |
| Linked-exact functions | 1,697 |
| Raw-byte-exact functions | 948 |
| Average instruction similarity | 67.54% |
| Missing exact-name counterparts | 15 |
| Wrong-signature counterparts | 0 |

This is a saved measurement, not a permanent project score. Re-run the
validator locally for a new result. High comparison coverage does not mean the
entire program is semantically recovered: it includes compatibility bodies,
small accessors, and code that still needs review.

The native UCRT64 target `gof2` is currently a green compile gate and produces
`cmake-build-ucrt/libgof2.a`.

## Source Inventory

| Area | Files | Status |
| --- | ---: | --- |
| `src/` total | 204 C++ files and 234 headers | Compilable host-side tree, not a finished game source release. |
| `src/engine/` | Shared engine systems | Partial recovery; layouts and helper paths remain under audit. |
| `src/game/` | UI, ships, missions, weapons, and world logic | Partial and uneven; high-value packages received focused passes. |
| `src/platform/android/` | 4 files | Android/JNI boundary representation, not a drop-in application. |

There is no trustworthy global percentage for semantic source recovery. A
percentage attached to a named package is a local evidence estimate, never a
percentage for the whole game.

## External Cross-Version Evidence

DeepOpen J2ME v1.0.4 is pinned at commit `d300f93` as an external shared
workspace reference at `../references/DeepOpen/`; it is not part of this
Android HD decompilation repository. Its J2ME menu, Radar, and resource-manager
bodies are useful to corroborate intent and stable semantic ordering, but they
cannot establish Android ARMv7 fields, masks, timing, or byte shape. The
initial MGame/HUD, Radar, and resource-chain comparison is recorded in
`DEEPOPEN_J2ME_CROSSWALK_2026-07-20.md`.

## Notable Packages

| Package | Current understanding | ARM status |
| --- | --- | --- |
| `MenuTouchWindow` | Draw/touch routing, load/save, settings, HUD layout, DLC/store, and language-list paths received focused source-backed passes. Local source-backed coverage is estimated around 65-70%; constructor and exact-shape work remains. | Recent body work is native-build checked; no whole-class byte-match claim. |
| `Radar` | Constructor/setup, target classification, lock/HUD behavior, asteroid/dead-cargo branches, and selected audio/scanner behavior received focused recovery. | `Radar::draw` remains a large source-shape and byte-match task. |
| `Hud` | Core-frame work recovered direct `Globals` routing, `PlayerEgo::level`, the `Hud::init` image/coordinate map, exact 20-entry event queue, four quick-menu construction modes, menu frame/cargo gauge, status/gamma rows, and linked gamma runtime. The complete `hudEvent` switch covers events `1..47`, Android GameText/sound routing, duplicate suppression and the `0x100019` important-event mask. Four staged runtime migrations now place every live field from `+0x000` through `+0x53b` at its Android ARM offset: Strings, event/touch state, 71 images, all packed coordinates, flash/hit timers, camera arrays and final ownership fields. The real class is compile-locked at `sizeof(Hud) == 0x53c`; the out-of-line `ArrayReleaseClasses<TouchButton *>` specialization is `100%` linked-exact. The pause button and right action cluster use the native low-byte mask, confirmed Image2D pairs, flash ordering, readiness timers and `Level+0x69` delayed-secondary lifecycle. The source-backed `Hud::draw` prelude now gates the event queue, remaps persisted iPad control anchors, restores the original canvas color on both exits, and leaves pause/orbit/menu orchestration to `MGame::OnRender2D`. See `HUD_ARM32_ABI_LAYOUT_2026-08-16.md`, `HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`, `HUD_RUNTIME_TAIL_2026-08-17.md`, `ARRAY_TOUCHBUTTON_ARM_ABI_2026-08-17.md`, `HUD_PAUSE_BUTTON_ARM_2026-08-17.md`, `HUD_ACTION_CLUSTER_ARM_2026-08-17.md`, and `HUD_DRAW_PRELUDE_IPAD_QUEUE_2026-08-17.md`. | UCRT64 is green; ARM corpus is `201/204` with three unrelated known failures. Constructor is linked-exact at `100%`; destructor is `97.5%` linked-exact at `80/80` instructions; `drawPauseButton` is `92.7%` linked-exact; `Hud::firePressed` is byte-exact; `Hud::draw` is `16.4%`; `Hud::init` is `22.1%`, touchEnd `92.3%`, getAnalogX/Y `90.9%`. Across 48 Hud functions, average similarity is `69.3%`, with 22 linked-exact and 16 raw-byte-exact. The lower fuzzy `Hud::init` score follows removal of a non-native zero-anchor fallback; exact-function counts are unchanged. Large draw/init/event bodies and the final destructor unwind tail remain. |
| Hangar / `ModStation` | `HangarList`, `HangarWindow`, `ListItemWindow`, scene-23 assembly and station camera routes received a focused source-backed pass. The package replaces local placeholder globals with typed Status/GameText/Item routes, restores the 64-entry preview mesh tables and direct native camera tables, records the original `BuildResourceList` mesh entry for resource `6801`, and types the 24 native Hangar action slots plus Android row metrics at `Layout+0x238/+0x248/+0x24c/+0x250`. See `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md`. | Native UCRT64 build is a compile gate only. Exact preview camera/output shape, remaining platform layout branches, untyped market tails, and ARM byte matching remain separate work. |
| `MGame` touch routing | The `Hud::touchEnd` branch now covers pause, primary-fire/hacking/auto-turret combat actions, camera/maneuver release, quick-menu/orbit actions, and the paused autopilot-menu, StarMap, ordinary ChoiceWindow, `MGame+0xcf` selector, MenuTouchWindow, non-terminal dialogue, and failed dialogue cleanup routes. See `MGAME_HUD_ACTION_ROUTING_2026-07-20.md`, `MGAME_PAUSED_TOUCH_ROUTING_2026-07-20.md`, and `MGAME_ARM_MATCH_2026-07-20.md`. | Focused ARM verification is live: current `OnTouchEnd(int,int,void*)` is 3.6% fuzzy, not linked- or raw-byte-exact. `MGame+0xca`, boost, game-over, and successful terminal dialogue/cutscene remain separate work. |
| `Level` particle setup | The player-engine loop in `Level::initParticleSystems` creates `field_80` systems for `ParticleSet(29 + nozzle)`, records their handles in `field_a8`, and applies confirmed Android scale, position, color, and per-ship atlas UV data to the live `ParticleSettingsRef::cur` table. The companion ABI/runtime audit recovered the paired `cur`/`init` `48 x 0xa0` tables, direct manager/system consumers, baseline scaling/interpolation, and a native smoke test. See `LEVEL_PARTICLE_SYSTEMS_2026-07-21.md` and `PARTICLE_SETTINGS_REF_ABI_RUNTIME_2026-07-21.md`. | Native UCRT64 build and the isolated particle smoke target are green. Original particle-name payloads and full render-path/ARM byte matching remain separate work. |
| Mesh merging | `MeshMerger`, `SimpleMeshMerger`, and `LodMeshMerger` have recovered merge and transform paths. | Full ARM verification remains package-specific. |
| Weapons | `Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`, and related runtime helper work have focused evidence. | Constructors, vtables, and bodies remain under ARM audit. |
| Resource loading | AEM/AEI loader and resource-table relationships are documented from native call paths. | Format understanding is not a claim that all resource paths are finished. |

## Priorities

1. Recover large behavior bodies only with an evidence packet and uncertainty list.
2. Audit shared layouts before cosmetic per-function rewrites.
3. Continue `Radar::draw` source-shape work without regressing exact functions.
4. Recover `Hud`, `Globals`, `ModStation`, `MGame`, mission, economy, AI, and
   platform helpers as separate packages.
5. Keep public documentation synchronized and do not import original game data.

## Evidence Terms

- **Source-backed**: tied to a named native, iOS, Java, symbol, or directly
  equivalent call-path source.
- **Confirmed**: cross-checked independently or supported by a matching result.
- **Needs confirmation**: plausible mapping awaiting stronger evidence.
- **Heuristic**: practical approximation with no claim of original behavior.

The original Android binary is required only for local verification and is
ignored in `_work/bins/`. Do not add proprietary binaries, game assets, APKs,
IPAs, OBBs, JARs, or extraction dumps.
