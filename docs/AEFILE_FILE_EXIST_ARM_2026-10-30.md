# AEFile existence check and archive path (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`AEFile::FileExist(String const&)` starts at `0x710a8` and ends at
`0x7116e`. At `0x710d6–0x710e8`, ARM constructs `String(path, false)`
directly as the by-value virtual `FileInterface::FileExist` argument,
calls the interface, and destroys the temporary before checking its
result. The previous source made a local string and then incurred an
additional by-value copy for the call.

On a native miss, ARM constructs another `String(path, false)`, checks
its first character, and prepends `/` for archive lookup if necessary.
The prepend at `0x71114–0x71144` uses an explicit `String(path, false)`
copy alongside the prefix. The recovered source retains those three
temporary lifetimes. The original calls `findPakFile` and checks its
returned pointer without an observed delete; that ownership behavior
is unchanged here.

Focused ARM source-shape / strict fuzzy improves **87.0% / 67.6% →
99.5% / 86.8%**. `FileExist` remains nonexact. Native `gof2` and ARM64
static builds pass, and ARMv7 compiles 209/209 units. The production
synthetic PAK test passes at `-O0/-O2`, including native existence and
missing archive path. A positive PAK existence result may retain an
opened `AEPakFile` object according to the original call path; the
synthetic gate does not claim its lifetime is safe. Live Android file
behavior remains unverified while device testing is deferred.

The full corpus compares **4,323/4,524** original functions over
**4,748** pairs: **95.00%** mean source-shape, **84.67%** strict fuzzy,
**2,664 linked-exact** and **1,088 byte-exact**, zero skipped and 201
missing. Both complete `(unit, symbol)` exact sets are unchanged from
the preceding full report. The corpus mean is unchanged at its displayed
precision. Reports and host-test outputs are outside Git in
`D:\gof2-arm64-crypto`.
