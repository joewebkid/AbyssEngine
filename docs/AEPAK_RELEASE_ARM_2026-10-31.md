# AEPakFile stream release (2026-10-31)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEPakFile::Release()` occupies `0x700d4–0x700f4`. When its owned
`FileInterface*` is non-null, ARM invokes the virtual `Close` method at
vtable offset `+0x44`, reloads the pointer, invokes the virtual deleting
destructor, and then clears the field. It returns one even when no
interface is present.

The recovered source previously relied only on `delete`. That happened
to close host and Android interfaces whose destructors call `Close`,
but omitted the explicit close required by the original ownership
contract. The source now calls `Close` before deleting the interface.
The standalone `Release` becomes **100.0% source-shape / 100.0% strict
fuzzy** and byte-exact; both nondeleting destructor aliases also become
linked-exact. Before this packet, `Release` was **80.6% / 82.8%** and
the destructor aliases were **80.4% / 96.3%**. The entire focused
`AEPakFile` unit is now 10/10 at 100% source-shape and strict fuzzy,
with 10 linked-exact and four byte-exact pairs.

The production-code synthetic PAK test passes at `-O0/-O2`. Its host
adapter counts all `Close` invocations: releasing a PAK file makes one
explicit call and one idempotent destructor call, while closing the
underlying stream exactly once. Destroying that PAK file after an
explicit `Release` causes no further close. Native `gof2` and ARM64
static builds pass; ARMv7 compiles 209/209 units. Packed PAK entries, alternate
interface implementations and live Android ownership remain
unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: mean source-shape **95.02% → 95.03%**, strict fuzzy
**84.71% → 84.71%**, **2,670 linked-exact** and **1,090 byte-exact**,
zero skipped and 201 missing. The complete exact-symbol sets lose no
matches. `Release` and its two nondeleting destructor aliases add three
linked-exact pairs; `Release` also adds one byte-exact pair. The
verifier reports are stored outside Git in `D:\gof2-arm64-crypto`.
