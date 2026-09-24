# Project Status

Last reviewed: 2026-09-20 (selective fionera reconciliation; older snapshots retain their dates)

## Selective Upstream Reconciliation (2026-09-20)

The unchanged fionera `ef63e5a0` snapshot was rechecked against the dirty local
tree and only Android-confirmed gaps were transferred. `PlayerTurret` now has
the native `0x168` layout and reset/revive activation; `TargetFollowCamera`
keeps size `0x178` with its tail fields at `0x120/0x130/0x138/0x13c`;
`Camera` is `0x5c` with a single Matrix construction; `Gun::setOffset(Vector*)`
uses `100.0f` and its Array lifetime no longer calls a null placeholder.
UCRT64 builds and the ARM corpus remains `206` compiled objects with the same
three unrelated failures. `ListItemWindow::update` also retains the expanded
local card while improving strict/source shape from `71.7/93.6%` to
`84.8/96.9%`. Focused family results and exact boundaries are in
`../../docs/findings/gof2_fionera_selective_transfer_20260920_ru.md`.

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
| `MenuTouchWindow` | Draw/touch routing, load/save, settings, HUD layout, DLC/store, and language-list paths received focused source-backed passes. The main-state dispatcher covers the confirmed `+0x04` button ID table, cargo summary, Google Play/restore/quit controls, and native X/Y forwarding. The state/dialog follow-up restores the phone/iPad `state 3` options body, states `10/12/13/17`, the calibration-pending byte at `+0x176`, native challenge fade values, and the selected DLC/game context at `+0x234`. State `15` now has its Android DLC launch/acknowledgement, purchase, restore, selector, result/error and app-data protocol (`+0x3c/+0x3f/+0x40/+0x48`); state `16` follows its separate native `OnTouchEnd` fallthrough instead of reusing that modal flow. Constructor social controls live in the confirmed shared auxiliary array at `+0xc0`, separate from DLC selector slots at `+0xf8`. State `9` now respects the confirmed `WantedWindow -> MissionsWindow -> MenuTouchWindow` `int` close-return chain instead of closing after every touch. See `MENUTOUCH_MAIN_DISPATCH_FREECAM_ARM_2026-08-23.md`, `MENUTOUCH_STATE_DIALOG_OPTIONS_ARM_2026-08-23.md`, `MENUTOUCH_DLC_STORE_STATE_ARM_2026-08-23.md`, and `MISSIONSWINDOW_TOUCH_END_RETURN_ABI_ARM_2026-08-23.md`. Local source-backed coverage remains an estimate around 72%; exact String cleanup and whole-switch shape remain. | UCRT64 is green. Forced-inline `OnTouchEnd` is `8.7%` at `3001/2887` target/base instructions, not linked- or byte-exact; this replaces the previous `3.7%` at `3001/2675` checkpoint. The source body is closer in instruction span but still differs in CFG, temporary lifetime and frame shape. `isMakingScreenshot` remains `100%` linked- and byte-exact at `6/6`. |
| `Radar` | Whole enemy body, station/planet phases, gas/plasma and asteroid/dead-cargo flows, direct audio, constructor/labels and distance/current-lock String lifetime recovered against Android instructions with iOS cross-checks. Render returns are now verified `void`. See `RADAR_DRAW_SOURCE_SHAPE_2026-09-15.md`. | The unchanged 19-symbol set averages **99.0% source-shape**, from 96.0526316%; 13 linked-exact and 8 raw-byte-exact. Draw is **90.8% source / 41.7% strict**, not byte-matched. Constructor aliases are 98.1%, current-lock 96.6%, distance 99.1%, ellipse 98.3%. Native build passes; ARM current-source coverage remains 201/204. |
| `Hud` | Core-frame work recovered direct `Globals` routing, `PlayerEgo::level`, the `Hud::init` image/coordinate map, exact 20-entry event queue, four quick-menu construction modes, menu frame/cargo gauge, status/gamma rows, and linked gamma runtime. The complete `hudEvent` switch covers events `1..47`, Android GameText/sound routing, duplicate suppression and the `0x100019` important-event mask. Four staged runtime migrations now place every live field from `+0x000` through `+0x53b` at its Android ARM offset: Strings, event/touch state, 71 images, all packed coordinates, flash/hit timers, camera arrays and final ownership fields. The real class is compile-locked at `sizeof(Hud) == 0x53c`; the out-of-line `ArrayReleaseClasses<TouchButton *>` specialization is `100%` linked-exact. The pause button and right action cluster use the native low-byte mask, confirmed Image2D pairs, flash ordering, readiness timers and `Level+0x69` delayed-secondary lifecycle. The source-backed `Hud::draw` prelude gates the event queue, remaps persisted iPad control anchors, restores the original canvas color on both exits, and leaves pause/orbit/menu orchestration to `MGame::OnRender2D`. Its hit-direction follow-up restores the four 300 ms direction pulses, Radar-centred transforms, hacking-aware steering dimming, and distinct rocket/normal color exits. The mission-panel pass restores kill-counter, passenger, production, ordinary cargo and countdown dispatch. The reticle/dock/camera pass restores hacking controls, disabled tint, alien-orbit dock exceptions, time-extender flow, autopilot dock substitution, direct camera tables and the auto-turret/banner split. The quick/secondary/fire audit restores native pressed/idle branches, the direct Radar target predicate and the correct `fireForTutorial` main-fire state. The progress/mining tail separates the native dock-fill draw image from its geometry-metrics image and exposes the signed 64-bit `LevelScript+0x08` timer. The first whole-function lifetime pass now inlines the native iPad remap and both steering copies, restores the seven-object secondary-label chain, and matches the dock progress query/String order. The boost/protector follow-up restores the three native boost draw paths and selects the evidence-backed `-fstack-protector-strong`. The mission/cargo lifetime pass restores direct `Globals::status/Canvas` reloads, native production `getCargo` repetition and the eight temporary String slots. The progress/mining control-flow pass removes non-native singleton guards, restores the shared jump/cloak body, fresh mining predicates, single pulse-timer store and white color reset. The prelude/health-bar pass now caches post-remap iPad anchors, restores boost read lifetimes, uses native `(rate * 0.01f) * width` recurrence, and joins armor coordinates through the confirmed address pairs. The gamma/rocket/hit-entry pass restores the native sub/add gamma-coordinate recurrence, the uncollapsed secondary-flash branch, and offset-selected shield/armor hit images. The challenge/orbit/menu ABI pass restores the live challenge-score body, orbit color stride, event 27, exact secondary setter, `GameSettings` iPad anchors and the corrected `Hud+0x4cc..0x4f0` quick-menu/touch layout. The touch/menu/cargo pass restores native phone/iPad hitbox routing, quick-menu frame iteration, real cargo replacement markers, full-width event text Y and the confirmed `Hud+0x3c0` secondary-label position. The init/event pass restores native quick-menu allocation order, equipment/station String lifetimes, duplicate-event queue construction and exact medal text. Its source-shape follow-up removes the non-native event-line alias, confirms event 19 as `Hud+0x100`, restores branch-local queue-string allocation, and returns phone/iPad hit testing to native field-load lifetimes. The queue-family pass restores the integer `updateQueue` ABI, 2000/4000 ms lifecycle, source-order banner fade/slide and a byte-exact slot insertion scan. The dedicated `Hud::init` pass removes the non-native image descriptor stack table, restores all 75 static image call sites, native queue/key allocation order, ship capability queries, integer pause coordinates and the 32-byte layout copy. The cargo/menu/touch/orbit source-shape pass restores the typed `Status::replaceHash` hidden return, native cargo String expressions, draw-site coordinate reads, nonzero-element-first touch routing and orbit Canvas/font lifetimes. Generated `Hud::draw` size is now `3033/3223`, while stack reservation and the VFP save set match at `224` bytes and `d8-d11`. See `HUD_ARM32_ABI_LAYOUT_2026-08-16.md`, `HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`, `HUD_RUNTIME_TAIL_2026-08-17.md`, `ARRAY_TOUCHBUTTON_ARM_ABI_2026-08-17.md`, `HUD_PAUSE_BUTTON_ARM_2026-08-17.md`, `HUD_ACTION_CLUSTER_ARM_2026-08-17.md`, `HUD_DRAW_PRELUDE_IPAD_QUEUE_2026-08-17.md`, `HUD_HIT_DIRECTION_STEERING_ARM_2026-08-17.md`, `HUD_CARGO_MISSION_PANELS_ARM_2026-08-17.md`, `HUD_RETICLE_DOCK_CAMERA_ARM_2026-08-18.md`, `HUD_QUICK_SECONDARY_FIRE_ARM_2026-08-18.md`, `HUD_PROGRESS_MINING_TAIL_ARM_2026-08-18.md`, `HUD_DRAW_STACK_LIFETIME_ARM_2026-08-18.md`, `HUD_BOOST_STACK_PROTECTOR_ARM_2026-08-19.md`, `HUD_MISSION_CARGO_LIFETIME_ARM_2026-08-19.md`, `HUD_PROGRESS_MINING_CONTROL_FLOW_ARM_2026-08-19.md`, `HUD_PRELUDE_HEALTH_BARS_ARM_2026-08-19.md`, `HUD_GAMMA_ROCKET_HIT_ENTRY_ARM_2026-08-19.md`, `HUD_CHALLENGE_ORBIT_MENU_ABI_ARM_2026-08-19.md`, `HUD_TOUCH_MENU_CARGO_ARM_2026-08-19.md`, `HUD_INIT_MENU_EVENT_ARM_2026-08-20.md`, `HUD_EVENT_QUEUE_ARM_2026-08-20.md`, `HUD_INIT_ARM_2026-08-20.md`, and `HUD_CARGO_MENU_TOUCH_ORBIT_ARM_2026-08-20.md`. | UCRT64 is green; ARM corpus is `201/204` with three unrelated known failures. The 48-function Hud set averages about `86.7%`, with 26 linked-exact and 19 byte-exact functions. Constructor is linked-exact at `100%`; destructor is `97.5%` linked-exact at `80/80` instructions; `drawPauseButton` is `92.7%` linked-exact; `Hud::firePressed` and `addToEventQueue` are byte-exact; `Hud::init` is `64.7%` at `1077/1021`; `checkIfQuickMenuIsEmpty` is `96.4%` at `42/41`; `Hud::draw` is `47.4%`; challenge score is `50.1%`; orbit information is `50.3%` at `225/216`; `touchedElement` is `94.1%` at `444/440`; `touchBegin` is `59.4%` at `65/63`; `drawMenu` is `63.1%` at `225/219`; `catchCargo` is `71.5%` at `470/453`; `drawEventQueue` is `55.6%`; `drawEventString` is `68.3%`; `updateQueue` is `95.3%`; `updateSecondaryWeaponString` is `93.0%` linked-exact; `hudEventMedal` is `84.1%` at `182/182` instructions; `hudEvent` is `66.1%` at `1088/1010`; `initHudMenu` is `27.3%` at `1245/1152`; touchEnd is `92.3%`; getAnalogX/Y are linked- and byte-exact. Large draw/event bodies, the remaining init coordinate lifetime shape and final destructor unwind tail remain. |
| `Hud` latest ARM snapshot | The main-action/target-context and String hidden-return passes restore the native fresh mining predicate, local Canvas boundaries, signed mining-alpha mirror, secondary temporary cleanup graph, direct `Level+0x69` route, dock label stack-slot reuse and explicit progress-label bool constructors. The image-metric/mission-overlay pass confirms the existing kill/passenger/production/cargo String cleanup graph, corrects `PaintCanvas::GetImage2DWidth/Height` to the native signed `int` return contract and restores the ordered-greater volatile-force branch. The quick-menu resource audit corrects mode 1 from the orbit header `0x4f4` to its native `0x4f6`, proves the complete `0x4f3..0x4f6` atlas chain and closes the direct `drawMenu` consumer. Its layout follow-up restores the asymmetric native recurrence: `Layout+0x1dc` remains cached while `Layout+0x30` is reloaded at each row transition, and the equipment String graph again belongs directly to mode 1. The frame-lifetime follow-up restores the native 160-byte dynamically aligned `initHudMenu` frame, stages the iPad fire anchor before platform-offset selection and removes non-native final-loop `Vector` lifetimes. The presentation-shape audit restores the native challenge-row Y recurrence and unsigned bonus centering, reconfirms all five orbit String lifetimes and records the remaining event-line load-order boundary. The dedicated init call-frame pass removes non-native metric lambdas, restores the four-word layout transfer, fresh screen-global reads and the exact steer/copy/fire anchor lifetime. See `HUD_MAIN_ACTION_TARGET_CONTEXT_ARM_2026-08-21.md`, `HUD_STRING_HIDDEN_RETURN_CLEANUP_ARM_2026-08-21.md`, `HUD_IMAGE_METRIC_MISSION_OVERLAY_ARM_2026-08-22.md`, `HUD_INIT_MENU_RESOURCE_CHAIN_ARM_2026-08-22.md`, `HUD_INIT_MENU_LAYOUT_RECURRENCE_ARM_2026-08-22.md`, `HUD_INIT_MENU_FRAME_LIFETIME_ARM_2026-08-23.md`, `HUD_CHALLENGE_ORBIT_PRESENTATION_SHAPE_ARM_2026-08-23.md` and `HUD_INIT_COORDINATE_CALL_FRAME_ARM_2026-08-23.md`; this row supersedes older numeric snapshots in the historical Hud synopsis above. | `Hud::draw` is `50.0%` at `3047/3223`; `initHudMenu` is `44.3%` at `1199/1245`; `drawMenu` is `84.3%` at `221/225`; `Hud::init` is `81.1%` at `1077/1068` with an exact 188-byte stack reservation; challenge score is `49.3%` at `340/350`; orbit information is `56.3%` at `219/225`; event queue is `75.4%`; event string is `90.1%` at `64/67`. All 48 Hud functions average `89.6%`, with 26 linked-exact and 19 byte-exact. `GetImage2DWidth/Height` remain linked- and byte-exact at `9/9` each. ARM remains `201/204` because of the same three unrelated `SolarSystem *` failures. |
| Hangar / `ModStation` | `HangarList`, `HangarWindow`, `ListItemWindow`, scene-23 assembly and station camera routes received a focused source-backed pass. The package replaces local placeholder globals with typed Status/GameText/Item routes, restores the 64-entry preview mesh tables and direct native camera tables, records the original `BuildResourceList` mesh entry for resource `6801`, and types the 24 native Hangar action slots plus Android row metrics at `Layout+0x238/+0x248/+0x24c/+0x250`. The station routing audit now proves the Android `+0x60..+0x6a` ownership map for radio/main/DLC/Choice, missions/lounge/hangar/StarMap, and status/dialogue/medal modal paths; all touch, update and draw consumers use that same mapping. `StatusWindow`, `SpaceLounge`, and now `HangarWindow` expose their source-backed `int OnTouchEnd(x, y)` ABI directly. Hangar returns nonzero from the confirmed top-level `Layout -> readyToClose()` path and the temporary station-credit close path at `ModStation+0x18`; ordinary detail, tab and internal modal paths retain ownership and return zero. `HangarWindow::OnTouchBegin` now also has its Android `int` ABI and source order for modal credit buttons, list selection, remote-blueprint confirmation and auto-equip routing. The ARM32 class map is now locked at the Android `operator new(0x134)` allocation, including scroll/inertia and list-action offsets; `OnTouchMove` follows the Android modal/detail/list control flow and drag transition. See `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md`, `MODSTATION_SUBWINDOW_ROUTING_ARM_2026-08-23.md`, `STATION_SUBWINDOW_TOUCH_RETURN_ABI_ARM_2026-08-23.md`, `HANGAR_TOUCH_RETURN_ABI_ARM_2026-08-23.md`, `HANGAR_TOUCH_BEGIN_SOURCE_SHAPE_ARM_2026-08-24.md`, `HANGAR_ARM32_LAYOUT_TOUCH_MOVE_2026-08-24.md`, `HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md`, and `HANGAR_CHOICE_TAIL_SOURCE_SHAPE_2026-08-24.md`. | UCRT64 is green. Focused ARM is `201/204` due to three unrelated `SolarSystem *` errors; `OnRender3D` is 82.1% (`70/64`), `HangarWindow::OnTouchBegin` is 27.2% (`448/390`) and `HangarWindow::OnTouchMove` rises from 11.6% to 78.1% (`141/138`). The large mixed-recovery `ModStation::OnTouchEnd` is 6.4% (`2022/906`), `HangarWindow::OnTouchEnd` is 11.8% (`2154/1968`), `SpaceLounge::OnTouchEnd` is 6.0% (`2713/333`) and `StatusWindow::OnTouchEnd` is 19.2% (`306/236`), with no linked/byte-exact result. Exact preview camera/output shape, remaining String/stack shape in Hangar touch bodies, platform layout branches, the full Hangar behavior body, and ARM byte matching remain separate work. |
| `MGame` touch routing | The `Hud::touchEnd` branch covers pause, primary-fire/hacking/auto-turret/boost actions, camera/maneuver release, quick-menu/orbit actions, and the paused autopilot-menu, StarMap, ordinary ChoiceWindow, `MGame+0xcf`, cargo-conversion `MGame+0xca`, terminal/challenge `MGame+0x1e4`, MenuTouchWindow and dialogue routes. Successful dialogue includes rewards, route/objective cleanup, mission-183 follow-up and all confirmed campaign station/module checkpoints. The shared MenuTouch close tail now restores the three native engine-sound routes, touch reset, light reinit, packed `Globals::options+0x28` particle thresholds, Skip-button producer/consumer flow, campaign `0/1/154/157/158` transitions and cinematic/free-camera cleanup. The related `OnInitialize` fragment keeps `options+0x0f` engine-effect routing separate from the native `>=0.25`/`>0.7` particle-quality thresholds. The free-camera ARM audit proves that its two coordinate arguments are dead: native `freeCamTouchEnd` consumes only `this`, touch ID, and stored deltas, so the non-reloaded `r1/r2` values are not missing logic. See `MGAME_HUD_ACTION_ROUTING_2026-07-20.md`, `MGAME_PAUSED_TOUCH_ROUTING_2026-07-20.md`, `MGAME_ARM_MATCH_2026-07-20.md`, `MGAME_TOUCH_SUCCESS_CARGO_TRANSITIONS_ARM_2026-08-23.md`, `MGAME_MENU_CLOSE_CUTSCENE_PARTICLE_ARM_2026-08-23.md`, and `MENUTOUCH_MAIN_DISPATCH_FREECAM_ARM_2026-08-23.md`. | Focused ARM verification is `8.5%` at `2863/2358` target/base instructions, versus the original 4.3%/1426 baseline and the preceding 9.2%/2169 checkpoint. It is neither linked- nor raw-byte-exact. `freeCamTouchEnd` is `20.0%` at `56/64`; `OnInitialize` is `14.1%` at `771/182`. Whole-function block order/local lifetime remains. |
| `Level` particle setup | The player-engine loop in `Level::initParticleSystems` creates `field_80` systems for `ParticleSet(29 + nozzle)`, records their handles in `field_a8`, and applies confirmed Android scale, position, color, and per-ship atlas UV data to the live `ParticleSettingsRef::cur` table. The companion ABI/runtime audit recovered the paired `cur`/`init` `48 x 0xa0` tables, direct manager/system consumers, baseline scaling/interpolation, and a native smoke test. See `LEVEL_PARTICLE_SYSTEMS_2026-07-21.md` and `PARTICLE_SETTINGS_REF_ABI_RUNTIME_2026-07-21.md`. | Native UCRT64 build and the isolated particle smoke target are green. Original particle-name payloads and full render-path/ARM byte matching remain separate work. |
| Mesh merging | `MeshMerger`, `SimpleMeshMerger`, and `LodMeshMerger` have recovered merge and transform paths. | Full ARM verification remains package-specific. |
| Weapons | `Gun`, `AbstractGun`, `SpriteGun`, `BeamGun`, and related runtime helper work have focused evidence. | Constructors, vtables, and bodies remain under ARM audit. |
| Resource loading | AEM mesh creation/clone paths and the full AEI `ResourceTexture -> ImageCreateFromFile/ImageCreateRegionFromFile -> TextureCreateFromFileIntern` path are source-backed. The AEI passes restore the real `AEimage\0` parser, typed `Image2D`/`AELoadedTexture` ownership, exact atlas triangles, format-specific mip uploads, filters, cubemap faces, cache writes and GL/error accounting. The appended font-table path restores exact UTF-16 set selection, glyph rectangles and per-glyph mesh ownership; 21 real atlases and 23,370 glyph records validate through their final byte. Font runtime covers native advance, direction, baseline/clipping, shader batches, fixed-function matrices and `<c:RRGGBBAA>` segmented color lifetime. See `AEM_AEI_NATIVE_LOADER_SPEC.md`, `AEI_TEXTURE_UPLOAD_ARM_2026-08-27.md`, `AEI_IMAGE_PARSER_ARM_2026-08-27.md`, `AEI_FONT_ATLAS_PARSER_ARM_2026-08-27.md`, `IMAGE_FONT_DRAW_RUNTIME_ARM_2026-08-27.md`, and `DRAW_STRING_COLOR_TAGS_ARM_2026-08-27.md`. | `ImageCreateRegionFromFile` is `81.0%` at `216/221`; `ImageCreateFromFile` is `34.0%` at `295/317`; `ImageCreateFontFromFile` is `57.6%` at `314/314` and `ImageFontRelease` is `88.3%` at `40/37`. Substring width is byte-exact, ordinary width is `97.8%`, `SetWorldViewMatrix` is linked-exact, `FontCreate` is `50.3%`, full draw is `17.1%` at `474/394`, `DrawStringColor` is `93.3%` at `136/132`, and `SplitTags` is `76.3%` at `184/183`. Real GLES payload/reload coverage, multi-line layout, and full draw byte matching remain. |

