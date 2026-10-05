# Main working-tree reconciliation (2026-10-05)

The uncommitted `main` checkout was inspected without changing it. The
committed feature branches were already combined in
`codex/integrate-decomp-20261005`; this follow-up is isolated in
`codex/integrate-main-working-20261005`.

The 68 non-overlapping tracked edits from `main` were transferred, together
with 23 new research documents, validation inputs and focused tools. They
cover AbyssEngine math/render/shader source recovery, HangarWindow, Radar,
ParticleSettings, application-module ABI layout, and related checks. The
existing ELF-declared function-size bound in `delink.py` was retained while
adding the tested PC-referenced final zero-literal handling from `main`.

Overlapping `Player.cpp/.h` changes were not replayed: the later Player branch
already contains their typed return, real sound globals and matrix-wrapper
behavior. The older `StarMap.cpp` working version introduced unresolved
pointer placeholders and was not substituted for the later source-backed
StarMap branch. The section-local alias correction in `asmdiff.py` was already
present. The useful validation guidance was merged into `VALIDATION.md`, while
the old status snapshot was summarized here rather than replacing newer
figures. No tracked `_work` deletions, old StarMap-document deletion or raw
local experiment reports were imported.

Validation: UCRT64 `gof2` builds; 206/206 ARM translation units compile;
verifier tests pass (36, one optional object-dependent skip); the native
ABI/lifetime and ParticleSettings smoke tests pass. Focused Player, particle
control and Mesh smoke tests pass. The full ARM corpus compares
**4,323/4,524** originals across **4,688** rows, with 201 absent, zero
wrong-type misses and zero skipped units. Mean source-shape is **91.75%** and
strict fuzzy **79.62%**. There are **2,300 linked-exact** and **997 byte-exact**
pairs: no prior exact pair was lost, with 101 linked and eight byte gains over
the preceding integrated snapshot. Reports are local, uncommitted files at
`_work/integrated_full_gun_20261005.json` and
`_work/integrated_main_working_20261005.json`.

These are comparison and compile results, not an integrated native gameplay
runtime pass. The dirty `main` checkout and GitHub remote remain untouched.
