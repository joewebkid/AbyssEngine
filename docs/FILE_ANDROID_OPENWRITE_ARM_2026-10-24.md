# Android file-write mode and instance counter (2026-10-24)

In Android 2.0.16 ARM, `FileInterfaceAndroid::OpenWrite` at `0x6e208`
passes the literal `"wb"` to `fopen` at `0x6e258–0x6e260`, after building
the app-root-plus-wide-name path. It gets the wide-name pointer once at
`0x6e220` and reuses it for the String constructor at `0x6e23e`. The
recovered C++ had a null mode pointer and called the name accessor again.
The path now uses the observed mode and retained pointer.

The FILE constructor at `0x6de68` increments the exported
`FileInterfaceAndroid::fileCounter` at `0x6de8a–0x6de8e`. Its recovered
body instead dereferenced a null private counter pointer; that pointer now
addresses `fileCounter`. This also affects the ZIP/JNI constructors and
destructor, so their exact matches were checked in the full corpus.

A host gate extracts the production `OpenWrite` and FILE constructor bodies,
along with their mode and counter definitions, into the existing preview
round-trip test. At Windows `-O0/-O2` it creates a file under an isolated
output directory, verifies its two written bytes and the counter increment,
and checks that opening under a missing directory fails without increment.
The existing preview round trip still passes. This is a host boundary test,
not execution of Android's JNI or ZIP paths.

Native `gof2` builds; ARM compiles **208/208** translation units; **93**
verifier tests pass with one existing skip. Focused `OpenWrite` moves from
**76.0% source-shape / 31.6% strict fuzzy** to **88.9% / 74.6%**, still
nonexact. The FILE constructor aliases reach **100% / 100%** and become
linked-exact.

Fresh full ARM corpus compares **4,323/4,524** original functions over
**4,737** pairs with **201** missing and zero skips. Mean source-shape /
strict fuzzy rises **94.24% / 83.62% → 94.30% / 83.70%**. There are
**2,743** 100%-fuzzy, **2,578 linked-exact** and **1,082 byte-exact**
pairs. Exact pair-set comparison finds six new linked-exact aliases: two
FILE constructors, two ZIP constructors and two destructors. No linked-
or byte-exact pair is lost.

Two local touchHLE iOS sandbox save sets were located but not copied into
the repository: their game/preview files are **13,054/68** and
**15,757/60** bytes. They have not yet been run through the recovered
reader. Android `OpenRead` and `FileExist` still contain null literal and
state placeholders; normal launch, JNI/ZIP behavior and complete save
round-trip remain unverified. Next priority: use those identified previews
as read fixtures, then restore the Android read/existence path from ARM.
