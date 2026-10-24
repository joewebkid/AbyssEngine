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

This is not the recovered Android app running on-device: JNI is
executed as source-extracted host code, and archive access uses
libarchive rather than the original libzip/JNI environment. It does
not prove that the Java app invokes the method before native
initialization or that settings reload succeeds. Those runtime
ordering and recovered-device checks remain next priorities.
