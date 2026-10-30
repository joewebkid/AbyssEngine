# FileInterfaceAndroid JNI write-stream recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The reviewed functions are the `jobject` constructor at `0x6de9c`, `Write`
at `0x6e4ac`, and `Close` at `0x6e7a0`.

The original constructor reads the shared `FileInterfaceAndroid::env`, obtains
the stream class, and caches method IDs. A nonzero mode flag selects
`write([B)V` and the write-side `close()V`; zero selects `read([B)I` and the
read-side `close()V`. The original ELF's GOT references resolve to the
class's `methodWrite`, `methodRead`, `methodCloseWrite`, and `methodCloseRead`
fields. The source previously dereferenced null placeholder pointers for
these caches and reversed the mode names. Those placeholders are now replaced
with the identified class fields and literal Java method signatures.

For a JNI stream, the original `Write` creates a byte array, copies the input
bytes, calls `JNIEnv::CallVoidMethod` with `methodWrite`, checks and clears a
Java exception, and deletes the local array reference. `Close` selects the
cached close method by mode and calls the same JNI wrapper before clearing
the stream field. The restored source follows these paths. The existing FILE
and ZIP paths were not changed.

Focused ARM comparison (source-shape / strict fuzzy):

| Symbol | Before | After |
| --- | --- | --- |
| `jobject` constructor, each alias | 37.4% / 8.8% | **94.4% / 61.1%** |
| `Write` | 33.8% / 14.0% | **91.0% / 52.9%** |
| `Close` | 75.3% / 69.1% | **94.9% / 69.4%** |

None of these four aliases is exact. The 34-symbol file unit retains its
previous exact symbols, and the `JNIEnv::CallVoidMethod` wrapper is newly
linked-exact. Native `gof2` and ARM64 static builds pass; ARMv7 compiles
209/209 translation units.

The full corpus compares **4,323/4,524** original functions over **4,747**
pairs: **94.97%** mean source-shape, **84.64%** strict fuzzy, **2,663**
linked-exact and **1,088** byte-exact, zero skipped and 201 missing. Compared
with the preceding full report, the complete `(unit, symbol)` exact sets
have no losses; only the JNI wrapper gains linked-exact status. The reports
are stored outside Git in `D:\gof2-arm64-crypto`.

The source still has no confirmed assignment to `FileInterfaceAndroid::env`.
The separate JNI `Read` implementation still uses null placeholder globals
and is not restored by this packet. Live JNI stream behavior remains
unverified while device testing is deferred. The next bounded packet should
trace initialization of `env` and restore `Read` against its ARM control flow
before treating JNI file streams as functional.
