# Android country-code handoff to GameText (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ndk23_setCountryCode` at `0x943a8..0x943ba` clamps unsigned
codes 16 and above to zero, then writes through GOT `0x210164`.
The ELF relocation identifies that slot as
`AbyssEngine::Engine::countryCode` at `0x2150e0`. Original
`OnCreateApplication` loads the same slot at `0xd09ae..0xd09ba`,
passes its signed halfword to `GameText::setLanguage(short,int)` at
`0xd09c6`, and supplies count **3401**. The JNI export
`Java_..._ToJNI_setCountryCodeOfDevice` at `0x6b6c8..0x6b6cc`
moves its third argument into `r0` and branches through veneer
`0x19aea8` to `ndk23_setCountryCode@plt` at `0x5e9bc`.

Recovered `JniBridge.cpp` instead wrote a private `countryCode`
that no reader used, while `OnCreateApplication` read the separate
`Engine::countryCode`, initially zero. Thus Java's selected language
could not reach startup. The setter now writes the ARM-backed engine
field and retains the original unsigned clamp. Its explicit no-inline
boundary preserves the observed JNI tail-call instead of folding the
setter into the wrapper.

Focused ARM source-shape/strict fuzzy:

| Symbol | Before | After | Exact |
| --- | --- | --- | --- |
| `ndk23_setCountryCode` | 50.2% / 20.0% | 100% / 100% | linked-exact, not byte-exact |
| JNI `setCountryCodeOfDevice` | 24.4% / 0% after setter correction but before call-boundary restoration | 100% / 100% | nonexact |

The original-archive host gate extracts both unchanged function bodies
from `JniBridge.cpp` and executes them with production `GameText`,
`AEFile` and the source-extracted Android archive-open path. At
Windows `-O0/-O2`, the JNI code 5 selects the Russian 3401-record
table, code 8 selects absent `cz.lang` and falls back to English,
and codes 16 and -1 clamp to zero. The existing direct English and
APK/OBB file gates pass. Native `gof2`, all **208/208** ARM units and
the **93-test** verifier suite (one skip) pass. The full corpus
compares **4,323/4,524** originals over **4,737** pairs at
**94.39%/83.84%** mean source-shape/strict fuzzy, **2,580
linked-exact** and **1,082 byte-exact**. Relative to the preceding
report, only `ndk23_setCountryCode` enters the linked-exact set;
no linked- or byte-exact pair is lost.

The installed original APK is byte-identical to the local 2.0.16 APK
(SHA-256 `515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`).
Its DEX shows `GOF2HD2012.onCreate` at `0x21c118`: it obtains
`Locale.getDefault().getLanguage()` and `.getCountry()`, then calls
`setCountry` at `0x21c386`. That method at `0x21c690` uppercases the
language argument and calls `ToJNI.setCountryCodeOfDevice` with GB=0,
DE=1, FR=2, IT=3, ES=4, RU=5, PL=6, PT=7, KR=14, JP=15, or 0 for
other languages. The country argument is unused. The only DEX call to
`ToJNI.initialize` is in `GLViewRenderer.onSurfaceChanged` at
`0x21b8be`, guarded by its `initialized` flag. This confirms the
intended Java-to-native startup handoff, but not a strict cross-thread
happens-before order: `onCreate` calls `setContentView(mGLView)` at
`0x21c2b6`, before the locale setter, and the renderer callback can
run on the GL thread. A live call-order trace is still needed to rule
out an early first-frame initialization race.

This is not the recovered Android app running on-device: JNI is
executed as source-extracted host code, and archive access uses
libarchive rather than the original libzip/JNI environment. Settings
reload and recovered-device execution also remain unverified.
