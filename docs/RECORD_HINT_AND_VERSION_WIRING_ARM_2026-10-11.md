# Record hint and version wiring — Android ARM (2026-10-11)

The Android ELF places `Globals::hints` at `0x21824c`; the saved
gas-cloud byte `0x218279` is consequently index `0x2d`. The original
`GameRecord::load` restores record byte `0x109` into that address when
the extended record tag is present. The source had modeled the whole
hint range as a `PlayerEgo` object behind a fixed absolute pointer
slot. It now uses a packed overlay of `Globals::hints`, with compile-time
checks of its size and the `0x2d` offset.

The same ARM/ELF evidence identifies the other fixed-slot values in
this path: the version/DLC flag bytes and scalar are in
`Globals::options`, while the empty mission is `Mission::empty`.
`GameRecord::load` now uses those live symbols. On the write side,
`RecordHandler::recordStoreWrite` serializes flags from
`Globals::hints` and `Globals::options` instead of null aliases.
Original Android writes tag `0x6e6a78`; both writer and reader now
use it instead of zero. This is required for the extended flag
section containing hint `0x2d` to be read and applied.

Native `gof2` and all 206 ARM translation units build. The verifier's
36 tests pass (one skipped). Focused `GameRecord::load` source-shape
improves from 96.9% to 97.6% and strict fuzzy from 66.3% to 68.7%; the
writer improves from 62.0% to 90.9% source-shape and 22.6% to 26.9%
strict fuzzy. `recordStoreRead` remains at 26.4% source-shape and
14.1% strict fuzzy. Full ARM corpus: 4,323/4,524 original functions
compared, 92.14% average source-shape, 80.38% strict fuzzy, 2,343
linked-exact, 1,008 byte-exact, zero skipped units and zero wrong-type
symbols. No previously exact pair was lost.

These checks do not prove a running-game save/reload round trip.
`RecordHandler::recordStoreRead` still has null item- and ship-table
aliases, and other recovery logic is incomplete. Those dependencies
are the next bounded priority; existing saves were not rewritten.
