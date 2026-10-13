# SpriteSystemDraw matrix queue audit (2026-10-13)

The original Android ARMv7 `libgof2hdaa.so` function at `0x86ddc`
calls `AEMath::operator*(view, world)` at `0x86e0c`, then constructs
the sprite vertices from `world`. On the material path, the disassembly
at `0x86f18`–`0x87034` appends the mesh, the 15-float `view` matrix,
the constant UV matrix with `m[5] = -1` and `m[12..14] = 1`, the
15-float `world` matrix, and color `0xffffffff` to the material queues.
Without a material it calls `MeshDraw` directly.

The local `SpriteSystemDraw` previously called two unresolved shims:
`AE_AEMath_matMul` with only one operand and
`AE_SpriteSystem_pushMatrix` with an integer-cast destination pointer.
It now calls the actual matrix product and appends typed matrices
through the existing cached-array helper. The product's return value
is unused in the original path as observed here; no downstream use is
invented. The direct-draw path and vertex-generation arithmetic are
unchanged.

The production-body host smoke checks null system, two particles with
individual and shared sizes, output quad positions, each material
queue, and the direct-draw branch. It uses fixture math/array/render
services, so it does not validate the original allocator, ARM32
runtime, OpenGL output, or a complete game frame. The wider
`ArrayAddCachedRaw` allocation/lifetime behavior remains unaudited.

Focused ARM results: source-shape 73.8% → 79.4%; strict fuzzy
18.3% → 11.2%; neither linked- nor byte-exact. The fuzzy decrease
is recorded rather than changing the backed queue semantics to fit
the metric. Native `gof2`, ARM 206/206 translation units, the host
smoke, and verifier unit tests (36 pass, one skipped) are green.

Full ARM corpus: 4,714 comparisons; 4,323/4,524 original functions
covered, 201 missing, zero skipped/wrong-type. Average source-shape
92.5407% → 92.5419%; strict fuzzy 80.8323% → 80.8308%.
Linked-exact remains 2,378 and byte-exact 1,008, with zero exact
symbol-set losses or gains. Focused reports are
`A:/Galaxy on Fire new version/_work/spritesystem_draw_before_20261013.json`
and `A:/Galaxy on Fire new version/_work/spritesystem_draw_after_20261013.json`;
the full report is
`A:/Galaxy on Fire new version/_work/spritesystem_draw_full_20261013.json`.

Next priority: audit the cached-array growth and matrix lifetime path
used by both `SpriteSystemDraw` and `MaterialDraw` before treating the
batch as runtime-safe. Current source-shape and green compile/smoke
results do not prove complete decompilation.

## Cached-array follow-up (2026-10-13)

The original `SpriteSystemDraw` at `0x86f18`–`0x87034` calls the three
existing `ArrayAddCached` specializations, rather than an inline
raw-header helper. The local clone used `ArrayAddCachedRaw`, which
duplicated the template and obscured the call contract. All five
queue appends now call the existing typed `ArrayAddCached` template;
the duplicate helper and its raw header cast were removed. The
specializations are declared `extern template` in this caller so
they remain defined only by `Array.cpp`, as the original out-of-line
ARM calls indicate. Without that declaration, Clang emitted three
extra local specializations and inflated the corpus comparison count.
The production-body host smoke now exercises the actual template and
checks preservation across the first capacity growth, plus automatic
destruction of its fixture arrays.

The original Android functions `Array<Matrix>::Array` at `0x77940`,
`ArrayAddCached<Matrix>` at `0x7eecc`, and
`Array<Matrix>::~Array` at `0x779aa` show `operator new[]`, `realloc`,
and `operator delete[]` on the same storage. The local array follows
that observed sequence. This allocator pairing is a portability and
sanitizer concern, not evidence that Android used a different path;
it is left unchanged rather than silently changing the recovered
behavior. The host smoke passes under the current MinGW allocator,
but does not prove allocator safety on every platform.

Focused `SpriteSystemDraw` source-shape 79.4% → 86.6% and strict fuzzy
11.2% → 22.7%; still not exact. The three unchanged helper
specializations score 90.2%/53.5% (Matrix), 98.6%/82.1% (Mesh*),
and 97.9%/84.2% (`unsigned int`) source-shape/strict fuzzy.
Native `gof2`, 206/206 ARM units, both sprite and MaterialDraw host
smokes, and 36 verifier tests pass (one skipped).

Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
covered, 201 missing, zero skipped/wrong-type. Average source-shape
92.5419% → 92.5434%; strict fuzzy 80.8308% → 80.8333%.
Linked-exact remains 2,378 and byte-exact remains 1,008; comparison
of the exact symbol sets shows no gains or losses. The intermediate
inline-instantiation trial was excluded from these final averages.
Focused final report:
`A:/Galaxy on Fire new version/_work/spritesystem_array_final_focus_20261013.json`;
full report:
`A:/Galaxy on Fire new version/_work/spritesystem_array_final_full_20261013.json`.

Next priority: audit `Material` construction/copy and multi-frame queue
lifetime with the real class, then address any confirmed ownership
divergence. The current smoke uses a fixture `Material` and has not
run under a sanitizer or in the original ARM32 process.
