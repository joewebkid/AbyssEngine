# ABI Runtime Gate (2026-09-27)

Scope: the 44 in-scope extra symbols, application-module vtables, String/Vector
by-value returns, and Array ownership. This is a focused gate, not a claim
that the recovered engine runs safely on ARM32.

## Extra Symbol Triage

`tools/scope_filter.py` reports 44 in-scope local symbols absent by exact name
from the Android function table. The complete list is
`cmake-build-match/verify/in_scope_extra.txt`:

| Group | Count | ABI significance |
| --- | ---: | --- |
| `_ZL` file-local helpers | 35 | No externally visible name; inspect behavior, not symbol parity. |
| Hangar `OnTouchEnd` lambda operators | 4 | Local code-generation shape, not exported ABI. |
| `AbstractGun`/`IApplicationModule` D0/D2 | 4 | Virtual destruction needs live-object checks. |
| `AbyssEngine::String` copy C2 | 1 | Deep-copy lifetime and call sites need checks. |

The original `.so` has derived `BombGun`, `MineGun`, `SentryGun`, `RocketGun`,
`ObjectGun`, `SpriteGun`, `BeamGun`, `MGame`, and `ModStation` vtables, but no
exported base `AbstractGun` or `IApplicationModule` vtable symbol. Extra base
destructor variants must not be removed merely to reduce the count: they are
emitted by local C++ virtual destruction. The native smoke below invokes both
through base pointers. The local `String` copy constructor calls `Set`, giving
an independent allocation in that smoke, but the absence of its exact Android
symbol does not establish how every original caller copied a String.

## Module Vtable Correction

Android `.data.rel.ro` relocations establish the same address-point order for
`MGame` (`_ZTV5MGame` at `0x20e4f0`, address point `+8`) and `ModStation`
(`_ZTV10ModStation` at `0x20d9e4`, address point `+8`):

| Slot offset | Android relocation target |
| ---: | --- |
| `+0x08` | `OnInitialize` |
| `+0x30` | `OnUpdate` |
| `+0x34` | `OnRender3D` |
| `+0x38` | `OnRender2D` |
| `+0x44` | `ShowLoadingScreen` |

`ApplicationManager.cpp` previously named `+0x08` showLoading and dispatched
the three update/render methods as `render3D(+0x30)`, `render2D(+0x34)`,
`update(+0x38)`. That would call the wrong live methods even though the
library linked. Its vtable description and call sites now follow the original
order. The 32-bit offsets are compile-time asserted. A native probe module
calls those same five slots and checks virtual destruction. The focused ARM
`ApplicationManager::OnUpdate` comparison remains 8.2% source / 3.8% strict;
the corrected slots do not byte-match the whole large method.

## Lifetime And Return Smoke

`gof2_abi_runtime_smoke` compiles the actual `String.cpp`,
`IApplicationModule.cpp`, and `AbstractGun.cpp` with `Array.h`. It repeats
`Array<bool>` growth/copy/assignment/removal, owned `Array<String*>` release,
and String construction/copy/assignment/substring/destruction 100 times,
checks Vector by-value return,
calls the application-module vtable slots, and deletes module/gun probes via
base pointers. The isolated target contains aborting definitions only for
unrelated String.cpp dependencies that the smoke never calls. UCRT64 run:
`GOF2 native ABI/lifetime smoke: OK`.

The same smoke translation unit compiles with matching Android NDK r18b ARMv7
flags. NDK r18b also links the complete Android `armeabi-v7a` ELF at
`_work/abi-arm-build/gof2_abi_runtime_smoke` (ELF32 ARM PIE, about 863 KiB;
only `libm`, `libdl`, and `libc` dynamic dependencies). The old NDK CMake
toolchain needs a temporary no-space `subst X:` mapping for this workspace.
Its Thumb disassembly shows `String::SubString` called with hidden
destination in `r0`, object in `r1`, and `start/end` in `r2/r3`; the 12-byte
Vector return uses a caller-provided `r0` destination. ARM relocations of the
probe vtable place update/render in the same slot order as the original game.
The original Android `String::SubString` at `0x72ed8` saves `r0` as its result
slot and `r1` as `this`, with `r2/r3` as the bounds. Original
`AEGeometry::getPosition` at `0xa4592` saves `r0` as its Vector result slot,
uses `r1` as `this`, and passes that slot to `MatrixGetPosition`. Thus the
return-slot placement is backed by both original and local ARM instructions.
These were initially call-convention checks, not execution on Android.

