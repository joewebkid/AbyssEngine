# Random-name loader and composition (2026-10-11)

Android 2.0.16 ARM `Globals::getRandomName` at `0xe40b4` loads first and
last name lists with `FileRead::loadNamesBinary(kind, both, true/false)`,
draws each index with `Globals::rnd->nextInt(list size)`, copies both
strings before releasing the lists, and returns `first + " " + last` only
when the last string is nonempty. The recovered version dereferenced null
RNG/stack-guard placeholders, used unbounded draws, and tested the first
string for the join. These behavior and ownership paths are restored.

ARM `FileRead::loadNamesBinary` at `0x120dd0` selects 13 distinct
`data/bin/names_*.bin` files across races 0–8. Races 3 and 8 randomly
select Terran or Nivelian files for each half; Cyborg and Grey have no
second-half file. The previous path mapping was invented and its mixed-race
RNG pointers were null. The recovered branch layout now follows the ARM
switch. All 13 unique filenames were cross-checked against the extracted
Java reference resource set, which has matching basenames under `data/txt`.
That corroborates names, not the Android runtime path or binary contents.

Focused ARM verification builds **206/206** translation units.
`getRandomName` rises from **85.7%/59.4%** to **95.6%/69.8%**
source-shape/strict fuzzy; `loadNamesBinary` rises from **80.4%/59.4%**
to **84.3%/77.3%**. Neither is exact. Native `gof2` and 36 verifier
tests pass (one skipped). Full-corpus results are in `PROJECT_STATUS.md`.

No game session or on-device asset read was exercised. A subsequent audit
restored `NewsTicker::replaceTokens` and its ARM random-call order; those
asset-dependent paths still need controlled runtime checks.