The touch/key lifecycle follow-up in
`HUD_TOUCH_KEY_LIFECYCLE_ARM_2026-08-23.md` supersedes the Hud aggregate in the
table above: `touchBegin`, `touchMove` and `touchEnd` are now `87.0%`, `97.1%`
and `97.8%`; `closeHudMenu` is `100%` linked-exact. All 48 Hud functions average
`90.7%`, with 27 linked-exact and 19 byte-exact.

The event dispatch/String lifetime follow-up in
`HUD_EVENT_DISPATCH_STRING_LIFETIME_ARM_2026-08-23.md` further supersedes that
aggregate: `hudEvent` is now `76.3%` and `hudEventMedal` is `84.9%`. All 48 Hud
functions average `91.0%`; linked-exact and byte-exact counts remain 27 and 19.

The health/hit Canvas follow-up in
`HUD_HEALTH_HIT_CANVAS_ARM_2026-08-23.md` raises `Hud::draw` from `50.0%` to
`51.4%` at `3223/3025` target/base instructions. It removes non-native gamma
guards and restores native Canvas reload boundaries across the health,
rocket-control, directional-hit and steering cluster. The 48-function average
and exact counts remain `91.0%`, 27 linked-exact and 19 byte-exact.

## Latest Hud Verification

The `HUD_EVENT_PRESENTATION_ARM_2026-08-20.md` and
`HUD_CHALLENGE_ORBIT_LIFETIME_ARM_2026-08-20.md` follow-ups are extended by
`HUD_INIT_MENU_SOURCE_SHAPE_ARM_2026-08-21.md`, followed by
`HUD_INIT_MENU_RESOURCE_CHAIN_ARM_2026-08-22.md` and
`HUD_INIT_MENU_LAYOUT_RECURRENCE_ARM_2026-08-22.md`, then
`HUD_INIT_MENU_FRAME_LIFETIME_ARM_2026-08-23.md`,
`HUD_CHALLENGE_ORBIT_PRESENTATION_SHAPE_ARM_2026-08-23.md`, and
`HUD_TOUCH_KEY_LIFECYCLE_ARM_2026-08-23.md`, followed by
`HUD_EVENT_DISPATCH_STRING_LIFETIME_ARM_2026-08-23.md` and
`HUD_HEALTH_HIT_CANVAS_ARM_2026-08-23.md`, and supersede the older inline Hud
figures above. The 48-function Hud set now averages `91.0%`, with 27
linked-exact and 19 byte-exact functions. `Hud::draw` is now `51.4%` at
`3223/3025` target/base instructions. Other current focused scores include
`drawEventQueue` 75.4%, `drawEventString` 90.1%, `drawChallengeModeScore`
49.3%, `drawOrbitInformation` 56.3%, and `initHudMenu` 44.3% at `1245/1199`
target/base instructions. The menu pass restores post-state cloak/jump action
insertion, the paired command action table, signed docking actions and native
menu-count reloads. The resource follow-up corrects the mode-1 header to
`0x4f6` and records all four header atlas entries; `drawMenu` currently reaches
`84.3%` at `225/221` target/base instructions. The layout recurrence follow-up
restores per-row `Layout+0x30` loads while retaining only `Layout+0x1dc`. The
frame-lifetime pass then restores the Android 160-byte dynamic alignment,
iPad anchor operand order and post-translate button reloads. The remaining
mode-1/mode-2 shared String/action area is still allocated at `sp+0x70` rather
than the native `sp+0x40`, so no byte-match is claimed.

