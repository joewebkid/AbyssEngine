# FMOD playback recovery (ARM32)

Date: 2026-11-06

## Original-binary evidence

The original APK's `FModSound::play(int, Vector *, Vector *, float)`
starts at `0x932f4`. It rejects a missing project, an event ID beyond
`0x8f4`, or a missing event system. An empty event slot triggers
`EventSystem::getEventBySystemID(id, 4, ...)` at `0x9334e`, where
mode four is `FMOD_EVENT_INFOONLY`. A nonempty slot is used directly.
The event receives the default or down pitch, followed by a nonzero
requested pitch (`0x9337a`/`0x93390`).

`Event::getCategory` at `0x93398` supplies an `EventCategory *`. On
failure, an existing slot is released through its parent group's
`freeEventData(event, false)` and cleared (`0x933aa`–`0x933c4`). On
success, virtual `EventCategory::getInfo` at `0x933d4` supplies the
category index. Index zero records the music event ID; indices above
two return. The code checks `categoryEnabled[index + 1]`, caches
optional position and velocity vectors, and obtains the real event
with `getEventBySystemID(id, 0, ...)` at `0x93482` if the slot was
empty. Mode zero is `FMOD_EVENT_DEFAULT`.

When either vector was supplied, the original calls
`Event::set3DAttributes(position, velocity, nullptr)` at `0x934bc`.
It then reads the event's `purge` property at `0x934d4`. If the
property call succeeds and the value equals one, it records the ID in
the first free `fxSlots` entry. Finally it starts the event at
`0x934fc`.

## Recovery and verification

The previous source omitted both lookup modes, interpreted the
`getCategory` result as a category index, indexed category flags by
event ID, and tested `currentMusicEvent` instead of the `purge`
property. The source now follows the original call and data flow.
Two unused undefined helper declarations were removed. The
authorized ARM64 silent build returns without playback.

- Focused ARMv7 build: **211/211** translation units. `play`
  source-shape **89.7% -> 91.6%**; strict fuzzy **58.4% -> 51.0%**;
  still non-exact. `getEventPauseLength` also shifts from **98.0% to
  99.0%** source-shape and **60.8% to 60.0%** strict fuzzy; its
  source was not edited.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved symbol names: **187 -> 185**, removing the
  undefined event-lookup and 3D-attribute helpers. The corresponding
  typed FMOD methods were confirmed in the original ARM32
  `libfmodevent.so` during the preceding event-lookup pass.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, **96.84%** mean source-shape, **86.94%** strict fuzzy,
  **2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and **151** extra symbols. Both exact
  membership sets match the preceding result. A separately emitted
  static `FMOD_Event_getProperty` wrapper symbol is no longer present.

These checks validate source structure and compile/link behavior.
They do not prove live playback or a full ARM32 shared-library link.
Next inspect the remaining FMOD music and category methods.
