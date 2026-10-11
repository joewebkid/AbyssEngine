# Native link duplicate audit (2026-10-11)

## Link evidence and change

The project currently builds `gof2` as a static library, not a full
native executable. A minimal probe that references
`RecordHandler::recordStoreRead` and links against that library exposed
seven multiple definitions: `Matrix::operator*=` and `operator=`,
`Vector::operator*=`, `operator-=`, `operator/=`, and `operator=`, plus
`ApplicationManager::gAppManager`. The math operators exist in both
`AEMath.cpp` and the individual `Matrix.cpp`/`Vector.cpp` units;
the global exists in both `Globals.cpp` and `ApplicationManager.cpp`.

For native 64-bit builds, the operators in `AEMath.cpp` and the global
in `ApplicationManager.cpp` are the single definitions. The duplicate
definitions in the other translation units remain compiled on 32-bit
ARM so the validated ARM corpus is unchanged. The probe now reports
zero multiple definitions. This does not make the complete executable
linkable: with the duplicate check passed and libcrypto provided, the
probe still reports thousands of unresolved reference occurrences,
including unimplemented UI/game helpers and FMOD APIs. The FMOD
directory contains headers but no native binaries by project policy.

## Verification

- Native `gof2` static-library build passed.
- A standalone `tools/native_math_link_smoke.cpp` linked the three
  math units with section garbage collection and ran with exit code 0.
  It checks the selected native vector and matrix assignment/operators.
- ARM object build: 206/206 translation units, no errors.
- Verifier tests: 36 run, 35 passed, one skipped.
- Full ARM corpus: 4,713 comparisons, 4,323/4,524 original functions,
  201 missing, 92.24% average source-shape, 80.43% strict fuzzy,
  2,353 linked-exact, 1,008 byte-exact; per-symbol comparison found
  zero exact-pair losses, skipped units, or wrong-type symbols.

The full report is at
`A:/Galaxy on Fire new version/_work/native_link_duplicates_full_20261011.json`.
The temporary compiled smoke executable was deleted after the test;
existing C: build directories were reused.

## Limits and next priority

No record save/reload runtime path was exercised. The linker probe is
diagnostic, not a playable executable, and the remaining missing
symbols cannot be treated as a single small fix. Next, build an
isolated reader/writer round-trip test with controlled file and game
dependencies, using section-level linking or a focused source subset;
separately recover or integrate the missing native runtime services
before attempting a full game executable.
