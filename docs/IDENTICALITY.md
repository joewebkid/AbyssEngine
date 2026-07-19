# Binary identicality: gap analysis & roadmap

> This is a long-term byte-identical relink workbook. Current per-function
> snapshot metrics live in [PROJECT_STATUS.md](PROJECT_STATUS.md); the figures
> below include historical measurements and are not semantic-recovery percent.

Goal: produce a `libgof2hdaa.so` byte-identical to `_work/bins/android_2.0.16_libgof2hdaa.so`
(same sha256). This document measures the gap and tracks progress. Measured against the original
(2,187,328 bytes) with our stripped link output.

## Section-size deltas (stripped ours vs original)

| section | orig | ours | delta | meaning |
|---|---|---|---|---|
| **.text** | 1,259,464 | 696,268 | **−563,196** | ~1260 engine functions our decomp declares-but-doesn't-implement (local code in the original). THE dominant gap. |
| **.rodata** | 269,159 | 549,399 | **+280,240** | ours is 75% NULL bytes (~412KB) vs orig 12%; oversized/placeholder local const data. |
| .ARM.extab | 156,312 | 82,176 | −74,136 | fewer/smaller exception tables (follows missing code) |
| .dynsym | 89,248 | 107,808 | +18,560 | we import ~1063 more symbols (the unimplemented functions) |
| .dynstr | 189,208 | 233,088 | +43,880 | name strings for those extra imports |
| .data | 10,604 | 1,820 | −8,784 | missing real initialised data |

## The four real gaps (in priority/size order)

