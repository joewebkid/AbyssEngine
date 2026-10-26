# StarSystem KIPlayer target release (2026-10-26)

The Android 2.0.16 `StarSystem::~StarSystem` at ARM `0x134d10`
releases each `KIPlayer*` in its target array, destroys the array, and
then deletes its allocation. The element-release helper is
`ArrayReleaseClasses<KIPlayer*>` at `0xadebc`; the array destructor is
at `0xadf06`. The prior C++ body called two undefined wrappers,
`ArrayRelease_KIPlayer` and `Array_KIPlayer_dtor`. It now uses the
typed `Array<KIPlayer*>` operations already present in the project.
The second `playerTargets` null check is retained after element release:
virtual element destructors can run before the array is destroyed, and
the original ARM reloads this field at that point.

Native and Android AArch64 static archives build. The NDK r18b verifier
compiles **209/209** ARMv7 translation units. The focused StarSystem unit
now compares **38** functions, up from **36** in the last full ARM report.
It has **19 linked-exact** matches, up from **17**, and retains all
**6 byte-exact** matches. The two newly compared and linked-exact
functions are `ArrayReleaseClasses<KIPlayer*>` and
`Array<KIPlayer*>::~Array`; no prior exact symbol is lost.
`StarSystem::~StarSystem` remains nonexact: source-shape changes
**88.7% to 86.1%**, while strict fuzzy changes **89.1% to 90.1%**.
Focused report: `_work/starsystem_target_release_guard_unit_20261026.json`.

With the existing AArch64 libzip and crypto archives, strict link
diagnostics reduce unresolved names **457 to 455** whole-archive and
**217 to 215** JNI-reachable. Exactly the two wrappers above disappear;
no name is added. Logs:
`D:/gof2-arm64-crypto/link-after-starget-final.log` and
`D:/gof2-arm64-crypto/link-jni-gc-after-starget-final.log`.

The latest full ARM corpus remains the preceding run: **4,323/4,524**
original functions over **4,743** pairs, **94.72% source-shape**,
**84.23% strict fuzzy**, **2,604 linked-exact**, **1,083 byte-exact**,
zero skips. It was not rerun for this focused edit. Full StarSystem
destruction with populated targets has not been exercised in a running
game; the ARM64 `.so` still fails strict linking. Next priority:
source-backed JNI-reachable game and engine bridge calls, followed by
the Android host and compatible audio implementation.
