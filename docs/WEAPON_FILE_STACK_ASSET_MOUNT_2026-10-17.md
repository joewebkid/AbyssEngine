# Weapon-position asset through the native file stack (2026-10-17)

The local Android 2.0.16 `weapons_hd.bin` (7,484 bytes; SHA-256
`eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82`)
was read through the unchanged production `AEFile.cpp`, `AENormalFile.cpp`,
`AEPakFile.cpp`, and the unchanged extracted body of
`FileRead::loadWeaponPositions`. A small host `FileInterface` fixture maps
only `data/bin/weapons_hd.bin` to the local asset directory; it does not
provide a general game filesystem, PAK support, saves or a platform host.
The fixture was later extended to `items.bin` and `ships.bin`; see
[the catalog check](CORE_CATALOG_ASSET_FILE_STACK_2026-10-17.md).
It was then extended to `stations.bin`; see
[the station check](FILE_READ_LOAD_STATION_OWNERSHIP_ARM_2026-10-17.md).
The String locale, float-formatting and print services are inert host
boundaries; the String concatenation used by AEFile matches the production
five-line implementation.

At both host `-O0` and `-O2`, the test confirms the missing-asset null
result without an open, then reads ship 63 (first record) and ship 1
(later record), checks four slots and selected coordinates, releases
the arrays, and observes two opens with two closes. The independent
focused ARM run compiles **208/208** units and keeps
`loadWeaponPositions` at **97.7% source-shape / 53.1% strict fuzzy**,
nonexact. The native `gof2` static-library target remains green.

No production code changed. Thus the preceding full-corpus report remains
authoritative: **4,323/4,524** originals, **4,723** comparisons,
**93.75%/82.89%** source-shape/strict fuzzy and **2,520 linked-exact /
1,076 byte-exact**. This is stronger integration evidence for the file
stack, not evidence of a running game. The next host milestone must mount
the wider original asset tree and drive a real station-to-flight path;
the current fixture cannot establish that. The loader's behavior for
ship IDs absent from this asset remains untested and has no invented
fallback policy.

Reproduce locally (the asset is not committed):

```powershell
python tools/run_weapon_positions_file_stack_smoke.py `
  --asset-dir 'A:/Galaxy on Fire new version/analysis/gof2_raw_bin' `
  --out 'A:/Galaxy on Fire new version/_work/weapon-file-stack-smoke' `
  --cxx 'C:/msys64/ucrt64/bin/g++.exe'
```
