# Host asset bridge and next recovery stages (2026-10-17)

## What is implemented

`host/ReadOnlyAssetFileInterface` is a reusable, native-only
`FileInterface` implementation. A logical prefix such as `data/bin/`
can be mounted on a directory of loose resources without changing
Android production source or the ARM matching build. The bridge reads
regular files, supports relative skipping and reports exact-read
success according to the Android interface contract. It closes each
stream once. It rejects duplicate mounts, path traversal, prefix
spoofing and resolved symlinks outside the mounted directory.

The existing production-stack smoke now uses this component rather
than its private four-name file shim. Against the local Android 2.0.16
assets, it reads `weapons_hd.bin`, `items.bin`, `ships.bin`,
`stations.bin`, `systems.bin`, `agents.bin` and `wanted.bin`, including
all 233 items, 64 ships, 135 station IDs, 34 systems, 27 agents,
25 wanted entries and direct station reads 70/91. Each
station is found in its declared system. The per-system station
loader is also tested for all 34 systems. Missing-asset and
path-rejection checks pass. The tested production `Status::setStation`
transition brings the total to forty-seven opens and closes at both
host `-O0` and `-O2`.
The native `gof2` and `gof2_host_file` targets build. Since `src/`
and the ARM-target definition did not change, the last full ARM report
remains the baseline: 4,323/4,524 originals, 93.76% source-shape,
82.91% strict fuzzy, 2,521 linked-exact and 1,076 byte-exact.

This is **not** an executable game host. Writes, enumeration, ZIP/PAK,
save directories, rendering, audio, module lifecycle and input are not
implemented. The runtime currently assumes local asset directories
are not concurrently mutated; canonical path checks are not an
atomic defense against a hostile symlink swap. Physical paths with
non-ASCII characters are not yet covered by a Windows runtime test.

## Completion stages

1. **Inventory and evidence** — map the original ARM symbols and
   compare Android with available iOS/PC code and assets. Completed
   enough for targeted recovery, but evidence gaps remain.
2. **Source and semantic recovery (current)** — restore bounded
   functions and ABI/lifetimes, prove behavior with focused tests,
   keep native and ARM builds green, and prevent exact-match losses.
   A source-shape percentage is evidence, not overall completion.
3. **Executable host (next integration milestone)** — connect real
   resource roots and minimal platform services, start one station
   and reach a controlled flight scene. Log exactly which original
   code runs and which boundaries remain stubs. The read-only loose
   asset bridge is the first reusable service for this stage.
4. **Gameplay coverage** — exercise save/load, missions, combat,
   economy, menus and transitions with recorded scenarios; replace
   stubs only from evidence and fix actual behavior gaps.
5. **Match and portability closure** — use REA/ARM comparison and
   cross-platform evidence for remaining source-shape/exact gaps,
   repeat full corpus and runtime regressions, and document any
   compiler ceiling or unverified behavior. No single metric alone
   declares decompilation complete.

Production `createAgents` and mission creation now pass a bounded
station-transition host gate, with the wall-clock RNG seam documented.
The next package is a small native executable that mounts the required resource roots
and run a station-to-flight scenario. Before presenting it as playable,
it must report the first unimplemented platform call instead of
silently faking it.
