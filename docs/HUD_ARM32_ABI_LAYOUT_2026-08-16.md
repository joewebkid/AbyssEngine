# Hud Android ARM32 ABI Layout

Date: 2026-08-16

## Scope

This package establishes a compile-checked, fixed-width evidence model for the
Android ARMv7 `Hud` object. It covers the constructor string skeleton, event
queue and progress state, image/coordinate spans, runtime timers, camera-mode
tail and the final allocation size.

The contract lives in `src/game/ui/HudArm32Layout.h`. It uses 32-bit surrogate
handles instead of host pointers, so every offset is checked by `static_assert`
on both the 32-bit matching compiler and the 64-bit UCRT compiler.

## Primary Evidence

- Android `Hud::Hud` at `0x1603fc` constructs 20 consecutive 12-byte Strings
  from `+0x01c` through `+0x100`, followed by Strings at `+0x1e0`, `+0x1f4`,
  `+0x200`, `+0x228`, `+0x3b4` and `+0x51c`.
- Android `Hud::~Hud` destroys the same seven String regions in reverse order.
- Android `Hud::init`, `hudEvent`, `catchCargo`, `draw`, `drawEventQueue`,
  `setTimeExtender`, `playerHit` and the small accessors independently expose
  the event, timer and flag offsets.
- Android `MGame` allocates `0x53c` bytes before calling `Hud::Hud`, proving the
  complete object size rather than only its highest observed member.

The Android IDA and Ghidra dumps were used for the offset reads. iOS remains a
behavioral cross-check, not the authority for this Android ABI contract.

## Locked Layout

| Range / offset | Confirmed role |
| --- | --- |
| `+0x004..+0x017` | iPad fire images, two 16-bit fire coordinates, steer/fire anchors. |
| `+0x018` | `Array<TouchButton *> *menuButtons`. |
| `+0x01c..+0x10b` | 20 consecutive 12-byte `AbyssEngine::String` objects. |
| `+0x15c` | Init-cleared Image2D handle; exact consumer remains unnamed. |
| `+0x160/+0x164` | Event origin X/Y. |
| `+0x1c4` | Faction-logo Image2D handle. |
| `+0x1d8/+0x1de` | Event scroll tick and active byte. |
| `+0x1e0/+0x1ec` | Main event String and letterbox byte. |
| `+0x1f4/+0x200` | Cargo/auxiliary event Strings. |
| `+0x218` | Secondary-label centered X coordinate. |
| `+0x21d..+0x221` | Cloak, boost, shield, armor-regen and autofire availability bytes. |
| `+0x228` | Additional event String. |
| `+0x235/+0x238/+0x244` | Cargo-full flag, quick-menu mode and shield-hit flash. |
| `+0x258/+0x25c` | Current secondary item and equipment-array pointers. |
| `+0x264..+0x270` | Event queue pointer, timer, dirty byte and paused state. |
| `+0x274..+0x27f` | Jump-map, jump/cloak, docking-transfer flags and fuel value. |
| `+0x280..+0x293` | Quick-menu/touch state, key-array and element-bit pointers. |
| `+0x298..+0x3b3` | 71 native Image2D handles. |
| `+0x3b4` | Secondary-weapon label String. |
| `+0x3c4..+0x3dc` | Quick-menu frame and row geometry. |
| `+0x3e0..+0x463` | 66 packed 16-bit HUD coordinates. |
| `+0x464/+0x468/+0x46c` | Charge, docking-transfer and hit-flash timers. |
| `+0x474/+0x476` | Boost-ready and cloak-ready latch bytes. |
| `+0x484/+0x488` | Boost notification remaining/pulse values. |
| `+0x498/+0x49c` | Cloak notification remaining/pulse values. |
| `+0x4a0/+0x4a5` | Autofire-enabled and tutorial-fire bytes. |
| `+0x4bc/+0x4c0/+0x4c4` | Time-extender timer/duration and mining-hint pulse. |
| `+0x4c8` | Message-active byte. |
| `+0x4d4..+0x4f0` | Touch/layout integers; event margins are `+0x4e8` and `+0x4f0`, separated by an independent dword at `+0x4ec`. |
| `+0x4f4..+0x510` | Four idle and four pressed camera-mode Image2D handles. |
| `+0x514/+0x518/+0x51c` | Previous camera mode, label timer and camera label String. |
| `+0x528/+0x52c` | Hacking-active byte and cargo aggregate count. |
| `+0x530/+0x534/+0x538` | `uintArray`, digit-sprite and multiplier-image handles. |
| `sizeof = 0x53c` | Allocation size observed in Android `MGame`. |

