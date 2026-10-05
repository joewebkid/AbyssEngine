# ModStation pending-product delivery (2026-10-05)

Scope: `ModStation::checkPendingProducts()` and the pointer return type of
`Status::getPendingProducts()`. Primary evidence is Android 2.0.16
`libgof2hdaa.so`, `checkPendingProducts` at `0x0daabc..0x0dad54` and the
getter at `0x0a8fd2..0x0a8fd6`.

The former C++ path called unresolved fragment helpers, dereferenced null
placeholder globals, treated the item table as `Array<int>`, and discarded
the delivery-message construction. The recovered path uses the original
`Globals::status`, `Globals::gameText`, and `Globals::items` routes and the
existing `Status`, `Item`, `Ship`, `PendingProduct`, and `ChoiceWindow`
methods:

- Start with text `0xd5` and a newline. If a pending product belongs to the
  current station, special campaign step `0x92` plus blueprint `0xd2`
  deletes that product and returns without a notice.
- Otherwise create the item from the `Globals::items` template at its
  blueprint index using the pending quantity, add it to the current ship's
  cargo, and append a line: newline, delivered amount, `"x "`, and the
  item text at `itemIndex + 0x4fa`. The two literal strings are present at
  Android `.rodata:0x1ca4c8` and `0x1cb078`.
- Destroy the completed pending product, null its slot, and set the notice
  byte at `ModStation+0x63`. After the loop, display the accumulated text
  through the ChoiceWindow at `+0x70` when that byte is set. Temporary
  String objects are destroyed before the product is deleted, matching the
  source-observed lifetime order.
- The Android getter loads `Status+0x1c` straight into `r0`. It now returns
  `Array<PendingProduct *> *` directly instead of routing that pointer
  through native 32-bit `int`; its ARM instructions remain byte-exact.

Focused ARM `checkPendingProducts` source-shape improves from **43.3% to
99.7%**, strict fuzzy from **23.1% to 86.8%**. It is not linked-exact or
byte-exact. The getter remains 100% / 100% and byte-exact. The UCRT64
`gof2` static library and all 206 ARM translation units build. Focused
reports are `_work/blueprint-isunlocked/modstation_pending_{before,after}.json`
and `status_pending_{before,after}.json`.

The complete ARM corpus compares 4,322/4,524 original functions across
4,675 rows, with 90.15% mean source-shape and 77.30% strict fuzzy. It retains
2,166 linked-exact and 977 byte-exact pairs, with zero losses or gains in
either exact-pair set against the preceding station-entry snapshot. There
are 202 absent functions, zero wrong-type misses, zero skipped units, and
130 extra symbols. The report is
`_work/blueprint-isunlocked/modstation_pending_full.json`.
No running-game delivery was exercised, so cargo ownership, notification
display, and the special-product transition remain unverified at runtime.
The next bounded source-recovery priority is `ModStation::showDlcMenu()`,
which still relies on fragment callbacks and button-cache globals.