1. **~1260 unimplemented functions** (.text −563KB). Our decomp *calls* engine internals via
   free-function shims but does not define them: `Mat_assign`, `MatrixGetDir`, `VectorCross`,
   `Radio_ctor`, `PE_upd_post`, `Array_dtor`, `MeshCreate`, … They are local (non-exported) code in
   the original (only 10 of our 1270 imports are even in the original's dynsym). Implementing them all,
   byte-exact, is the bulk of finishing the decompile.
2. **~3800 functions not byte-exact.** Of the ~5400 we DO define, only 923 are byte-exact / 1636
   linked-exact (verify.py). The rest differ and must be driven to exact.
3. **.rodata 75%-null bloat** (~412KB). Local const data, oversized or zero-placeholder, that the
   original does not have. Source not yet pinned (exported OBJECTs match; it's anonymous const data).
4. **~9-35 extra dynsym symbols** + build-id. Bounded: 6 compiler ctor/dtor aliases, a `SHA256_version`
   libcrypto leak, a Camera C1, a JNI anchor. The GNU build-id is a content hash -> matches only once
   everything else does.

## Done
- **Pass 1: strip** the link output (`strip --strip-all`) -> section set matches the original
  (no .symtab/.strtab/.debug_*). Parity unaffected (35/0/0); verify/sodiff read .dynsym+.text.

## Reality
Same-hash == completing the decompile to byte-perfection: implement ~1260 functions, make ~5600
functions byte-exact, recover all real const data, and match the exact link layout. This is the
project's ultimate completion milestone -- a very large, multi-pass effort, not a quick fix. Progress
is tracked by: byte_exact/linked_exact function counts (verify) and the section-size deltas above.

## Next levers (tractable, bounded first)
- Pin the .rodata null source (largest zero runs -> owning TU) and fix oversize/placeholder const.
- Reduce the handful of extra dynsym symbols (restrict libcrypto exports; fold the ctor aliases).
- Byte-exactness campaign: verify.py worst-first, fix near-miss (80-99%) high-value functions first.
- Implement missing engine functions in batches (math/Vector/Matrix shims are the densest cluster).

## Concrete leads found (for the byte-exactness campaign)

The verify report (cmake-build-match/verify/report.json) has per-function rows: 870 byte_exact,
1569 linked_exact, 2446 size_match of 4507; pct buckets {100:1569, 90-99:231, 70-89:630, 50-69:645,
<50:1432}. 2061 functions size-MISMATCH (1980 game-ish -> need restructuring); 375 size-match >=80%
not-linked (the highest-value near-misses).

Systematic clusters (one cause fixes many):
1. **Shader `UpdateMeshData` cluster** (~15 fns at 86-94%, GenericShader/VertexColorShader/BumpShaderV2/4/
   CubeMapping/EnergyShield/...). Diff via direct disasm: the only real (non-relocation) deltas are
   field-offset drift. In `GenericShader`, fields from `uSpecularColor` onward are +4 in the original
   (orig 0x5c/0x60/0x68/.. vs ours 0x58/0x5c/0x64/..) -> our decomp is missing a 4-byte uniform-location
   field between uDiffuseColor and uSpecularColor (or has them mis-ordered). Plus a 1-byte Mesh field
   drift (orig [r4,#9] vs ours [r4,#10]). Fixing the shared base/layout flips the whole cluster.
   NOTE: this is layout drift that field_0xNN scanning can't see (semantic-named fields) -- a class of
   drift beyond [[drift-campaign]]. A generalized ASM-offset-diff drift detector would surface all of it.
2. **ParticleSettings_str[401401]={0}** -- 392KB zero array (the .rodata bloat), a mis-sized decomp
   artifact; the real particle-name string table is a few KB. Needs the original's data recovered
   (the `buf` results are dead in our decomp, so it's correctness-neutral but blocks .rodata identicality).

## PaintCanvas layout reconstruction (in progress — concrete data)

offdrift + per-method disasm give the original offsets (vs ours) for PaintCanvas fields:
- `engine`        orig 0x34  ours 0x3c   (GetWidth, SetWorldViewMatrix: `ldr [r0,#0x34]`)
- `meshCount`     orig 0x24  ours 0x30   (MeshSetUv: `ldr.w ip,[r0,#0x24]`)
- `meshes`        orig 0x28  ours 0x34   (MeshSetUv: `ldr [r0,#0x28]`)
Deltas: meshCount/meshes are +0xc (our struct has 12 extra bytes before 0x24); engine is +8 (so our
struct is ALSO missing 4 bytes between `meshes` and `engine` — orig has 2 fields there [0x2c,0x30],
we have 1 [`gameOrientation`]). Root: `::Array<AELoadedTexture*> cubeTextures` (12B, count/data_/
capacity_) is MIS-PLACED at 0x20 in ours; the original has it elsewhere. All early fields
(culledCount, quad2dMesh, field_0xc/14/1c, mask2dImage, gameOrientation, cubeTextures) ARE used, so
this is a RE-ORDER, not a deletion.

### Safe methodology for any struct (no-degradation gated)
1. Disassemble every method of the struct (ours+orig), align 1:1 (size-match), map each our-offset to
   its orig-offset; translate offsets->field names via the source. This yields the COMPLETE original
   field order (a full layout reconstructor tool over offdrift is the next tooling step).
2. Reorder/repad the header to match, atomically (the whole struct, not a few fields).
3. Rebuild + relink + verify: keep ONLY if byte_exact/linked_exact counts RISE and parity stays
   35/0/0; otherwise revert. The verify counts are the hard no-degradation gate.

This is the engine for the byte-exactness campaign; each reconstructed struct flips many functions.
Dozens of structs drift (offdrift delta histogram: -8:14, +4:12, +8:11, +32:9, ... across 300 fns).

## GROUND-TRUTH unblock: Ghidra has the original loaded

The original (android_2.0.16_libgof2hdaa.so, 8096 fns, image_base 0x10000) is OPEN in Ghidra (MCP).
`get_struct_layout <Struct>` returns the real field offsets; `decompile_function <addr>` shows real
field usage. This is the authoritative source for the byte-exactness campaign — reconstruct each
struct from Ghidra's layout, not from guesses. The verify byte_exact/linked_exact count remains the
no-degradation gate when applying.

### PaintCanvas — definitive layout (Ghidra get_struct_layout, size 524):
0x0 gravityRotationEnabled(=initialized), 0x4 culledCount, 0x8 field_0x8(=quad2dMesh), 0xc field_0xc,
0x10 field_0x10, 0x14 field_0x14, [0x18 gap], 0x1c field_0x1c, 0x20 field_0x20(int), 0x24
field_0x24(=meshCount), 0x28 field_0x28(=meshes), [0x2c gap=mask2dImage], 0x30 orientation
(=gameOrientation), 0x34 engine, 0x38.. matrices, ... 0x150 images, 0x164 cameras, ...
DECOMP BUG: `ChangeCubeTexture` (decompiled) reads cube count@field_0x10 / data@field_0x14 -> the
cube-texture Array IS field_0x10/0x14/0x18 (count/data/capacity @ 0x10). Our decomp instead made a
SEPARATE `cubeTextures` Array at 0x20, displacing field_0x20/meshCount/meshes by +0xc (root of the
~10 PaintCanvas near-misses, e.g. engine 0x3c-should-be-0x34). FIX: map `cubeTextures`->0x10 (merge
with field_0x10/0x14/0x18 uses), add the standalone `field_0x20` int, shift meshCount/meshes/
gameOrientation/engine back by 0xc. Verify-gate each step.
