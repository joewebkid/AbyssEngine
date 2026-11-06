# FMOD listener update and event cleanup (ARM32)

Date: 2026-11-06

## Original-binary evidence

The original APK's `FModSound::updateAll(...)` starts at `0x92ff0`.
At `0x9315c` it calls
`FMOD::EventSystem::set3DListenerAttributes(int, const FMOD_VECTOR *,
const FMOD_VECTOR *, const FMOD_VECTOR *, const FMOD_VECTOR *)`. The
argument registers and stack slots contain listener index zero,
position, up, velocity, and forward, in that order. It then calls
`FMOD::EventSystem::update()` at `0x93166`. The listener vector
members are at offsets `0x2424`, `0x2430`, `0x2428`, and `0x242c`
respectively. `AEMath::Vector` and `FMOD_VECTOR` both contain three
consecutive `float` fields.

When a tracked sound has finished, `updateAll` obtains its parent
group and invokes vtable slot `+8` at `0x931bc`, corresponding to
`FMOD::EventGroup::freeEventData(event, false)`. `FModSound::play`
uses the same virtual call at `0x933c2` when releasing an existing
slot. The previous source called an undefined free-event helper at
both sites. It also used an undefined listener helper with a missing
forward argument and cast the FMOD system pointer to 32-bit `int`.

## Recovery and checks

The source now calls the typed listener and update methods, preserving
the original argument order, and calls `group->freeEventData(...,
false)` at both release sites. The listener method is exported by the
original ARM32 `libfmodevent.so`. The authorized ARM64 silent build
returns from `updateAll` without FMOD calls, matching its null system
state; ARM64 playback remains a separate unimplemented task.

- Focused ARMv7 build: 211/211 translation units. `updateAll`
  source-shape **80.6% -> 84.4%**, strict fuzzy **45.8% -> 64.9%**;
  `play` source-shape **87.8% -> 89.7%**, strict fuzzy **51.9% ->
  58.4%**. Both remain non-exact. The other 44 `FModSound`
  comparisons retain their previous results. A final focused rerun
  after the ARM64-only guard produced identical ARM32 results.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass.
- Raw ARM32 unresolved names: **186 -> 185**. The undefined
  free-event and listener helper names disappeared; the real FMOD
  listener method import was added.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, **96.83%** mean source-shape, **86.94%** strict fuzzy,
  **2,798 linked-exact**, **1,113 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 152 extra symbols. Both exact
  membership sets match the preceding result.

This verifies the original static call targets and data layout. Live
listener behavior, event cleanup, and a full ARM32 shared-library
link remain unverified. Next inspect event lookup and category handling
in `FModSound::updateEvent3DAttributes`.
