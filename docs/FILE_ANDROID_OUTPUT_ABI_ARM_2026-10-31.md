# FileInterface Output return ABI (2026-10-31)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FileInterfaceAndroid::Output(char*)` at `0x6e800` is a single `BX LR`:
it neither uses its argument nor constructs a return value. Both the
Android Ghidra and IDA decompilations identify it as `void`; the
independent GOF1 iPhone engine's `FileInterfaceIPhone::Output(char*)`
is also a void no-op. The iPhone function is supporting lineage
evidence, not a GOF2 byte-match source.

The recovered interface had declared `char *Output(char*)` and returned
the input pointer, adding an ARM instruction and an unsupported return
contract. `FileInterface`, its Android subclass, the read-only host
adapter and the archive-test adapter now declare `void Output(char*)`.
The virtual slot order and object fields are unchanged. There are no
in-tree calls that consume an `Output` return value.

Focused ARM source-shape / strict fuzzy improves **76.7% / 66.7% →
100.0% / 100.0%**; `Output` becomes byte-exact and linked-exact.
ARMv7 compiles 209/209 units; native `gof2` and ARM64 static builds
pass. The production-code PAK and source-extracted Android read tests
pass at `-O0/-O2`. The real-archive test adapter also compiles and
links with the corrected method signature; its APK/OBB runtime path
was not rerun in this packet. Live Android behavior remains unverified
while device testing is deferred.

The full ARM corpus compares **4,323/4,524** original functions over
**4,749** pairs: **95.02%** mean source-shape, **84.71%** strict fuzzy,
**2,667 linked-exact** and **1,089 byte-exact**, zero skipped and 201
missing. Both complete `(unit, symbol)` exact sets have no losses;
`Output` adds one linked-exact and one byte-exact pair. The corpus
source-shape mean is unchanged at its displayed precision. The
verifier reports are stored outside Git in `D:\gof2-arm64-crypto`.
