# Project Status

Last reviewed: 2026-07-20

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

## Notable Packages

| Package | Current understanding | ARM status |
| --- | --- | --- |
| `MenuTouchWindow` | Draw/touch routing, load/save, settings, HUD layout, DLC/store, and language-list paths received focused source-backed passes. Local source-backed coverage is estimated around 65-70%; constructor and exact-shape work remains. | Recent body work is native-build checked; no whole-class byte-match claim. |
| `Radar` | Constructor/setup, target classification, lock/HUD behavior, asteroid/dead-cargo branches, and selected audio/scanner behavior received focused recovery. | `Radar::draw` remains a large source-shape and byte-match task. |
| `Hud` | Core-frame pass recovered direct `Globals` routing, `PlayerEgo::level` use in `draw`, the source-backed `Hud::init` image map and coordinate setup, exact 20-entry event-queue capacity, iPad control-anchor routing, all four `initHudMenu` button-construction modes, and the matching `drawMenu` frame/cargo-gauge path. It also covers event-queue rendering, orbit information, and secondary-weapon text. `hudAction` is now confirmed as the original Android no-op, rather than an unfinished dispatcher. Unnamed source slots remain offset-labelled rather than semantically guessed. See `HUD_CORE_FRAME_2026-07-20.md`. | Native UCRT64 build is green. This is not a full `Hud::draw` recovery, a claim that all image roles are named, or an ARM byte-match claim. |
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
