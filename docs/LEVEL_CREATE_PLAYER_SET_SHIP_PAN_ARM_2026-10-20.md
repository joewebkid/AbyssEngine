# Continuous Level to PlayerEgo ship binding — 2026-10-20

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
places `Level::createPlayer()` at Thumb `0xaf1bc`. Its normal path
constructs `PlayerEgo` at `0xaf28a`, stores it at `Level+0xf0` at
`0xaf292`, obtains ship index and race at `0xaf296–0xaf2aa`, calls
`PlayerEgo::setShip` at `0xaf2b4`, then `setLevel` at `0xaf2be`.

`tools/run_level_player_ship_pan_gate.py` extends the existing original-
asset Pan gate with the exact C++ `Level::createPlayer` source slice from
the player assignment through `setShip`. The earlier separate `setShip`
gate remains available. Windows SwiftShader and an Android ARMv7 device
pass at `-O0/-O2`: `Level::player` points to the constructed ego, its
new hull owns the first ship group, all five original Midorian AEM files
load in catalog order, and **37/37** original-file operations close.
The path ends immediately before production `PlayerEgo::setLevel`; it
does not establish that `Level::createPlayer` or `Level::init` return.

The same bounded platform-material service supplies a test mode for the
first ship mesh. Equipment, emergency, cloak, supernova and other ships
are not exercised. The next integration boundary is `setLevel` and its
particle-system handles, followed by hull rotation and weapon positions.

No production C++ changed in this package. The focused ARM comparison
for `Level::createPlayer` remains **86.0% source-shape**, **35.5% strict
fuzzy**, non-exact (408 generated versus 466 original instructions).
The current source tree retains the previous full report's
**4,323/4,524** originals in **4,735** comparisons, **93.95%/83.24%**
mean source-shape/strict fuzzy, **2,551 linked-exact**, **1,079
byte-exact**, and zero skipped units. That report is
`set-ship-init-full.json` (SHA-256
`fcd8ec2b7f2e3ff9c694dcc94e95bc1ee314160cda570539f29d666a8430bd8c`).
