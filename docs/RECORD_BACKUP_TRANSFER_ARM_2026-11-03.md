# Record backup transfer bindings

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`RecordHandler::changeSaveDirectoryToBackupDirectory` begins at
`0xd0258`. The original reads `Globals::recordSlots` when sizing
both record arrays and again after read and write calls. It sets
`Engine::backupSaveGames` to one before writing any records.

The reconstruction read two private null-initialized pointer slots
instead. The function now reads the exported record count and sets
the original engine flag. `Globals::recordSlots` is declared as a
pointer-shaped value elsewhere in the project, but stores a small
integer count (six or twelve during `Globals::init`). The conversion
through `uintptr_t` preserves that value on ARMv7 and ARM64.

Focused ARM verification compiled **209/209** translation units.
The function rises from **34.6% to 92.4% source-shape** and from
**19.1% to 80.5% strict fuzzy**, with 121 generated instructions
against 115 original. The `RecordHandler` unit retains **21
linked-exact** and **1 byte-exact** function without losses.
The remaining assembly difference includes allocation cleanup:
the reconstruction deletes the temporary array objects after
releasing their data, while the original ARM does not show those
object deletes. No semantic change was made for code shape alone.

A host smoke compiles the unchanged production function at `-O0`
and `-O2` with in-memory record methods. It checks two populated
slots, one missing slot, current and backup writes, next-slot hash
updates, the backup flag's ordering, and the zero-slot case. Windows
native and ARM64 static builds pass. The smoke performs no file I/O;
real save files and persistence remain unverified. No device run was
made; Redmi testing remains stopped by user request.

The full ARM corpus compares **4,323/4,524** original functions in
**4,764** pairs at **96.45% mean source-shape / 86.32% strict fuzzy**,
with **2,757 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. Relative to the preceding full report, all pair and exact
sets are unchanged; there are no losses.

Next: map one later `ModStation::OnTouchEnd` route against original
ARM before replacing its overloaded private pointer aliases.
