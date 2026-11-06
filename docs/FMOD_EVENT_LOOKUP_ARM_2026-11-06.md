# FMOD event lookup and 3D update (ARM32)

Date: 2026-11-06

## Original-binary evidence

The original APK's
`FModSound::updateEvent3DAttributes(FMOD::Event *, int, Vector *,
Vector *, bool)` starts at `0x935a8`. A non-null event first calls
`Event::getCategory` at `0x935dc`. Result `0x24` is
`FMOD_ERR_INVALID_HANDLE` in the bundled FMOD 4.44 headers and
enters a fresh lookup. Other nonzero results return null. A valid
existing event obtains its category index through virtual
`EventCategory::getInfo` at `0x935f2`, then checks
`categoryEnabled[index + 1]`. It updates cached position/velocity and
calls `Event::set3DAttributes` at `0x9372a`. On a successful update
with restart requested, it reads state and, if bit `0x8` is set,
stops with `true` before starting again (`0x93748`/`0x9374e`).

The fresh path calls `EventSystem::getEventBySystemID(id, 0x4, ...)`
at `0x9364c`, where mode `0x4` is `FMOD_EVENT_INFOONLY`. It obtains
the returned event's category and its actual category index through
`EventCategory::getInfo` at `0x93666`, checks the corresponding flag,
and updates cached 3D attributes. After `set3DAttributes` succeeds,
it calls `getEventBySystemID(id, 0, ...)` at `0x937d6`, starts that
second event at `0x937e2`, and returns it. FMOD mode `0` is
`FMOD_EVENT_DEFAULT`.

## Source recovery and verification

The previous source called an undefined three-argument lookup helper
that omitted the mode, treated the `getCategory` result as a category
index, indexed flags by event ID, and returned the information-only
event even when the second lookup failed. The source now uses the
typed FMOD methods and follows the two lookup modes and return paths
above. The two new ARM32 method imports are both exported by the
original `libfmodevent.so`. The already authorized ARM64 silent path
returns the input event without contacting FMOD.

- Focused ARMv7 build: **211/211** translation units. Target
  source-shape **79.7% -> 87.0%**, strict fuzzy **29.3% -> 43.8%**;
  still non-exact. The other 45 `FModSound` functions retain their
  metrics.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved names: **185 -> 187**, reflecting two new
  real FMOD method imports present in the original ARM32 library.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, **96.84%** mean source-shape, **86.94%** strict fuzzy,
  **2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 152 extra symbols. The linked-exact
  and byte-exact membership sets are unchanged.

This is static evidence for the recovered call sequence. Live event
playback and a complete ARM32 shared-library link remain unverified.
Next inspect `FModSound::play`, which still uses an undefined lookup
helper and has incorrect category handling.
