# ModStation entry snapshot recovery (2026-10-05)

Scope: `ModStation::enterStation()` on local branch
`codex/blueprint-isunlocked-arm-20261002`. Primary evidence is Android 2.0.16
`libgof2hdaa.so:_ZN10ModStation12enterStationEv` at `0x0d7200..0x0d72ac`.

The original loads `Globals::status` through GOT `0x21015c`, calls
`Status::getStation()` and `Status::departStation(Station*)`, visits the
station, and applies new medals through `Globals::achievements` (GOT
`0x210214`). It then calls `Status::getShip()` three times through the
function GOT at `0x210240` to fetch equipment sorts 10 and 9 and the ship
index. It stores those indices, or `-1` for absent equipment, at Status
`+0x150`, `+0x154`, and `+0x158`, then clears `+0x124` and `+0x11c`.

The recovered source had a null `getShip` function pointer and used a
redundant pointer-to-member cast for `departStation`. The function now calls
the already declared methods directly, uses the original `Globals` routes,
and preserves the observed call and store order. No new wrapper or ABI
shim was needed.

Focused ARM verification changes from **46.1% source-shape / 38.6% strict**
to **100% / 100% linked-exact**; raw byte-exact is false because calls and
globals carry relocations. All 206 ARM translation units compile and the
UCRT64 `gof2` static library builds. Focused reports are
`_work/blueprint-isunlocked/modstation_enterstation_{before,after}.json`.

The full `--fail-on-wrong-type` corpus compares 4,322/4,524 original
functions across 4,675 rows, with 202 absent, zero wrong-signature and zero
skipped. Mean source-shape rises from 90.13% to 90.14%, strict fuzzy from
77.27% to 77.28%, and linked-exact from 2,165 to 2,166; byte-exact stays
at 977. Exact `(unit, symbol)` comparison finds the one linked gain in
`enterStation` and zero losses of either kind. The complete report is
`_work/blueprint-isunlocked/modstation_enterstation_full.json`.

Linked-exact instruction bytes do not establish that the entry flow or
medal state was exercised in a running game. That integrated behavior
remains unverified; the next source-recovery priority is the separate
`ModStation::checkPendingProducts()` path, whose current body still contains
fragment helpers.
