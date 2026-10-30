# FileInterfaceAndroid JNI read recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FileInterfaceAndroid::Read` occupies `0x6e2e4–0x6e41c`. The existing ZIP
and FILE paths compare the returned byte count with the request. In the JNI
path, the original reads `FileInterfaceAndroid::env` for each JNI call,
allocates a byte array, and calls `JNIEnv::CallIntMethod` with the cached
`methodRead`. It describes and clears an exception only if one is present.
If the returned count equals the request, it copies the array to the caller
and builds a temporary `String` by appending a space and each byte converted
through `String(char)`. It always deletes the local array reference. The
temporary string has no observed consumer, but its construction and
destruction are visible in ARM and are preserved.

The prior source dereferenced null placeholder globals in the JNI branch
and described an exception for an ordinary short read. It now uses the
identified static fields and restores the three distinct success, short
read, and exception paths. Focused ARM source-shape / strict fuzzy for
`Read` changes **21.0% / 13.2% → 96.0% / 90.3%**. The function remains
nonexact; the residual instruction differences are in return/stack-canary
layout. The file unit gains one linked-exact `JNIEnv::CallIntMethod` wrapper
without losing any prior exact match.

The source-extracted host JNI mock compiles the production `Read` body and
checks full read, short read, exception handling, byte copy, and local
reference deletion at `-O0` and `-O2`. Native `gof2` and ARM64 static
builds pass; ARMv7 compiles 209/209 translation units.

The full corpus compares **4,323/4,524** original functions over **4,748**
pairs: **94.99%** mean source-shape, **84.66%** strict fuzzy, **2,664**
linked-exact and **1,088** byte-exact, zero skipped and 201 missing. The
complete `(unit, symbol)` exact sets have no losses against the preceding
report; the only gain is the `JNIEnv::CallIntMethod` wrapper. Reports and
host-test binaries are stored outside Git in `D:\gof2-arm64-crypto`.

The original `setEnvironmentVariables` JNI entrypoint at `0x6b5b4` assigns
`g_pEnv`, `g_pClass`, and `g_pActivity`; it does not assign
`FileInterfaceAndroid::env`. REA's direct references to that class field
show the file methods reading it, but do not establish how it is initialized
or whether JNI file streams were reached in normal play. The reconstructed
source likewise has no confirmed initializer. Live JNI behavior remains
unverified while device testing is deferred. Next, trace the provenance and
lifetime of `env` before enabling or claiming live JNI file I/O.
