# Contributing

Thanks for helping with the reconstruction. The useful unit of work here is a
small, reviewable claim that can be traced to evidence.

## Before You Start

Read `README.md`, `AGENTS.md`, and `docs/PROJECT_STATUS.md`. Search for prior
work before implementing a second version of the same function or layout.

Useful contributions include signatures, layouts, constructors, vtables,
decompiler-to-source reconciliation, verification tooling, and documentation
that separates facts from hypotheses.

## Evidence Required

For a behavior or layout change, state in the commit or pull request:

1. The symbol, class, and changed files.
2. Primary evidence: binary version/address, named decompiler dump, iOS
   cross-reference, Java reference, or symbol map.
3. What is confirmed and what still needs confirmation.
4. Validation performed and its outcome.

Java can explain intent, but cannot establish Android ARM layout or byte shape
by itself.

## Validation

- Run the native `gof2` build after C++ work when the toolchain is available.
- Run focused ARM verification when a patch targets matching.
- Do not claim ARM improvement from a native x64 build.
- Keep generated build output out of commits.

See [docs/VALIDATION.md](docs/VALIDATION.md) for commands and report fields.

## Patch Shape

- Keep one subsystem or mechanical repair per change.
- Do not reformat unrelated recovered files.
- Do not remove a shim until its direct replacement has the known call contract.
- Use the evidence labels defined in `AGENTS.md`.
- Update public status documentation with newly confirmed evidence.

## Prohibited Content

Do not add original game binaries, assets, APKs, IPAs, OBBs, JARs, decrypted
content, credentials, or large generated extraction output.
