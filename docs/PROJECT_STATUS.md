# Project Status

Last reviewed: 2026-08-20

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
| `Hud` | Core-frame work recovered direct `Globals` routing, `PlayerEgo::level`, the `Hud::init` image/coordinate map, exact 20-entry event queue, four quick-menu construction modes, menu frame/cargo gauge, status/gamma rows, and linked gamma runtime. The complete `hudEvent` switch covers events `1..47`, Android GameText/sound routing, duplicate suppression and the `0x100019` important-event mask. Four staged runtime migrations now place every live field from `+0x000` through `+0x53b` at its Android ARM offset: Strings, event/touch state, 71 images, all packed coordinates, flash/hit timers, camera arrays and final ownership fields. The real class is compile-locked at `sizeof(Hud) == 0x53c`; the out-of-line `ArrayReleaseClasses<TouchButton *>` specialization is `100%` linked-exact. The pause button and right action cluster use the native low-byte mask, confirmed Image2D pairs, flash ordering, readiness timers and `Level+0x69` delayed-secondary lifecycle. The source-backed `Hud::draw` prelude gates the event queue, remaps persisted iPad control anchors, restores the original canvas color on both exits, and leaves pause/orbit/menu orchestration to `MGame::OnRender2D`. Its hit-direction follow-up restores the four 300 ms direction pulses, Radar-centred transforms, hacking-aware steering dimming, and distinct rocket/normal color exits. The mission-panel pass restores kill-counter, passenger, production, ordinary cargo and countdown dispatch. The reticle/dock/camera pass restores hacking controls, disabled tint, alien-orbit dock exceptions, time-extender flow, autopilot dock substitution, direct camera tables and the auto-turret/banner split. The quick/secondary/fire audit restores native pressed/idle branches, the direct Radar target predicate and the correct `fireForTutorial` main-fire state. The progress/mining tail separates the native dock-fill draw image from its geometry-metrics image and exposes the signed 64-bit `LevelScript+0x08` timer. The first whole-function lifetime pass now inlines the native iPad remap and both steering copies, restores the seven-object secondary-label chain, and matches the dock progress query/String order. The boost/protector follow-up restores the three native boost draw paths and selects the evidence-backed `-fstack-protector-strong`. The mission/cargo lifetime pass restores direct `Globals::status/Canvas` reloads, native production `getCargo` repetition and the eight temporary String slots. The progress/mining control-flow pass removes non-native singleton guards, restores the shared jump/cloak body, fresh mining predicates, single pulse-timer store and white color reset. The prelude/health-bar pass now caches post-remap iPad anchors, restores boost read lifetimes, uses native `(rate * 0.01f) * width` recurrence, and joins armor coordinates through the confirmed address pairs. The gamma/rocket/hit-entry pass restores the native sub/add gamma-coordinate recurrence, the uncollapsed secondary-flash branch, and offset-selected shield/armor hit images. The challenge/orbit/menu ABI pass restores the live challenge-score body, orbit color stride, event 27, exact secondary setter, `GameSettings` iPad anchors and the corrected `Hud+0x4cc..0x4f0` quick-menu/touch layout. The touch/menu/cargo pass restores native phone/iPad hitbox routing, quick-menu frame iteration, real cargo replacement markers, full-width event text Y and the confirmed `Hud+0x3c0` secondary-label position. The init/event pass restores native quick-menu allocation order, equipment/station String lifetimes, duplicate-event queue construction and exact medal text. Its source-shape follow-up removes the non-native event-line alias, confirms event 19 as `Hud+0x100`, restores branch-local queue-string allocation, and returns phone/iPad hit testing to native field-load lifetimes. The queue-family pass restores the integer `updateQueue` ABI, 2000/4000 ms lifecycle, source-order banner fade/slide and a byte-exact slot insertion scan. The dedicated `Hud::init` pass removes the non-native image descriptor stack table, restores all 75 static image call sites, native queue/key allocation order, ship capability queries, integer pause coordinates and the 32-byte layout copy. The cargo/menu/touch/orbit source-shape pass restores the typed `Status::replaceHash` hidden return, native cargo String expressions, draw-site coordinate reads, nonzero-element-first touch routing and orbit Canvas/font lifetimes. Generated `Hud::draw` size is now `3033/3223`, while stack reservation and the VFP save set match at `224` bytes and `d8-d11`. See `HUD_ARM32_ABI_LAYOUT_2026-08-16.md`, `HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`, `HUD_RUNTIME_TAIL_2026-08-17.md`, `ARRAY_TOUCHBUTTON_ARM_ABI_2026-08-17.md`, `HUD_PAUSE_BUTTON_ARM_2026-08-17.md`, `HUD_ACTION_CLUSTER_ARM_2026-08-17.md`, `HUD_DRAW_PRELUDE_IPAD_QUEUE_2026-08-17.md`, `HUD_HIT_DIRECTION_STEERING_ARM_2026-08-17.md`, `HUD_CARGO_MISSION_PANELS_ARM_2026-08-17.md`, `HUD_RETICLE_DOCK_CAMERA_ARM_2026-08-18.md`, `HUD_QUICK_SECONDARY_FIRE_ARM_2026-08-18.md`, `HUD_PROGRESS_MINING_TAIL_ARM_2026-08-18.md`, `HUD_DRAW_STACK_LIFETIME_ARM_2026-08-18.md`, `HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md`, `HUD_MISSION_CARGO_LIFETIME_ARM_2026-08-19.md`, `HUD_PROGRESS_MINING_CONTROL_FLOW_ARM_2026-08-19.md`, `HUD_PRELUDE_HEALTH_BARS_ARM_2026-08-19.md`, `HUD_GAMMA_ROCKET_HIT_ENTRY_ARM_2026-08-19.md`, `HUD_CHALLENGE_ORBIT_MENU_ABI_ARM_2026-08-19.md`, `HUD_TOUCH_MENU_CARGO_ARM_2026-08-19.md`, `HUD_INIT_MENU_EVENT_ARM_2026-08-20.md`, `HUD_EVENT_QUEUE_ARM_2026-08-20.md`, `HUD_INIT_ARM_2026-08-20.md`, and `HUD_CARGO_MENU_TOUCH_ORBIT_ARM_2026-08-20.md`. | UCRT64 is green; ARM corpus is `201/204` with three unrelated known failures. The 48-function Hud set averages about `86.7%`, with 26 linked-exact and 19 byte-exact functions. Constructor is linked-exact at `100%`; destructor is `97.5%` linked-exact at `80/80` instructions; `drawPauseButton` is `92.7%` linked-exact; `Hud::firePressed` and `addToEventQueue` are byte-exact; `Hud::init` is `64.7%` at `1077/1021`; `checkIfQuickMenuIsEmpty` is `96.4%` at `42/41`; `Hud::draw` is `47.4%`; challenge score is `50.1%`; orbit information is `50.3%` at `225/216`; `touchedElement` is `94.1%` at `444/440`; `touchBegin` is `59.4%` at `65/63`; `drawMenu` is `63.1%` at `225/219`; `catchCargo` is `71.5%` at `470/453`; `drawEventQueue` is `55.6%`; `drawEventString` is `68.3%`; `updateQueue` is `95.3%`; `updateSecondaryWeaponString` is `93.0%` linked-exact; `hudEventMedal` is `84.1%` at `182/182` instructions; `hudEvent` is `66.1%` at `1088/1010`; `initHudMenu` is `27.3%` at `1245/1152`; touchEnd is `92.3%`; getAnalogX/Y are linked- and byte-exact. Large draw/event bodies, the remaining init coordinate lifetime shape and final destructor unwind tail remain. |
| `Hud` latest ARM snapshot | The main-action/target-context pass restores the native fresh mining predicate, explicit primary image selection, local Canvas reload boundary and signed mining-alpha mirror. See `HUD_MAIN_ACTION_TARGET_CONTEXT_ARM_2026-08-21.md`; this row supersedes older numeric snapshots in the historical Hud synopsis above. | `Hud::draw` is `48.1%` at `3039/3223`; `initHudMenu` is `35.0%` at `1188/1245`; challenge score is `49.0%`; orbit information is `56.3%` at `219/225`; event queue is `75.4%`; event string is `90.1%`. All 48 Hud functions average `88.5%`, with 26 linked-exact and 19 byte-exact. ARM remains `201/204` because of the same three unrelated `SolarSystem *` failures. |
| Hangar / `ModStation` | `HangarList`, `HangarWindow`, `ListItemWindow`, scene-23 assembly and station camera routes received a focused source-backed pass. The package replaces local placeholder globals with typed Status/GameText/Item routes, restores the 64-entry preview mesh tables and direct native camera tables, records the original `BuildResourceList` mesh entry for resource `6801`, and types the 24 native Hangar action slots plus Android row metrics at `Layout+0x238/+0x248/+0x24c/+0x250`. See `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md`. | Native UCRT64 build is a compile gate only. Exact preview camera/output shape, remaining platform layout branches, untyped market tails, and ARM byte matching remain separate work. |
| `MGame` touch routing | The `Hud::touchEnd` branch now covers pause, primary-fire/hacking/auto-turret combat actions, camera/maneuver release, quick-menu/orbit actions, and the paused autopilot-menu, StarMap, ordinary ChoiceWindow, `MGame+0xcf` selector, MenuTouchWindow, non-terminal dialogue, and failed dialogue cleanup routes. See `MGAME_HUD_ACTION_ROUTING_2026-07-20.md`, `MGAME_PAUSED_TOUCH_ROUTING_2026-07-20.md`, and `MGAME_ARM_MATCH_2026-07-20.md`. | Focused ARM verification is live: current `OnTouchEnd(int,int,void*)` is 3.6% fuzzy, not linked- or raw-byte-exact. `MGame+0xca`, boost, game-over, and successful terminal dialogue/cutscene remain separate work. |
| `Level` particle setup | The player-engine loop in `Level::initParticleSystems` creates `field_80` systems for `ParticleSet(29 + nozzle)`, records their handles in `field_a8`, and applies confirmed Android scale, position, color, and per-ship atlas UV data to the live `ParticleSettingsRef::cur` table. The companion ABI/runtime audit recovered the paired `cur`/`init` `48 x 0xa0` tables, direct manager/system consumers, baseline scaling/interpolation, and a native smoke test. See `LEVEL_PARTICLE_SYSTEMS_2026-07-21.md` and `PARTICLE_SETTINGS_REF_ABI_RUNTIME_2026-07-21.md`. | Native UCRT64 build and the isolated particle smoke target are green. Original particle-name payloads and full render-path/ARM byte matching remain separate work. |
| Mesh merging | `MeshMerger`, `SimpleMeshMerger`, and `LodMeshMerger` have recovered merge and transform paths. | Full ARM verification remains package-specific. |
| Weapons | `Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`, and related runtime helper work have focused evidence. | Constructors, vtables, and bodies remain under ARM audit. |
| Resource loading | AEM/AEI loader and resource-table relationships are documented from native call paths. | Format understanding is not a claim that all resource paths are finished. |

## Latest Hud Verification

The `HUD_EVENT_PRESENTATION_ARM_2026-08-20.md` and
`HUD_CHALLENGE_ORBIT_LIFETIME_ARM_2026-08-20.md` follow-ups are extended by
`HUD_INIT_MENU_SOURCE_SHAPE_ARM_2026-08-21.md` and supersede the older inline
Hud figures above. The 48-function Hud set now averages `88.5%`, with 26
linked-exact and 19 byte-exact functions. Current focused scores include
`drawEventQueue` 75.4%, `drawEventString` 90.1%, `drawChallengeModeScore`
49.0%, `drawOrbitInformation` 56.3%, and `initHudMenu` 35.0% at `1245/1188`
target/base instructions. The menu pass restores post-state cloak/jump action
insertion, the paired command action table, signed docking actions and native
menu-count reloads.

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
