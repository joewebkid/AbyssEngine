# Hud Runtime Prefix Migration

Date: 2026-08-16

## Scope

This package moves the first proven Android ARMv7 `Hud` region from the
fixed-width evidence model into the live C++ class. The migrated range is
`Hud+0x000..+0x10b`:

- state bytes at `+0x000..+0x003`;
- iPad fire images at `+0x004/+0x008`;
- packed iPad fire coordinates at `+0x00c/+0x00e`;
- steering/fire anchors at `+0x010/+0x014`;
- `menuButtons` at `+0x018`;
- twenty consecutive `AbyssEngine::String` objects at `+0x01c..+0x100`.

The live class now has ARM-only `sizeof(String)` and `offsetof` assertions for
this complete prefix. `HudArm32Layout` remains the independent full-object
evidence contract.

## Source Shape

Android `Hud::Hud` at `0x1603fc` constructs the first twenty Strings with one
loop from offset `28` through offset `256`, stepping by `12`. Android
`Hud::~Hud` destroys the same region in reverse with a loop.

The previous C++ class declared twenty independent members. Although their
content was usable, Clang emitted twenty separate constructor/destructor call
sites. The runtime class now uses `String strings_01c_100[20]`, reproducing the
array ownership and loop shape. The one known consumer of the final String now
uses `strings_01c_100[19]`.

Resources `0x4c6` and `0x6aa` are now created directly into the native prefix
fields. Their former copies in `HudInitImageSlots` were removed. The later
image-span migration has since removed that compact helper entirely; see
`HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`.

## ARM Result

| Function | Before | After | Instructions after |
| --- | ---: | ---: | ---: |
| `Hud::Hud` | `5.6%` | `92.7%` | `82/82` original/local |
| `Hud::~Hud` | `36.7%` | `73.3%` | `80/81` original/local |
| `Hud::init` | `9.2%` | `9.8%` | `1077/1120` original/local |
| `Hud::hudEvent` | `9.7%` | `9.7%` | `1088/885` original/local |

None of these functions are linked- or raw-byte-equal. The constructor now has
the exact instruction count; its remaining differences are the six later
String offsets:

| String | Current runtime offset | Android offset |
| --- | ---: | ---: |
| main event line | `+0x124` | `+0x1e0` |
| cargo event line | `+0x134` | `+0x1f4` |
| auxiliary event line | `+0x140` | `+0x200` |
| secondary event line | `+0x154` | `+0x228` |
| secondary weapon label | `+0x214` | `+0x3b4` |
| camera mode label | `+0x31c` | `+0x51c` |

These offsets are not being imitated with constructor-only scratch storage.
They require the real event, image, coordinate and timer regions to be migrated
in ownership order. The event/progress region, image span and secondary-label
String are now complete; see `HUD_RUNTIME_EVENT_PROGRESS_2026-08-17.md` and
`HUD_RUNTIME_IMAGE_SPAN_2026-08-17.md`.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus compiles `201/204` translation units; the same three unrelated
  `SolarSystem *` versus numeric system-index failures remain.
- `tools/lint_hacks.py` reports no byte-match definition hacks.