The touch/key follow-up restores the original moved-touch Y coordinate and the
native key-slot cleanup lifecycle. Its focused scores are `87.0%` for
`touchBegin`, `97.1%` for `touchMove`, `97.8%` for `touchEnd`, and linked-exact
`100%` for `closeHudMenu`.

The event dispatch follow-up restores the complete seven-String cargo-loss
expressions for events 30 and 47, types the packed dock-transfer state and
places medal percentage clamping inside the native String chain. `hudEvent`
reaches `76.3%` and `hudEventMedal` reaches `84.9%`.

The 2026-08-31 whole-family audit in `HUD_ARM_SOURCE_SHAPE_99_2026-08-31.md`
supersedes every numeric Hud snapshot above. Across all 48 native Hud symbols,
the allocation-insensitive source-shape average is now `99.24%`; the independent
family averages are `98.75%` instruction-shape inventory, `99.35%` opcode
inventory, `99.21%` ordered control flow and `99.65%` instruction-count
agreement. The strict ordered fuzzy average remains `95.61%`: 36 functions are
strict-fuzzy `100%`, 32 are linked-exact and 20 are raw byte-exact. This is a
verified source-shape milestone, not a claim that `Hud::draw`, `initHudMenu`,
`init`, challenge/orbit presentation, or the whole class are byte-matched.

