# MenuTouchWindow DLC dialog call (2026-10-11)

Scope: `MenuTouchWindow::callDlcMenu()` and its three ARM32 field-offset
assertions. The only direct source caller is `ModStation::showDlcMenu()`.
Primary evidence is Android 2.0.16 `libgof2hdaa.so`,
`_ZN15MenuTouchWindow11callDlcMenuEv` at `0x0012d064..0x0012d0cb`.

The original reads `Globals::appManager` twice and calls
`ApplicationManager::GetApplicationData()` at `0x12d076` and `0x12d082`.
It clears application-data byte `+0x4c`, sets byte `+0x3d`, reads the
`ChoiceWindow *` at `MenuTouchWindow+0x104`, sets message bytes `+0x170`
and `+0x17a`, and retrieves GameText IDs `0x47` and `0x1a9`. Its tail
branch at `0x12d0c0` reaches the PLT entry for
`ChoiceWindow::set(String const&, String const&)` at `0x66708` through a
linker veneer. GOT relocations at `0x20fd50` and `0x2102a0` identify the
app-manager and game-text globals. The source now calls those real methods
instead of undefined fragment callbacks. It retains the original repeated
reads of both globals, including the possibility of a change across an
external call, rather than caching their pointees.

Focused verification improves source-shape **97.3% to 100.0%** and strict
fuzzy **84.9% to 100.0%**. The function becomes **linked-exact**; it is
not raw byte-exact because of relocations. Native `gof2`, 206/206 ARM
translation units and 36 verifier tests (one optional skip) pass.

The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_call_dlc_20261011.json`:
4,323/4,524 originals, 4,689 comparison rows, **91.88%** mean
source-shape, **79.94%** strict fuzzy, **2,314 linked-exact**, **997
byte-exact**, 201 missing, zero wrong-type misses, zero skipped units and
112 extra symbols. Relative to the preceding DLC-menu snapshot, no row
or exact pair is lost; this function is the sole linked-exact gain.

No running-game DLC/store path or platform-side application-data consumer
was exercised. The next bounded priority is an evidence-backed audit of
the platform consumer of request byte `+0x3d` and acknowledgement byte
`+0x4c`, then an integrated DLC-dialog smoke when those services can run.
