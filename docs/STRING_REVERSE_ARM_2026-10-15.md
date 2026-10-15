# String::Reverse ARM recovery (2026-10-15)

Android GOF2 HD 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `AbyssEngine::String::Reverse` at `0x722e8–0x7236a`.
At `0x72300–0x7230c` it checks the source data pointer and language 9.
It then constructs a separate zero-terminated `String` from that data
(`0x7230e–0x72318`), walks the original stored length backward, reads
through the temporary string's indexed access with its out-of-range
terminator fallback (`0x7231c–0x72352`), writes into the existing data
buffer, and destroys the temporary (`0x72354–0x72356`). REA/IDA does not
provide complete body ranges or prove runtime behavior; these are direct
instruction observations.

The previous in-place swap normally produced the same visible text, but
disagreed when a buffer contained an embedded zero while its stored length
remained longer: the original first copies only the zero-terminated prefix
and writes terminators for out-of-range indices. The recovered body now
uses that copy and the existing `String::operator[]` fallback. A standalone
test compiles the production `String.cpp` and checks ordinary reversal,
another language, an embedded zero and an empty string:

```powershell
& 'C:\msys64\ucrt64\bin\g++.exe' -std=c++17 -O0 -ffunction-sections -fdata-sections -Isrc '-Wl,--gc-sections' src/game/core/String.cpp tools/test_string_reverse.cpp -o _work/string_reverse_smoke.exe
& '.\_work\string_reverse_smoke.exe'
```

Focused ARM source-shape/strict fuzzy rises **36.3%/7.8% →
91.0%/62.3%**, with **53 original / 51 reconstructed** normalized
instructions. This is neither linked- nor byte-exact; the 98% aspiration
is not met. The small source-only loop-shape trial follows the Android
`index != -1` condition and is retained for its evidence, not just its
score. Native `gof2`, **206/206** ARM translation units, the production
source smoke and 37 verifier tests (one expected skip) pass.

The full report at `_work/string_reverse_full_20261015.json` compares
**4,323/4,524** original functions at **93.47%** average source-shape
and **82.37%** strict fuzzy, with **201 missing**, **123 extras** and zero
skipped/wrong-type misses. Against
`_work/camera_create_full_20261015.json`, the exact symbol sets are
identical: **2,486 linked-exact** and **1,065 byte-exact**, zero losses
or gains. Only `String::Reverse` changes comparison metrics. The host
smoke does not execute the original ARM library; live language/locale
behavior remains unverified.

## Next ABI priority: PlayerFighter::setLevel

The original Android function at `0xdca30–0xdcb08` calls
`KIPlayer::setLevel`, reads geometry at `+0x08` and level at `+0x54`,
then uses `AEGeometry::getReferenceMatrix`,
`ParticleSystemManager::addSystem` and `enableSystemEmit(false)` five
times. The particle sets are 9, 15, 42, 17 and 18; manager offsets are
`Level+0x74/0x78/0x84/0x8c/0x8c`; handle writes are at
`PlayerFighter+0x1a0/0x80/0x84/0x134/0x138`.

Current `PlayerFighter::setLevel` instead calls three null function
pointers. The ARM compiler's layout for the current C++ class places
the derived `geometry` at `+0x128`, `level` at `+0x150`, the two
`field_0x80/0x84` names at `+0x170/+0x174`, the two
`field_0x134/0x138` names at `+0x1dc/+0x1e0`, and
`engineTrailSystem` at `+0x224`. The inherited `KIPlayer::geometry`
and `KIPlayer::level` do occupy `+0x08/+0x54`. Thus replacing the null
calls alone would still write several handles to wrong offsets. No
`PlayerFighter` code was changed here; reconstruct its subclass layout
and affected callers as a separate bounded ABI package before using
this method as a runtime gate.