Unresolved bytes remain explicitly named `unknown_*`; no semantics are assigned
from adjacency alone.

`HudArm32ImageOffset` additionally locks the confirmed consumers inside the
71-handle image span. This includes the quick-menu frame at
`+0x298/+0x29c/+0x2a0`, shield/armor/gamma rows through `+0x2d4`, control
cluster through `+0x350`, event/secondary/header images at
`+0x354/+0x358/+0x35c`, hit indicators at `+0x360..+0x36c`, and the
fuel/progress/transfer group at `+0x370..+0x38c`.

## Runtime Migration Progress

The first three live-runtime migrations are complete. The prefix, 20-String
array, event Strings, queue, touch/progress state, all 71 Image2D handles and
the secondary-label String now occupy their Android offsets through `+0x3bf`.
The coordinate and timer/camera regions remain compact:

| Field | Current ARM class | Android object |
| --- | ---: | ---: |
| `menuButtons` | `+0x018` | `+0x018` |
| first/final array String | `+0x01c/+0x100` | `+0x01c/+0x100` |
| main event String | `+0x1e0` | `+0x1e0` |
| `eventQueue` | `+0x264` | `+0x264` |
| image-span start | `+0x298` | `+0x298` |
| image-span end | `+0x3b0` | `+0x3b0` |
| secondary label String | `+0x3b4` | `+0x3b4` |
| charge fade timer | `+0x458` | `+0x464` |
| event margins | `+0x4a8/+0x4ac` | `+0x4e8/+0x4f0` |
| camera label String | `+0x4b8` | `+0x51c` |
| object size | `0x50c` | `0x53c` |

The three migrations raise the constructor from `5.6%` to `98.8%` and the
destructor from `36.7%` to `90.7%`. The constructor now differs at only the
camera-mode String offset. Later source-backed functions still encode compact
coordinate and tail offsets until those regions are migrated. See
`HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md` for the resource map and focused ARM
results.

## Boundary And Migration Order

`HudArm32Layout` is an evidence contract, not a second runtime `Hud` and not a
claim that the current class is ABI-correct. The former compact
`HudInitImageSlots` storage has now been removed. A smaller temporary helper
holds only the eight camera images until their native tail is migrated.

The safe runtime migration is deliberately split into four steps:

1. **Complete:** move the prefix, `menuButtons` and 20-String constructor array.
2. **Complete:** move event queue, touch and progress state through `+0x294`.
3. **Complete:** replace compact image storage with the native
   `+0x298..+0x3b3` slots and move the secondary-label String to `+0x3b4`.
4. Restore the packed 16-bit coordinate holes, move the timers/camera tail,
   then lock the real 32-bit `Hud` with `sizeof(Hud) == 0x53c` and member
   `offsetof` assertions.

Doing this in stages keeps native ownership and destructor behavior testable;
casting the current compact object to the larger evidence layout would be an
out-of-bounds runtime bug and is explicitly rejected.

## Validation

- UCRT64 native build: `libgof2.a` links successfully and compiles every ABI
  assertion.
- ARM corpus: `201` translation units compile; the same `3` unrelated units
  fail on the known `SolarSystem *` versus integer system-index mismatch.
- After the image-span migration, constructor similarity is `98.8%`
  (`82/82` original/local instructions), destructor `90.7%` (`80/81`),
  `Hud::init` `9.8%` (`1077/1118`) and `Hud::hudEvent` `9.7%` (`1088/885`).
  Constructor/destructor are not byte-equal. Four small field accessors are
  linked- and raw-byte-exact; the wider Hud report has nine linked-exact and
  eight raw-byte-exact functions. See `HUD_RUNTIME_PREFIX_2026-08-16.md`,
  `HUD_RUNTIME_EVENT_PROGRESS_2026-08-17.md` and
  `HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`.
