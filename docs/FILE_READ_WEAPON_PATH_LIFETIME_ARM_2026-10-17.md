# Weapon-position path lifetime (2026-10-17)

Android 2.0.16 ARM `FileRead::loadWeaponPositions(int)` at
`0x121302..0x12131e` constructs a temporary `String` for
`data/bin/weapons_hd.bin`, calls `AEFile::FileExist`, destroys the
string at `0x12130e`, branches on the result, and only then calls
`AEFile::OpenRead` on the present-file path. The recovered
function-scoped `String path` instead survived until return. Passing
a temporary `String` to `FileExist` restores the observed lifetime
without changing the parser or adding a missing-file policy.

Focused ARM source-shape/strict fuzzy improves from **83.1%/36.0%**
to **97.7%/53.1%**. This function remains nonexact. Native `gof2`,
**208/208** ARM translation units and 40 verifier tests (one existing
skip) pass. The focused report is at
`A:/Galaxy on Fire new version/_work/filread_weapon_positions_lifetime_after_20261017.json`.

The full ARM corpus compares **4,323/4,524** original functions in
**4,721** comparisons, with **201 missing** and **123 extras**.
Rounded average source-shape/strict fuzzy is **93.74%/82.88%**.
The **2,518 linked-exact** and **1,076 byte-exact** `(unit,symbol)`
sets are unchanged from the preceding CutScene FileRead lifetime
report: zero lost and zero gained exact pairs. The full report is at
`A:/Galaxy on Fire new version/_work/filread_weapon_positions_lifetime_full_20261017.json`.

The record parser and four-slot layout were checked against the same
ARM function: type 3 reads three extra floats and stores its second
vector as `(extraX, extraZ, extraY)`, matching the current source.
The original loop has no end-of-file guard before finding the requested
ship ID, and the original missing-file branch returns null. Neither
path is a license to invent a guard in the reconstructed source.
The shipped `weapons_hd.bin` data and both caller paths still need
runtime validation; ship IDs absent from the file need a separate
call-path audit.
