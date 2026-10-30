# FileRead ship-parts loader recovery (2026-10-30)

The primary source is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`FileRead::loadShipParts` occupies `0x11fad0–0x11fc2c` and opens
`data/bin/shipparts.bin`. Its first two reads call `AEFile::Read(signed
char&, uint)`. The prior C++ used `char`, which selected a different
overload on the matching build. The fields are now `int8_t`, preserving
the signed group comparison and signed count used to size the array.

ARM destroys the temporary `String` immediately after `FileExist`, before
`OpenRead`. The source now uses a temporary at that call. ARM reloads the
array data pointer between `ReadSwitched` calls; the source no longer keeps
one pointer across the ten-field record parse. The loop scans at most 64
groups. It returns the matching group's array without closing the handle,
or closes and returns the last allocated array after exhausting the groups.
These unusual ownership paths are observed in the original and retained;
the local change does not infer a safer policy from an unavailable asset.

Focused ARM source-shape / strict fuzzy for `loadShipParts` improves
**70.6% / 35.5% → 97.7% / 76.4%**. The function remains nonexact. Its
156 original instructions compare with 158 compiled instructions; most
remaining differences are register allocation and placement of the
`ReadSwitched(int&)` function address. Explicitly caching that address in
C++ did not change the result and was rejected. The 48-symbol `FileRead`
unit retains its 19 linked-exact and four byte-exact pairs.

A source-extracted host test compiles the production loader at `-O0/-O2`
and checks the ten parsed values, signed group identity, absence of a
resource, and destruction of the temporary path before `OpenRead`. Native
`gof2` and ARM64 static builds pass; ARMv7 compiles 209/209 units.

The full corpus compares **4,323/4,524** original functions over **4,748**
pairs: **94.99%** mean source-shape, **84.66%** strict fuzzy, **2,664**
linked-exact and **1,088** byte-exact, zero skipped and 201 missing. Rounded
global means are unchanged by this single-function repair. Against the
preceding full report, the complete `(unit, symbol)` linked-exact and
byte-exact sets have no gains or losses. The reports and host-test binaries
are stored outside Git in `D:\gof2-arm64-crypto`.

The original `shipparts.bin` is unavailable locally, so parsing against
real records, malformed counts, the no-match result, and live caller
ownership remain unverified. No direct call to this loader was found in
the current recovered source. Next offline priority: inspect
`AEFile::findPakFile`, which is on the archive lookup path.