## Latest Hangar Verification

The `HANGAR_TOUCH_END_ECONOMY_SHIP_SWAP_2026-08-24.md` follow-up extends the
previous layout and input passes with the exact Android social-credit reward
table, blueprint cancellation route, and the two-stage ship-swap/DLC/trade-in
flow. The focus run is still `201/204` because of three unrelated
`SolarSystem *` compile errors. `HangarWindow::OnTouchEnd` rises from `6.0%`
at `2154/1487` target/base instructions to `6.4%` at `2154/1776`.
`OnTouchBegin`, `OnTouchMove`, and `update` remain respectively `27.2%`,
`78.1%`, and `48.3%`. No Hangar method is claimed linked- or byte-exact.

The following `HANGAR_TOUCH_END_LIST_DIALOG_SOURCE_SHAPE_2026-08-24.md` pass
restores Android's continued list-action walk and scoped help-text String
branches. `HangarWindow::OnTouchEnd` rises to `11.2%` at `2154/1871`
target/base instructions. The shared modal application-module close flag is
now typed as `ModStation+0x18`, `closeHangarAfterCredits`. The
`HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md` correction proves its
station-launched paid-credit close route also returns nonzero to the owner;
the focused score stays `11.2%` at `2154/1879`.

`HANGAR_CHOICE_TAIL_SOURCE_SHAPE_2026-08-24.md` then restores the direct
Android `LABEL_117` convergence for replace-equipment, credit and sell-ship
dialogs. `HangarWindow::OnTouchEnd` reaches `11.8%` at `2154/1968`; no
linked- or raw-byte-exact result is claimed.

