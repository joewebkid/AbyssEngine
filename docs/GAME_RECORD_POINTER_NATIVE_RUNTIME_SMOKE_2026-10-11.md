# GameRecord native pointer runtime smoke — 2026-10-11

## Scope and evidence

The recent `GameRecord` native sidecars preserve 32-bit ARM layout while
holding full-width pointers in a native 64-bit build. To test that specific
property, `tools/game_record_pointer_smoke.cpp` constructs the real record,
sets 29 distinct high-bit pointer markers through its typed setters, and
checks every corresponding getter. Four adjacent scalar values are checked
for accidental overwrite. The smoke exits successfully (code 0).

Only the smoke build defines `GOF2_RECORD_POINTER_SMOKE`. That flag omits
`GameRecord::load`, whose unrelated dependencies prevent a small standalone
link. The smoke supplies minimal `String` constructor/destructor stubs.
Normal native and ARM builds compile the unmodified production `load` body.
The generated test executable on A: was removed after the run.

Reproduce from the repository root with the UCRT64 compiler:

```powershell
& 'C:/msys64/ucrt64/bin/g++.exe' -w -std=gnu++14 -O0 -fpermissive `
  -DGOF2_RECORD_POINTER_SMOKE -Isrc -Ithird_party/jni -Ithird_party/gl `
  -Ithird_party/fmod/inc -Ithird_party/libzip -Ithird_party/crypto `
  tools/game_record_pointer_smoke.cpp src/game/mission/GameRecord.cpp `
  -o 'A:/Galaxy on Fire new version/_work/game_record_pointer_smoke_20261011.exe'
& 'A:/Galaxy on Fire new version/_work/game_record_pointer_smoke_20261011.exe'
```

## Verification

- Native `gof2` static-library build: pass.
- ARM build: 206/206 translation units, zero failures.
- Verifier tests: 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons; 4,323/4,524 original functions
  compared; 201 missing; zero skipped or wrong-type symbols.
- Average source-shape: 92.24%; strict fuzzy: 80.43%; linked-exact: 2,353;
  byte-exact: 1,008.
- Per-symbol comparison against
  `A:/Galaxy on Fire new version/_work/native_link_duplicates_full_20261011.json`:
  zero linked-exact or byte-exact losses and zero source-shape changes.
  Current report:
  `A:/Galaxy on Fire new version/_work/record_pointer_runtime_smoke_full_20261011.json`.

## Remaining limit

The markers are never dereferenced. This checks storage width and adjacent
field integrity, not allocation ownership, `GameRecord::load`, file-format
round trips, or full game runtime behavior. A real save/reload remains the
next substantive validation task; the full native executable still has
unresolved dependencies, including absent FMOD binaries.
