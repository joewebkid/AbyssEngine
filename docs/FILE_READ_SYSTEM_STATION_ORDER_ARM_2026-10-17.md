# Per-system station-loader ordering (2026-10-17)

## Evidence

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`FileRead::loadStationsBinary(SolarSystem*)` at `0x12014c`:
REA/IDA shows an outer station ID scan from 0 to 134. For each ID,
it walks the system's input ID array and appends a newly constructed
station at a monotonically increasing output position when the IDs
match. It does not write to the input array's index. The recovered
C++ body follows that order. The ARM function is not exact in the
latest corpus: 93.7% source-shape, 56.2% strict fuzzy.

Independent local Android `systems.bin` decoding shows one system
whose station IDs are nonascending: system 20, K'ontrr, contains
`62, 59, 58, 63, 64`. Its loader result is instead
`58, 59, 62, 63, 64`. The host test compiles the unchanged production
body and verifies ascending returned IDs for all 34 systems at both
`-O0` and `-O2`; 42/42 file opens/closes balance across five assets.
The test does not modify the source or the asset.

## Interpretation and next check

This is an original binary/data ordering property. Android ARM
`StarSystem::StarSystem(int)` at `0x133f06` reads the input station ID
by position to compare it with the current station. At `0x133f40`
and `0x133f8e`, it reads the station object's texture from the
loader result at that same position. Thus the first three K'ontrr
positions structurally pair different IDs and textures in the
original code. At `0x134026`, the constructor obtains the station
index from the loaded object for its own `stationIdxArray`. The
on-screen consequence has **not** been observed in a running game;
we make no stronger claim. No speculative reorder was made in the
matching source. If a host workaround is ever needed, keep it outside
the ARM-matched loader and document the behavioral difference.

No ARM source changed; the latest full corpus remains 4,323/4,524
original functions compared, 93.76% source-shape, 82.91% strict
fuzzy, 2,521 linked-exact and 1,076 byte-exact.
