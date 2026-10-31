# Hangar credit flags: options-record binding (Android ARM)

## Evidence

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
exports `Globals::options` as a 100-byte object at `0x2181e8`. The six social
offer bytes at `0x218230..0x218235` are therefore offsets `0x48..0x4d` of this
object. The credit-menu viewed byte `byte_218236` is offset `0x4e`.
That same byte gates the first hangar dialog; its source binding was completed
in `HANGAR_HINT_FLAGS_PERSISTENCE_ARM_2026-10-31.md`.

REA/IDA pseudocode for `HangarWindow::render` (`0x1490d8`) reads the five
offer flags and the viewed byte. `HangarWindow::OnTouchEnd` (`0x14c740`)
writes the offer flags and viewed byte. `RecordHandler::loadOptions`
(`0xcd450`) reads all seven bytes from the options file, and
`RecordHandler::saveOptions` (`0xcd8c8`) writes them. The file's existing
`OptionsBuffer` names the same offsets.

## Change

`HangarWindow.cpp` previously used separate zero-initialized file statics.
That discarded loaded offer state and left credited offers visible again after
restart. The viewed flag also did not share the saved byte. Both are now
aliases of the exact `Globals::options` bytes used by `RecordHandler`. The
existing reward table, touch dispatch, and save calls are unchanged.

## Verification

- Windows native `gof2` static target: built successfully.
- ARM64 `gof2` static target: built successfully.
- Android ARMv7: 209/209 translation units compiled, 0 failures.
- Focused `game/ui/HangarWindow` comparison: 40 functions, average source
  match 99.5%, 18 linked-exact, 6 byte-exact. `render` source match changed
  from 97.1% to 97.3%; `OnTouchEnd` remained 98.0%.
- Full corpus: 4,323/4,524 original functions compared, 4,752 comparison
  rows, no skipped units, 201 missing. Average source match 95.55%, fuzzy
  85.16%, 2,687 linked-exact and 1,091 byte-exact. Both exact sets are
  identical to the preceding `hangar-social-full.json` baseline: zero losses
  and zero gains. `OnTouchEnd` strict fuzzy changed from 70.3% to 68.1%, while
  `render` improved from 35.8% to 46.8%; the overall rounded means are
  unchanged.

The original binary confirms the options-record binding. Runtime behavior
on a phone was not exercised in this offline packet.
