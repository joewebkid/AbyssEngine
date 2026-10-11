# Dialogue sound mapping — Android ARM (2026-10-11)

## Evidence and correction

Android 2.0.16 `Globals::getDialogueSoundId(int, Agent *)` at ELF address
`0xe5818` searches 1,504 code/sound pairs beginning at `.rodata`
`0x1feb20`. Its loop accesses 3,008 32-bit words. The recovered source
had a one-element zero table but kept the full loop, so almost every
lookup read out of bounds before reaching the agent-specific fallback.

`DialogueSoundPairs.inc` now contains all 3,008 words from the original
ELF, in original order. The table's 12,032 bytes have SHA-256
`24201ffc5c374c33aeec4660c20bd8af5788878246a0e6f0b1cae22c09895b52`.
The compiled ARM `Globals.o` emits `_ZL14gGDS_pairTable` as a 0x2f00-byte
`.rodata` object; its bytes have the same hash. The 1,504 keys are unique.
The fallback switch and caller interfaces were not changed. Callers are
`DialogueWindow` and `Radio`.

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units.
- Verifier tests: 37 run, 36 passed, one skipped. The new
  `test_dialogue_sound_pairs.py` guards table length, unique keys, and hash.
- Focused function: source-shape 96.1% to 97.2%; strict fuzzy 39.5% to
  41.5%. The 49-function `Globals` unit retains ten linked-exact and one
  byte-exact matches.
- Full corpus: 4,713 comparisons, 4,323/4,524 original functions
  compared, 201 missing, zero skipped or wrong-type symbols; 92.32%
  source-shape, 80.47% strict fuzzy, 2,353 linked-exact, 1,008 byte-exact.
  Per-symbol comparison against the previous full report found no exact
  losses. Report:
  `A:/Galaxy on Fire new version/_work/dialogue_sound_pairs_full_20261011.json`.

## Remaining limit and next priority

This proves the lookup data and ARM shape, not live audio playback or all
fallback behavior. The native executable still cannot link fully with
the currently available FMOD dependencies. Next audit an independent
native global placeholder such as the random source in
`Globals::getRandomPlanetName`, then validate its callers separately.
