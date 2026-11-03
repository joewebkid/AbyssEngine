# ModStation suspend and resume (Android ARM)

Source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
`ModStation::OnResume` is at `0xdc29c–0xdc2ce` and `OnSuspend` at
`0xdc2d0–0xdc2ea`.

The ARM relocation at `0x210168` names `Globals::sound` and the one at
`0x21016c` names `Globals::options`. `OnResume` checks the sound pointer,
calls `FModSound::tryToStopMusicForBGMusic`, then tail-calls
`FModSound::setVolume(1, raw float at options offset 0)`. The value is
loaded as a 32-bit word; converting those bits through `int` changes the
value. The relocation at `0x210420` names `Globals::recordHandler`.
`OnSuspend` clears `activeTouch` at offset `0x128` and conditionally
tail-calls `RecordHandler::saveOptions`.

The source had null-initialized static pointer placeholders for all
three globals. It now uses the evidenced globals and passes the first
four options bytes as a float. Independent `MGame::OnResume` source
uses the same sound and options expression.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 units. Both symbols reach **100%**
source-shape and strict fuzzy, and both become linked-exact:

| Symbol | Before source / strict | After source / strict | Generated / original instructions |
| --- | --- | --- | --- |
| `OnResume` | 65.5% / 27.6% | 100% / 100% | 20 / 20 |
| `OnSuspend` | 55.9% / 14.3% | 100% / 100% | 11 / 11 |

The full ARM corpus compares **4,323/4,524** original functions over
**4,767** pairs. Mean source-shape rises **96.46% → 96.47%** and strict
fuzzy **86.32% → 86.35%**. There are **2,761 linked-exact** (two new)
and **1,098 byte-exact** symbols, **201** missing and zero skipped.
No pairs or exact symbols are lost.

Live suspend/resume, audio and option persistence remain unverified on
device. Redmi testing stays stopped. Next inspect the first
status-dependent branch in `ModStation::OnTouchEnd`.
