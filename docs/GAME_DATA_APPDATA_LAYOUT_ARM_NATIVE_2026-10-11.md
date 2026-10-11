# GameData application-data layout (2026-10-11)

Scope: the application-data bytes used by the Android frame bridge,
`MenuTouchWindow`, `HangarWindow` and `MGame`. The old menu code cast the
returned `GameData *` to short, padded byte views or indexed literal ARM
offsets. That works only while pointers and virtual `String` objects have
the original 32-bit size; on a 64-bit host the DLC flags then overlap
preceding `String` storage.

Primary evidence is Android 2.0.16 `libgof2hdaa.so`:

- `OnCreateApplication` at `0x000d0738` allocates **0xc8** bytes for
  `GameData` at `0xd0752`. It initializes the request/result words at
  `+0x3c/+0x40`, purchase code at `+0x48`, and acknowledgement byte at
  `+0x4c` (`0xd077a..0xd0786`). Stores at `0xd078c..0xd0798` confirm
  the packed late fields `+0x6d/+0x71/+0x75/+0x77`; the old ordinary
  `int32_t` members incorrectly aligned that group and the following
  strings.
- `ndk23_newrender` at `0x000941d8..0x00094252` converts request byte
  `+0x3d` into ready byte `+0x40` and clears the first pending flag among
  `+0x3c/+0x3e/+0x3f/+0xa0..+0xa3` on other paths.
- The existing menu ARM evidence identifies input gates `+0x05/+0x0c`,
  DLC request/result bytes `+0x3c..+0x42`, purchase code `+0x48`, and
  acknowledgement byte `+0x4c`. The original `ModStation::OnUpdate` at
  `0x000d9e04..0x000d9e30` also distinguishes purchase fields
  `+0x54/+0x58/+0x60`; that separate path is **not** recovered here.

`GameData` now exposes typed byte aliases for those flags and a packed
late-field group. The JNI, menu, hangar and `MGame` consumers use the
real object rather than a fixed-offset shadow. ARM32 assertions cover
size **0xc8** and key offsets; a host assertion checks that the DLC
request field lies after the wider `String` storage. The old unresolved
application-data fragment getters remain unresolved and are not claimed
as runtime working.

Focused `ndk23_newrender` improves from **83.9% to 98.8%** source-shape
and **70.5% to 96.5%** strict fuzzy; `OnCreateApplication` improves from
95.4%/67.3% to 95.9%/69.7%. `MenuTouchWindow::callDlcMenu` remains
linked-exact. Native `gof2`, 206/206 ARM translation units and 36 verifier
tests (one optional skip) pass. The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_game_data_20261011.json`:
4,323/4,524 originals, 4,689 rows, **91.89%** mean source-shape,
**79.97%** strict fuzzy, **2,319 linked-exact** and **997 byte-exact**.
No comparison row or exact pair was lost; `GameData`'s constructor and
destructor gained the two linked-exact matches. Missing remains 201,
wrong-type zero, skipped zero, and extra 112.

An executable whole-game DLC/store path is still unverified: Java billing,
several fragment getters and other application-data fields remain open.
Next audit `ModStation::OnUpdate`'s purchase/restore view and pointer
ownership at `GameData+0x54/+0x58/+0x60`, then build a representative
native request-flow smoke when the getter calls are resolved.
The purchase-result consumer has since been repaired; see the
[follow-up audit](MODSTATION_PURCHASE_RESULT_ARM_NATIVE_2026-10-11.md).
