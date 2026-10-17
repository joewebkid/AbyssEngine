# FileRead::loadStation temporary-ID ownership (2026-10-17)

Primary evidence: Android 2.0.16 `libgof2hdaa.so`, function
`_ZN8FileRead11loadStationEi` at `0x11f948`. ARM instructions
`0x11f94c..0x11f95a` allocate a two-byte `new[]` buffer and store
the truncated station ID; `0x11f95c` passes it to the selected-ID
loader; `0x11f960..0x11f962` fetch the first station pointer; and
`0x11f964..0x11f966` delete the ID buffer before returning. There
is no deletion of the returned `Array<Station *>` container in this
function. This matches the recovered call contract, including its
ownership limitation; it is not a claim that the original was leak-free.

The previous C++ used a stack `int16_t` and deleted the returned array.
It now uses `new int16_t[1]`, calls the same loader, returns its first
station and deletes only the temporary ID buffer. Positive callers
including `Galaxy::getStation` receive the same station pointer. A
missing `stations.bin` still has no safe return path in the original;
no fallback is invented here.

Removing the array deletion would also suppress the existing
FileRead-local exact `Array<Station *>::~Array` template symbol. An
explicit instantiation retains that original helper body without
calling it from `loadStation`; focused ARM checks both symbols as
linked-exact. The first intermediate full run exposed this one lost
pair, prompting the explicit instantiation before final validation.
The explicit instantiation also emits a `D1` destructor alias absent
from the original, so the extra-symbol count rises by one. This is
documented rather than hidden or fixed through an unrelated behavior
change.

Focused ARM comparison of `loadStation` changes from **57.5%
source-shape / 14.3% strict fuzzy**, nonexact, to **100% / 100% and
linked-exact** (not byte-exact). The host file-stack smoke reads the
local `stations.bin` at `-O0/-O2`: all 135 selected station records,
UTF-8 conversion of `Nehébru`, and direct `loadStation(70)` / `(91)`
for the Supernova/Valkyrie starting stations pass. Its host
`Station` destructor is a test boundary; the whole game is not run.

The asset stays outside Git (3,408 bytes; SHA-256
`761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3`).
The native `gof2` build, 208/208 ARM units and 40 verifier tests
(one existing skip) pass. The final full ARM corpus compares
**4,323/4,524** originals over **4,723** comparisons, with **201
missing** and **124 extras**. Average source-shape/strict fuzzy rises
**93.75%/82.89% → 93.76%/82.91%**; linked-exact rises **2,520 →
2,521**, while byte-exact stays **1,076**. Exact `(unit, symbol)` set
comparison against `mtw_dlc_preset_full_20261017.json` finds zero
linked/byte losses. The final local report is
`A:/Galaxy on Fire new version/_work/station_lifetime_full_final_20261017.json`.
