# Globals music selection — Android ARM (2026-10-11)

## Evidence and correction

Android 2.0.16 `Globals::playMusicAndFadeOutCurrent(int)` at ELF address
`0xe55cc` routes modes 0, 1, and 2 through `Globals::sound`. It calls
`FModSound::stop` with the current music event (the sound object's first
word), then selects a track by station, campaign stage, orbit, or race.
The original `.rodata` at `0x1fb858` contains the mode-0 race tracks
`0x80, 0x7f, 0x81, 0x82`; at `0x1fb868` it contains the mode-1 tracks
`0x86, 0x8b, 0x8a, 0x89`.

The recovered source previously dereferenced four translation-unit-local
null sound placeholders, passed zero instead of the current music event
to `stop`, truncated `Station *` to `int` for an unresolved fake helper,
and indexed one-element zero tables as if they held all race tracks.
It now uses the live sound object, typed `Station::getIndex()`, and the
ARM-confirmed tables. The original mode and special-track branch order is
retained. A compile-time assertion checks that both tables have four
entries. Callers include the game, menus, station module, and touch UI.

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier suite: 36 run, 35 passed, one skipped.
- Focused `playMusicAndFadeOutCurrent`: source-shape 86.9% to 93.1%; strict
  fuzzy 55.9% to 70.4%. Its 49-function `Globals` unit keeps ten
  linked-exact and one byte-exact matches.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; 92.32%
  source-shape, 80.47% strict fuzzy, 2,353 linked-exact, 1,008 byte-exact.
  Per-symbol comparison against the prior full report found zero exact-pair
  losses. Report: `A:/Galaxy on Fire new version/_work/globals_music_full_20261011.json`.

## Remaining limit and next priority

ARM shape is not a live audio test. The native target is a static library;
full executable linking still lacks dependencies, including FMOD binaries.
Some callers also retain their own unresolved global placeholders, so not
every route into this function is runnable yet. Next audit the independent
`Globals::getDialogueSoundId` pair table, which is currently a one-entry
placeholder indexed as a much larger original table.
