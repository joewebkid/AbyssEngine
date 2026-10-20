# PlayerFighter mission-crate ABI recovery (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::setMissionCrate(bool)` at `0xdcb18–0xdcb78`. The ARM body
stores the flag at Fighter `+0xd0` and returns immediately when false.
When true, it first clears the loot-list pointer at `+0x50`, allocates an
`Array<int>`, obtains the mission through `Globals::status->getMission()`,
and adds item `0x74` for mission type 5 or `0x75` otherwise, followed by
quantity 1. The independent recovered listing agrees with this call order.

The previous C++ narrowed the `Mission*` to `int` before calling
`Mission::getType()`. That is valid only accidentally on ARM32 and can
dereference a truncated pointer on the native 64-bit host. The recovery
uses the typed mission pointer and the same `Globals::status` source as ARM.
It also restores the pointer clear before allocation. The only in-tree
caller is the mission-ship creation path in `Level.cpp`.

`tools/run_player_fighter_mission_crate_smoke.py` extracts the production
method and tests both flag values and mission types at `-O0` and `-O2`.
The native UCRT64 `gof2` library builds; all 208 NDK r18b ARM translation
units compile; the other Fighter production-method smokes and 93 verifier
tests pass (one existing skip). Focused source-shape/strict fuzzy rises
from **97.8%/94.4%** to **100.0%/97.3%**. The function is still nonexact:
the original and rebuilt versions each contain 37 instructions, but a
single `mov r4, r0` is scheduled on opposite sides of the flag branch.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs, with **94.03%/83.37%** mean source-shape/strict
fuzzy, **2,557 linked-exact** and **1,082 byte-exact**. There are zero
skipped or wrong-type symbols. Comparing each `(unit, symbol)` with the
preceding `PlayerFighter::initPush` report finds no exact-pair loss and
only this function's source-shape changed. Full report:
`fighter-mission-crate-full-current.json` (SHA-256
`a67ecb4f15fc044ea925fcfd1f3e94ce1516e128a545e94772c37b126c31c13b`).

The smoke does not exercise mission-ship creation in a complete game, nor
does it establish behavior for a null mission pointer. The original ARM
code calls `getType()` without checking for null. The unresolved 3,556-
instruction `PlayerFighter::update` remainder remains a separate, much
larger priority; this mission-crate fix does not establish a continuous
Fighter update path.
