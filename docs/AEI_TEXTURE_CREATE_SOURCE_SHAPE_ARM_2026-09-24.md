# TextureCreate Family ARM Source-Shape Pass

Date: 2026-09-24

## Scope and evidence

The Android ARM32 symbol table and `analysis/gof2_libgof2hdaa_full_ida.c`
confirm five functions in this family:

| Function | Android address | Focused source-shape | Strict fuzzy | Target/base instructions |
| --- | ---: | ---: | ---: | ---: |
| `AbyssEngine::TextureCreateFromFileIntern` | `0x6f7f4` | 92.2% | 50.3% | 713/674 |
| callback `PaintCanvas::TextureCreate` | `0x79a34` | 95.5% | 51.7% | 61/59 |
| `PaintCanvas::TextureCreateGlobal` | `0x799b8` | 100.0% | 100.0% | 44/44 |
| `AbyssEngine::TextureCreateFromFile` | `0x6f7cc` | 100.0% | 100.0% | 13/13 |
| callback-free `PaintCanvas::TextureCreate` | `0x79b00` | 100.0% | 100.0% | 7/7 |

`TextureCreateGlobal` is included because Android call sites and its body show
that it loads and binds a global texture through `TextureCreateFromFile`. No
restore or cube-loader variant was added to the family.

## Accepted recovery

`TextureCreateGlobal` now passes the generated texture id to `glBindTexture`;
the previous code always bound id `0`. The fake global canary shim and the
decompiler's apparent second array slot were removed; the handle is an ordinary
scalar passed by address. The public `void` return type stays unchanged: the
decompiler's stack-guard expression is not treated as recovered return
semantics.

In `TextureCreateFromFileIntern`, raw formats 1–3 share the mipmap block, as in
the Android `LABEL_49` control flow. Unsigned mip-dimension tests use the
equivalent `dimension > 3` form, matching the ARM compare while preserving the
minimum 1-pixel level. The upload formats, block-size formulas, ownership,
callback order and error handling remain source-backed by the Android body.

Focused family result:

- Unweighted source-shape: **97.54%** across five symbols.
- Instruction-weighted source-shape: **93.07%** (weights use original/base
  instruction counts, 797 total).
- Unweighted strict fuzzy: **80.40%**.
- Instruction-weighted strict fuzzy: **54.39%**.
- Linked-exact: 3/5; raw-byte-exact: 0/5.

The family is below the requested 99% source-shape target. This pass preserves
the evidence-backed behavior and records 97.54% as the current measured result;
it does not claim that 99% is an absolute compiler-shape ceiling.

## Rejected source-shape trials

| Trial | Result | Decision |
| --- | --- | --- |
| Give callback `TextureCreate` a shared output-store exit and a two-element local texture buffer | 95.4% source / 34.1% strict, down from 95.5% / 51.7% | Rejected; worsened strict fuzzy and did not improve source-shape. |
| Change `TextureCreateGlobal` to return `unsigned int` and explicitly return zero | 98.2% source / 89.9% strict, down from 100.0% / 100.0% | Rejected; the header's existing `void` API is retained. |
| Use a shared callback output label without the extra buffer | 95.5% source / 51.7% strict | Rejected; identical metrics to the existing source with more control-flow scaffolding. |

These trials did not justify accepting lower-scoring or less direct code.

## ARM and native validation

- Focused report: `_work/texture-create-arm-scalar-trial.json` (the accepted
  scalar-handle variant).
- Full ARM objects: **204/204 translation units compiled**, with NDK r18b.
- Full-corpus comparison: **4,274/4,524** original functions compared, 250
  absent, 7 implemented with a different signature, 0 skipped units.
- Baseline before this pass: 2,037 linked-exact and 932 raw-byte-exact functions.
- Full-corpus comparison after this pass: 2,038 linked-exact and 932
  raw-byte-exact functions. No linked-exact or byte-exact functions were lost;
  `TextureCreateGlobal` gained linked-exact status.
- Native UCRT64 `gof2` build: passed.

The three `Player` / `NewsTicker` / `StarMap` compile failures named in the
goal were already resolved in the checked-out branch at baseline: its first
successful NDK r18b build compiled all 204 translation units. No files owned by
the Gemini mesh branch were changed.
