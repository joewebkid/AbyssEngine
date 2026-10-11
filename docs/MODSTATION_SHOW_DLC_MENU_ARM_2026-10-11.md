# ModStation DLC menu setup (2026-10-11)

Scope: `ModStation::showDlcMenu()` only. Its direct caller is
`HangarWindow.cpp` at the station-menu path. Evidence is Android 2.0.16
`libgof2hdaa.so`, `_ZN10ModStation11showDlcMenuEv` at
`0x000dc1bc..0x000dc29b`.

The old source used null button-cache globals and unresolved fragment
callbacks. The ARM body creates `MenuTouchWindow(2)` at `0xdc1d8..0xdc1e8`
when `ModStation+0x50` is null, writes the open byte at `+0x62`, copies the
first ten button coordinates as integers into the real global arrays, stores
the full button count, clears the byte at `+0x66`, then calls
`MenuTouchWindow::callDlcMenu()` at `0xdc264`. The ELF GLOB_DAT relocations
identify the arrays and count: `Globals::sub_menu_buttons_x` at GOT
`0x00210418`, `sub_menu_buttons_y` at `0x0021041c`, and
`sub_menu_button_count` at `0x0020fd4c`. The source now uses those fields,
globals and methods directly. ARM32 layout assertions cover the three
`ModStation` field offsets involved.

Focused source-shape improves **57.3% to 99.0%** and strict fuzzy **29.8%
to 72.6%** (79 original versus 78 compiled instructions). Control-flow and
call-count scores are both 100%; this function is neither linked-exact nor
byte-exact. Native `gof2`, 206/206 ARM translation units, 36 verifier tests
(one optional skip), and four scope tests pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_dlc_menu_20261010.json`:
4,323/4,524 originals compared across 4,689 rows, **91.88%** mean
source-shape, **79.94%** strict fuzzy, **2,313 linked-exact** and **997
byte-exact**. Against the preceding loader snapshot, no comparison row or
exact pair was lost. There are 201 absent runtime/library symbols, zero
wrong-type misses, zero skipped units, and 112 extra symbols.

No running-game DLC/store flow was exercised. Constructor side effects,
button population, app-data flags and the downstream
`MenuTouchWindow::callDlcMenu()` helper path therefore remain unverified at
runtime. The next bounded priority is auditing that downstream helper and
its fragment callbacks against ARM before claiming DLC-menu behavior.
