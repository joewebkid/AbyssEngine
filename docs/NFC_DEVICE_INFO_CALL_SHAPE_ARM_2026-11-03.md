# NFC DeviceInfo JNI call shape

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The three getters `NFC::isPad` (`0x69ccc`), `getWidth` (`0x69d7c`)
and `getHeight` (`0x69e08`) call the Java `DeviceInfo` methods already
identified in [the earlier JNI note](NFC_DEVICE_INFO_JNI_ARM_2026-10-31.md).
The original keeps the address of global `g_pEnv` in a register and
reloads its `JNIEnv*` value before each JNI call. All three getters
call the same local exception path at `0x69d5c` after the Java call.
That path checks JNI table slot `0x390` and conditionally tail-calls
slot `0x44` (`ExceptionCheck` and `ExceptionClear`).

The recovered getters had cached `g_pEnv` once and expanded the
exception check inline three times. They now read through the global
slot on each call and invoke one out-of-line `nfcClearException`
helper. The compiled helper has the original instruction sequence,
including the conditional tail call. The Java class, method names,
signatures, and returned values are unchanged.

ARM focused validation compiles **209/209** translation units. Scores
for `isPad` improve **72.1% / 48.8% → 100% / 100%**, and for both
`getWidth` and `getHeight` **71.3% / 52.6% → 100% / 100%**
(source-shape / strict fuzzy). `getWidth` gains linked-exact; neither
of the other two is linked-exact or byte-exact. No focused exact
match is lost. Windows native and ARM64 static builds pass.

The full corpus compares **4,323/4,524** original functions in
**4,763** pairs at **96.28% mean source-shape / 86.09% strict fuzzy**,
with **2,748 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. `getWidth` is the only linked-exact gain; no comparison pair
or exact match was lost. The new local helper adds one named symbol
to the verifier's extra-symbol count (**144 → 145**). Its role matches
the original unnamed `sub_69D5C` path.

The original and reconstructed functions match after instruction
normalization, but this is not proof of live JNI return values or
Java class availability. The other NFC wrappers remain separate
work. Next inspect one bounded wrapper family with original ARM
evidence; Redmi testing remains stopped by user request.
