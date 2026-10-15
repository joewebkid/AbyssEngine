# ImageFactory RNG call contract — Android ARM, 2026-10-15

## Confirmed source and correction

Original Android 2.0.16 `libgof2hdaa.so` SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA assembly for `ImageFactory::createChar(int)` at `0x11c6cc`
loads `Globals::rnd` into `r0`, bound 2 into `r1`, then calls
`_ZN11AbyssEngine8AERandom7nextIntEi` at `0x11c6dc`.
The overload at `0x11c6f4` makes the same member call with bound 4
at `0x11c70c`, then in its four-part loop at `0x11c750` with bounds
from the table at `0x201c24`. The recovered source had the right
object/bounds values but emitted the undefined free-function symbol
`_ZN11AbyssEngine8AERandom7nextIntEPvi`. It now casts the existing
`Globals::rnd` object to `AERandom*` and calls its real member method.

The REA/IDA evidence identifiers are
`ev_a6370847d0997affdb9fc8fe644b61da7eb2c3774483f06ed4362871100e5d60`
and `ev_f561ebab9fe7f02cc21612d5608e741af2fa9a4ce99de0d47b85375a7f6ef19b`.
The assembly/register evidence, not the decompiler's recovered C++
signature, establishes the ABI claim. REA reports that IDA does not
provide complete body ranges or typed reference coverage in this profile.

## Gates and interpretation

| Gate | Before | After |
| --- | ---: | ---: |
| Focused `createChar` source-shape/strict fuzzy | 100%/100% both | 100%/100% both |
| Focused linked-exact / byte-exact | 2 / 0 | 2 / 0 |
| Full corpus compared unique / original | 4,323 / 4,524 | 4,323 / 4,524 |
| Full average source-shape / strict fuzzy | 93.59% / 82.63% | 93.59% / 82.63% |
| Full linked-exact / byte-exact | 2,503 / 1,074 | 2,503 / 1,074 |
| Direct call-target matches / candidates | 2,345 / 80 | 2,348 / 77 |

Exact-pair comparison of the full before/after reports found zero lost and
zero gained `(unit,symbol)` pairs in both exact sets. Missing remains 201,
extras 123, skipped zero. The ARM build compiled all 206 units with zero
errors; native `gof2` and the extracted-production portrait smoke passed.
The smoke fixture now gives `Globals::rnd` an RNG object and checks member
dispatch. Local reports are outside Git at
`A:/Galaxy on Fire new version/_work/image_factory_rng_before_20261015.json`,
`image_factory_rng_after_20261015.json`,
`image_factory_rng_full_20261015.json`, and
`arm_call_targets_after_image_rng_20261015.json`.

This fixes a symbol/ABI contract, not a demonstrated random-sequence
behavior bug. Original ARM32 execution, end-to-end game portraits and
random-sequence parity remain unverified. Next bounded investigation:
PaintCanvas methods whose masked-byte-matched bodies still call local
`paintcanvas_ext_*` shims instead of original engine methods.
