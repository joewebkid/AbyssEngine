# PlayerEgo ARM32 movement division target (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
This pass covers the division call shared by `PlayerEgo::right`,
`left`, `down`, and `up`.

Original ARM calls `__aeabi_idiv@plt` at `0xa0874`, `0xa0ab8`,
`0xa0d32`, and `0xa0fa0`, respectively. The divisor is 63 (`0x3f`),
and the numerator is a signed integer converted from the preceding
float calculation. The reconstruction's ARM32 path instead called
an undefined C++ stand-in `aeabi_idiv_(int, int)`. Its 64-bit path
already used ordinary signed division.

The ARM32 path now directly calls the runtime `__aeabi_idiv`; the
small C++ forwarding expression is forced inline so no separate
stand-in symbol is emitted. The final ARM32 object has an undefined
reference to the standard runtime symbol and none to the C++
stand-in. This is an ABI target correction; it does not claim that
the surrounding movement math is fully recovered.

Focused ARMv7 verification compiles **211/211** translation units.
The four targets retain their preceding source-shape/strict fuzzy
scores: right **88.1%/67.4%**, left **88.3%/71.5%**, down
**88.0%/71.4%**, and up **88.4%/74.7%**. All remain non-exact.
Windows native and ARM64 static builds pass, as does strict ARM64
shared linking. The raw 211-object ARM32 unmatched-symbol inventory
falls **189 -> 188** with no new name.

The full ARMv7 corpus compares **4,323/4,524** original functions in
**4,772** pairs: mean source-shape **96.83%**, strict fuzzy
**86.93%**, **2,798 linked-exact**, **1,113 byte-exact**, 201 missing,
zero skipped, zero wrong-type, and 152 extra symbols. Both complete
exact-match membership sets are unchanged. An interim non-inlined
forwarder was rejected because it emitted an extra local symbol and
slightly reduced the four targets' strict fuzzy scores.

Live movement behavior and full ARM32 shared linking remain
unverified. Next priority: inspect the remaining gameplay-specific
ARM32 calls, including the UI sine target, and verify movement
against the original on a device when available.
