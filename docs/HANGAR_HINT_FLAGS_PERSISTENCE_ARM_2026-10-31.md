# Hangar hint flags: saved-state binding (Android ARM)

## Evidence

The original Android `libgof2hdaa.so` exports `Globals::hints` as a 59-byte
object at `0x21824c`. REA/IDA pseudocode for `HangarWindow::setSellMode`
(`0x14bff0`) and `selectItem` (`0x14e570`) reads and writes
`0x218269`, `0x21826a`, and `0x21826b`. These are `Globals::hints` offsets
`0x1d`, `0x1e`, and `0x1f` for the buy, sell, and slot explanations.
`RecordHandler::recordStoreWrite` serializes hint offsets `0x08..0x20`;
`GameRecord::load` restores the `0x18..0x27` block from the game record.

`HangarWindow::initialize` (`0x147e24`) uses `byte_218236` for its first
hangar dialog. This is the same `Globals::options[0x4e]` byte used by the
credit-menu viewed gate and saved by `RecordHandler::saveOptions`.

## Change

The three explanations previously used file-local zero-initialized bytes,
so loaded game-record state did not suppress already-seen dialogs. They now
refer to the exact `Globals::hints` bytes. The separate first-hangar dialog
byte was removed; both users now refer to the shared saved options byte.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused HangarWindow: 40 comparisons, average source 99.5%, fuzzy 91.2%,
  18 linked-exact and 6 byte-exact. Compared with the preceding focused
  report, `initialize` source 97.7% -> 97.8%, `selectItem` 98.2% -> 98.5%,
  `setSellMode` 99.4% -> 99.7%; no exact losses.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows; zero skipped and 201 missing. Average source 95.55%, fuzzy 85.16%,
  2,687 linked-exact and 1,091 byte-exact. Both exact sets are identical to
  `hangar-options-full.json`: zero losses and zero gains. The overall rounded
  means are unchanged.

No phone save/reload cycle was run in this offline packet. The next priority
is testing the saved dialog flags in a real game session once device work
resumes, then auditing other unbound UI state.
