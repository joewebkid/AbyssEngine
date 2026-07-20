# AGENTS.md

## Purpose

This repository reconstructs the Android ARMv7 version of *Galaxy on Fire 2
HD* and its Abyss Engine runtime. Treat it as reverse-engineering research, not
as a greenfield rewrite.

## Read First

1. `README.md`
2. `docs/PROJECT_STATUS.md`
3. `CONTRIBUTING.md`
4. The target header, implementation, call sites, and subsystem documentation.

Do not bulk-read or reformat the tree just to establish context.

## Evidence Rules

- `source-backed`: tied to an identified native, iOS, Java, symbol, or
  disassembly source.
- `confirmed`: cross-checked by independent evidence or a matching result.
- `needs confirmation`: plausible but not sufficiently evidenced.
- `heuristic`: practical approximation with no claim of original behavior.

Never call a guessed field name or placeholder replacement `source-backed`.
Never turn a native build success into a byte-match claim.

## Preferred Sources

1. Android ARMv7 `libgof2hdaa.so`: signatures, layout, instructions, and
   primary behavior reference.
2. Android/iOS decompiler output: control flow, strings, and cross-checks.
3. J2ME/Java lineage: intent and algorithms only, never byte-match proof.
4. Existing local evidence and upstream history: leads, not automatic proof.

The original binary belongs only in ignored `_work/bins/`; do not add it or
other original game data to Git. The optional DeepOpen J2ME comparison source
lives outside this repository in the shared `../references/DeepOpen/` workspace
directory and is not precedent for adding assets or dumps here.

## Editing Rules

- Preserve 32-bit layout intent; do not casually use host-pointer-sized types.
- Type a `void*` only when the field and ownership are evidenced.
- Keep changes within one subsystem and avoid unrelated formatting.
- Comment non-obvious source anchors and unresolved uncertainty briefly.
- Update `docs/PROJECT_STATUS.md` or a technical note when confirmed status,
  metrics, or risk changes.

## Validation

Native regression build on Windows/MSYS2:

```powershell
$env:PATH = 'C:\msys64\ucrt64\bin;C:\msys64\usr\bin;' + $env:PATH
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

Read `docs/VALIDATION.md` before ARM work. Record before/after focused verifier
results for a source-shape or byte-matching change.

## Git Safety

- Do not commit `_work/bins/`, build output, `.cache/`, APK/IPA/OBB/JAR files,
  extracted assets, external J2ME reference trees, or credentials.
- Stage only files related to the recovery package.
- Keep commits small and describe recovered behavior without inflated progress
  claims.
- Do not rewrite public history without explicit maintainer instruction.
