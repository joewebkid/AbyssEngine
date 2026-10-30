# NFC DeviceInfo JNI calls (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `NFC::isPad` (`0x69ccc`), `getWidth` (`0x69d7c`) and
`getHeight` (`0x69e08`) load the global `g_pEnv`, find
`net/fishlabs/ow_utils/DeviceInfo`, then call these static Java methods:

| C++ method | Java method | JNI signature |
| --- | --- | --- |
| `isPad` | `isPad` | `()Z` |
| `getWidth` | `getDisplayWidth` | `()I` |
| `getHeight` | `getDisplayHeight` | `()I` |

Each original method invokes `sub_69D5C` after the Java call. Its ARM
body at `0x69d5c–0x69d7a` calls JNI table offsets `0x390` and, when
that returns true, `0x44`: `ExceptionCheck` then `ExceptionClear`.
The previous source dereferenced an uninitialized local `nfc_env`
pointer, passed empty class/method/signature strings, and incorrectly
called a declared `NFC_DeleteLocalRef` helper. These three paths now
use the real `g_pEnv` symbol, the evidenced class and method strings,
JNI static calls, and the original exception handling. The Android
JNI bridge defines and initializes `g_pEnv` in
`ToJNI_setEnvironmentVariables`.

On non-Android builds these methods currently return false or zero;
desktop device-information behavior needs its own platform provider.
The other `NFC` methods still contain unresolved placeholder JNI state
and are outside this packet. Live behavior and Java class availability
remain unverified while device work is deferred.

Focused ARM source-shape / strict fuzzy for `isPad` changes **0.5% /
0.0% → 72.1% / 48.8%**, for `getWidth` **0.6% / 0.0% → 71.3% /
52.6%**, and for `getHeight` **0.6% / 0.0% → 71.3% / 52.6%**. The
`engine/core/NFC` comparison covers 40 rows after this change,
including emitted JNI helper methods, versus 38 before it. Unit means
improve **40.1% / 22.4% → 48.5% / 30.2%**; linked-exact rises
**6 → 8**, byte-exact stays **2**, with no focused exact losses.
Windows and ARM64 static builds pass; ARMv7 compiles **209/209** units.

The full ARM corpus compares **4,323/4,524** originals over **4,751**
pairs: **95.23% mean source-shape**, **84.94% mean strict fuzzy**,
**2,686 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Compared with the preceding revision, two generated JNI
method wrappers gain linked-exact, with no linked-exact or byte-exact
losses. Reports are stored outside Git under `D:\gof2-arm64-crypto`.
