# Engine device-information call ABI

Reference: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Engine::GetDeviceInfo()` at `0x6ddd6–0x6ddee` takes the hidden
structure-result pointer in `r0`. It calls `NFC::isPad` at `0x6dddc`,
stores the returned byte at result offset `+8`, calls `NFC::getWidth`
at `0x6dde2` and stores at `+0`, then calls `NFC::getHeight` at
`0x6dde8` and stores at `+4`. There is no receiver setup before any
of the three calls; after the first, `r0` contains the prior return
value. This supports treating these methods as static for the recovered
call ABI. Their own Android implementations use global JNI state and
do not read an `NFC` object.

The previous C++ expression `NFC().isPad()` (and equivalents for width
and height) constructed three temporary NFC objects and generated
35 ARM instructions for `GetDeviceInfo`. The recovered call now uses
`NFC::isPad/getWidth/getHeight` directly. This retains the original
method symbols while making the wrapper's field stores and call order
match the original. Other NFC methods and its constructor are unchanged.

Focused ARM validation compiles **209/209** translation units.
`GetDeviceInfo` rises from **48.7% source-shape / 31.1% strict fuzzy**
to **100% / 100%**, with **10** reconstructed instructions matching
the original **10**, and becomes linked-exact. The three JNI getter
functions retain their previous focused scores and exact states.
Windows native and ARM64 static builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,763** pairs, with **96.26% mean source-shape / 86.06% strict
fuzzy**, **2,747 linked-exact / 1,098 byte-exact**, 201 missing and
zero skipped. Against the preceding full report, `GetDeviceInfo` is
the sole linked-exact gain. Pair and byte-exact sets are unchanged;
there are no exact losses.

The direct calls and structure layout are verified by ARM shape;
live JNI class availability and returned values remain unverified.
Next audit the individual `NFC` getter JNI paths and exercise them
on a permitted host. Redmi testing remains stopped by user request.
