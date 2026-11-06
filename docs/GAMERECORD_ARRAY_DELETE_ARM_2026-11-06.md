# GameRecord array deletion calls (2026-11-06)

Primary source: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `GameRecord::load` (`0x1559f0`), the old `Status` arrays at offsets
`0x94`, `0x98`, `0x90` and `0xac` are conditionally destroyed before
their pointers are replaced. The original calls `Array<bool>::~Array`
at `0x155e3c`, `0x155e5a` and `0x155ed8`, and `Array<int>::~Array`
at `0x155e78`. Each is followed by `operator delete`.

The ARM32 reconstruction previously called an undefined free
`Array_dtor(void*)` placeholder, then deleted its supposed return
value. The four paths now use typed `delete`, matching the original
destructor and deallocation call targets on ARM32 as well as the
existing Windows and ARM64 behavior.

Focused ARMv7 verification compiles **211/211** translation units.
`GameRecord::load` remains **97.6%** source-shape and **69.0%** strict
fuzzy, non-exact. The raw unmatched-symbol inventory over 211 ARM32
objects falls **232 -> 231**, removing only `_Z10Array_dtorPv`.
Windows native and ARM64 static builds pass, as does strict ARM64
shared linking.

The full ARMv7 corpus compares **4,323/4,524** unique original
functions in **4,772** pairs: **96.81%** mean source-shape, **86.85%**
strict fuzzy, **2,792 linked-exact**, **1,108 byte-exact**, 201
missing, zero skipped and zero wrong-type. Two additional comparison
pairs are the newly emitted `Array<bool>` and `Array<int>` destructor
instantiations in the `GameRecord` unit. The latter adds the single
linked-exact pair; `GameRecord::load` itself is unchanged in match
metrics. No previous exact-match pair is lost.

Live save loading, the `Array<bool>` destructor's non-exact code shape,
and full ARM32 shared linking remain unverified.
