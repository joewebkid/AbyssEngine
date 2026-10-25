# WantedWindow selected-entry text recovery (2026-10-25)

Primary reference: Android 2.0.16 ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow::selectWanted(int)` starts at Thumb address `0xe1034`.

The active branch obtains three stations and their systems from
`Globals::galaxy` (`0xe10e0..0xe1164`), then formats each as
`station name + " (" + system name + ")"` (`0xe116e..0xe129c`). The
prior source used speculative English labels and omitted the system
names. Both the active and inactive, non-terminated branches use an
integer `String` constructor and append `"$"` to the reward
(`0xe12c6..0xe12ee`, `0xe1438..0xe1460`), not `"reward "`.

The terminated branch sets the four location/reward fields to the
ARM-observed placeholders `" -- "`, `" --"`, `" --"`, and `" --"`
(`0xe1336..0xe13b2`). The detail field uses text ID **3227** for a
terminated entry, **3228** otherwise (`0xe1494..0xe14ba`). The scroll
text uses ID **3223**, newline + the first location, two newlines +
ID **3224**, newline + the second location, then two newlines + text
ID **3174 + Wanted::getIndex()** (`0xe1566..0xe1670`). Its initial
literal at `0x1ca472` is a NUL byte, supporting the empty title/body
initialization. The prior source incorrectly used ID 3229 for the
detail field and initial body, and appended location/reward fields to
the scroll body instead of the localized description.

The corrected production body passes a source-extracted host test at
`-O0/-O2` for active, terminated, and inactive entries, including
station/system strings, rewards, detail text, and full scroll body.
Existing Wanted init and touch tests also pass. Native `gof2`,
**209/209** ARM translation units, and **95** verifier tests (one skip)
pass. Focused `selectWanted` source-shape/strict fuzzy rises from
**89.5%/49.6%** to **95.5%/57.7%**; it remains nonexact.

The full corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs, with **94.69%** mean source-shape and
**84.21%** mean strict fuzzy. The linked-exact and byte-exact sets
remain at **2,600** and **1,082**, respectively, with zero gains and
zero losses relative to the preceding `WantedWindow::init` report.
The full report is `_work/wanted_select_full_20261025.json` (not
committed).

The host test substitutes `String`, localization, world, and UI objects.
It does not prove live rendering or the validity of all indices in a
real save. `WantedWindow::draw` still dereferences four null-initialized
pointer slots and has unverified label construction; that is the next
UI recovery priority. The connected arm64-only Android 16 phone cannot
install the original 32-bit APK, so this screen was not tested there.

Focused reports: `_work/wanted_select_before_20261025.json` and
`_work/wanted_select_after_20261025.json` (not committed).
