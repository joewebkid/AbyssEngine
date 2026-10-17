# Level stage-0 host allocations (2026-10-17)

Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
`Level::init` at `0xadf94` allocates `LODManager` with 0x14 bytes and ten
`ParticleSystemManager` objects with 0x64 bytes each. The source previously
carried those ARM32 constants directly into the native host build. Both
types contain pointers, so their Windows x64 layout is larger. Constructing
them in those undersized blocks risks writing past the allocation before
`Level::createSpace` is reached.

The eleven stage-0 allocations now use `sizeof` of the corresponding type.
The ARM32 class sizes still resolve to the original constants; focused
`Level::init` source-shape/strict fuzzy is unchanged at 84.8%/52.7%,
nonexact. Focused reports:
`A:/Galaxy on Fire new version/_work/level_stage0_alloc_before_20261017.json`
and `level_stage0_alloc_after_20261017.json`.

The source-extracted stage-0 allocation block runs at `-O0` and `-O2` with
production `LODManager` and `ParticleSystemManager` constructors. It checks
all ten resource IDs, the system-index 12 dust variant, initialized fields,
and 16 guard bytes after every host allocation. Native `gof2` and all
208 ARM translation units build. This is a host memory-safety gate, not a
claim that the full `Level::init` or game resources run yet.

The full ARM report
`A:/Galaxy on Fire new version/_work/level_stage0_alloc_full_20261017.json`
compares 4,323/4,524 originals in 4,730 rows, with 201 missing, zero
skipped and zero wrong-type. Mean source-shape/strict fuzzy remains
93.86%/83.04%. Linked-exact/byte-exact remains 2,528/1,076, with zero
pair losses against `level_skybox_full_20261017.json`. The verifier unit
suite passes 40 tests (one existing skip).

Next: join the stage-0 constructors to a resource-backed `createSpace`
fixture, then reach `createPlayer` and stage 1. Other fixed ARM allocations
in later Level paths require separate review when those paths are entered;
`createPlayer` immediately uses fixed 0x114/0x3a0 allocations for `Player`
and `PlayerEgo` plus a raw `Player+0x69` store. Those must be checked for
host layout and runtime semantics before claiming the full entry path.