## ARM32 device follow-up (2026-10-15)

The same smoke was rebuilt from the current
`codex/integrate-main-working-20261005` checkout using NDK r18b,
`ANDROID_ABI=armeabi-v7a`, `ANDROID_PLATFORM=android-21` and the
existing CMake target. `readelf` identifies the result as ELF32 ARM
PIE; SHA-256 is
`080636078a8936462bdac48402f362e87255bb2cdb864b873488355cb2226b07`.
The temporary `subst X:` mapping was removed after the build.

ADB reported an M2007J20CG on Android 12 supporting `armeabi-v7a`.
The isolated executable was pushed to a previously absent, uniquely
named file in `/data/local/tmp`, run five times, and removed. All five
runs printed `GOF2 native ABI/lifetime smoke: OK` and exited zero.
Each run includes the smoke's 100 String/Array lifetime iterations,
Vector return, application-module vtable slots and virtual destruction.
The temporary device file was confirmed absent afterward. The UCRT64
host smoke and native `gof2` target also remained green.

This is **observed execution of the reconstructed isolated ABI probe**,
not execution of the original GOF2 binary, a memory-sanitizer result,
or proof that the full native game runs. The most recent full ARM
corpus before this documentation-only update, at
`_work/player_end_explosion_full_20261015.json`, remains
**4,323/4,524** compared, **93.43%** source-shape, **82.33%** strict
fuzzy, **2,485 linked-exact**, **1,065 byte-exact**, **201 missing**,
**123 extras** and zero skipped; no new source-shape or exact claim is
made from this device run.

The final full ARM verifier run after the dispatcher edit compares 4,626
functions, covers 4,282/4,524 original names, reports 242 external exact-name
misses, zero wrong-type names, 2,072 linked-exact and 942 byte-exact. The
scope filter again reports zero in-scope absent, zero wrong-type, 44 extra.
The 25 verifier unit tests and the UCRT64 `libgof2.a` build pass. The large
`ApplicationManager::OnUpdate` body stays 8.2% source / 3.8% strict; this
change corrects virtual call targets, not its whole-function instruction
shape.

`Array<T>` currently allocates with `::operator new[]`, grows with `realloc`,
and releases with `::operator delete[]`. The host test does not prove that
mixing these allocator APIs is portable or safe for non-trivial `T`. The
original Android `Array<bool>` constructor at `0xa8ce2` calls `_Znaj`, its
`ArrayAdd<bool>` at `0x17698c` calls `realloc`, and its destructor at
`0xad998` calls `_ZdaPv`. Thus this pairing follows the original path; it is
not an accidental invention of the port. The exact allocator implementation,
behavior for non-trivial elements, and every ownership call site still need
ARM32 runtime validation. Do not change the generic Array allocator solely
for a verifier score without checking this original convention.

## Remaining Gate

- Repeat the now-passing Android `armeabi-v7a` smoke under a memory checker
  where available, and exercise original derived modules and guns rather
  than only the isolated probe classes.
- Inspect the actual Android C++ allocator targets and `Array<bool>`/
  `ParticleSettings` callers before declaring new[]/realloc/delete[] safe.
- Extend return-slot checks to real game functions and both branch paths, not
  just `String::SubString` and a Vector probe.
- Keep the 39 local helper/lambda extras separate from the five ABI-sensitive
  entries; no symbol was removed to cosmetically reach zero extras.

No ARM32 runtime was available in this environment: ADB has no connected
device, WSL/QEMU are unavailable, and the installed ARM64 emulator image has
an empty 32-bit ABI list. The native pass must not be labelled ARM32-safe.
