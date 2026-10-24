# AEFile interface app-root handoff (2026-10-24)

Android 2.0.16 ARM `AEFile::SetInterface(FileInterface*)` at `0x7019c`
checks the enabled interface and then the exported `AEFile::appRoot`
pointer. When non-null, it passes that pointer to virtual slot `0x34` at
`0x701bc` before creating the file arrays and installing the interface.
The separate `AEFile::SetAppRootDir` wrapper at `0x71258` uses the same
virtual slot; the recovered `FileInterface` declaration identifies it as
`SetAppRootDir(void*)`, not `ResetSaveDirectory()`.

The previous C++ used a private initialization flag that is never set in
this source, so it never forwarded the root pointer. Its guarded call was
also to `ResetSaveDirectory()`. The production branch now checks `appRoot`
and calls `SetAppRootDir(appRoot)` on the incoming interface. A host-only
observer on the existing read-only asset interface checks that the pointer
is forwarded and the save-directory reset is not called. The existing
source/asset path runs at Windows `-O0/-O2` and reports **179/179** balanced
file lifetimes at each optimization level.

Focused ARM builds **208/208** translation units and moves this function
from **79.2% source-shape / 61.2% strict fuzzy** (33 generated vs 52 target
instructions) to **90.2% / 75.8%** (43 vs 52). It is not linked- or
byte-exact. Native `gof2` builds and **93** verifier tests pass with one
existing skip.

Fresh full corpus compares **4,323/4,524** original functions over
**4,737** pairs with **201** missing and zero skips. Mean source-shape /
strict fuzzy stays **94.24% / 83.62%**; there are **2,739** 100%-fuzzy,
**2,572 linked-exact** and **1,082 byte-exact** pairs. Exact pair-set
comparison against the preceding report finds no gains or losses.

The original exports `AEFile::appRoot`, but no in-tree assignment to it
was found. The host check deliberately supplies a non-null pointer; it
does not prove this branch executes during a normal game launch. A genuine
original save fixture and live Android were unavailable. Next priority is
to identify any external/JNI producer of `appRoot` and verify the complete
save/preview load lifecycle against real data.
