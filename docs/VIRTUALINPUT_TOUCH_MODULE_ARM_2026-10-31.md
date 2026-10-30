# Touch module and joystick override (2026-10-31)

Primary evidence is the original Android ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `simulateTouch` (entry `0x6aaf0`), instruction `0x6b0a6` reads the
hidden flag at `0x217998`. `UseJoystick` at `0x6a04c` writes this same
flag, and `GetUseJoystick` at `0x6a058` reads it. When set, the ARM
body copies the two saved stick floats into the eased stick floats.
The recovered source previously gated that copy on the separate
`SoftwareStick::useJoystick` field, which controls a different mouse
configuration path. The source now shares the hidden `UseJoystick`
flag between `NFC.cpp` and `VirtualInput.cpp`.

At `0x6b0b2–0x6b0c0`, the original loads `Globals::appManager`, calls
`ApplicationManager::GetCurrentApplicationModule`, and compares the
result to module 2 before synthesizing touch events. The previous
source used the separate `ApplicationManager::gAppManager` singleton.
The recovered condition now uses the observed global and method.

An IDA pseudocode view appeared to test only the horizontal eased
coordinate before `OnTouchEnd`. The ARM body at `0x6b138–0x6b152`
uses a conditional second floating-point compare, so both coordinates
must equal 0.5. The existing two-coordinate source condition was
retained. No claim about that condition rests on the shorter
pseudocode rendering.

Focused `simulateTouch` improves **83.3% → 83.7% source-shape** and
**38.2% → 38.7% strict fuzzy**. The 20-symbol input unit remains
**85.5% / 74.1%** after rounding and keeps nine linked-exact and two
byte-exact matches. The 38-symbol NFC unit keeps six linked-exact and
two byte-exact matches. Native Windows and ARM64 static builds pass;
ARMv7 compiles 209/209 translation units. Live joystick and touch
dispatch remain unverified while device testing is deferred.

The full ARM corpus compares **4,323/4,524** originals over **4,749**
pairs: **95.12% mean source-shape**, **84.82% mean strict fuzzy**,
**2,677 linked-exact**, **1,091 byte-exact**, zero skipped and 201
missing. Both rounded means and both exact counts are unchanged from
the preceding revision. Complete `(unit, symbol)` exact-set comparison
finds no gains or losses. The report is stored outside Git under
`D:\gof2-arm64-crypto`.