`HANGAR_SET_SELL_MODE_STORE_BLUEPRINT_2026-08-24.md` replaces the remaining
shim-based `HangarWindow::setSellMode` Store/Blueprint body with the Android
inventory rebuild, fabrication, pending-product, route-warning and
auto-equip flows. It also confirms that the second ship-swap cancellation and
both ship-purchase error notices clear the pending-swap word at `+0x90`.
Focused ARM validation measures `setSellMode` at `13.5%` (`708/643`) and
keeps `OnTouchEnd` at `11.8%` (`2154/1973`); both remain source-backed, not
linked- or byte-exact.

## Latest MGame Verification

The 2026-09-23 MGame gate in
`MGAME_INIT_STARMAP_SOURCE_SHAPE_2026-09-21.md` compares all 45 native MGame
symbols at `99.04%` average source-shape and `90.81%` strict fuzzy. Sixteen
functions are linked-exact and nine are byte-exact. `OnTouchEnd` and
`OnUpdate` reach `94.9%` and `96.7%` source-shape, but remain only `38.3%`
and `58.3%` strict; no byte-match is claimed for either large body.

The matching full-corpus run builds all 209 ARM objects without failures and
compares 4844 functions at `88.74%` source-shape / `75.50%` strict, with 2100
linked-exact and 975 byte-exact functions. UCRT64 `libgof2.a` also links.

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
