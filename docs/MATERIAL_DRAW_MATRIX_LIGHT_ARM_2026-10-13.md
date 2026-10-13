# MaterialDraw matrix and ambient routing — Android ARM audit (2026-10-13)

## Evidence and recovery

Android 2.0.16 `libgof2hdaa.so`, symbol
`_ZN11AbyssEngine12MaterialDrawEPNS_11PaintCanvasEPNS_6EngineEPNS_8MaterialEb`
at ELF `0x870b8`, calls the matrix product at `0x8715a`. Its first
matrix operand comes from `Material +0x60` (`arr_5c.data`), its second
from `Material +0x30` (`arr_2c.data`), both indexed with a 60-byte
stride. The result is passed to `PaintCanvas::SetWorldViewMatrix`;
the original `arr_2c[i]` goes to `Engine::SetModelMatrix`, and
`arr_38[i]` to `SetUVMatrix`. The prior C++ passed only one operand to
an unresolved multiplication shim, sent Canvas' unrelated world-view
field to `SetWorldViewMatrix`, and sent the wrong model matrix. The
method now uses `AEMath::operator*(arr_5c[i], arr_2c[i])` and the
original destinations.

The same ARM body checks `Material::ambientColor.x` at `+0x68` against
`-10.0f`. Otherwise it saves the engine light-ambient RGB at
`+0x268/+0x26c/+0x270`, sets light `0x4000` to the material RGB,
draws the batch, and restores the saved RGB. The C++ previously used
the global-scene ambient setter with only the material's first
component and did not restore the engine's previous light. The new
code follows the light-specific path; a 32-bit offset assertion guards
`Engine::lightAmbient`. Packed color bytes are converted by double
division by 255.0 before `SetColor`, as in the ARM VFP sequence.

The production-body host smoke checks two queued meshes, matrix
operand identity/order, world-view/model/UV routing, packed colors,
ambient save/restore, the `-10.0f` sentinel, early exits, and Array
count reset. It mocks matrix multiplication and render services;
those implementations are not validated by this smoke.

## Verification and limits

- Native `gof2` static library: pass. ARM: 206/206 translation units,
  zero failed. Host smoke: `MaterialDraw host smoke: OK`.
  Verifier suite: 37 run, 36 pass, one skipped.
- Focused `MaterialDraw` strict fuzzy rises 43.9% to 96.3%; source-shape
  rises 82.5% to 99.4%. Target and local each disassemble to 135
  instructions, with 100% control-flow and call-count agreement.
  It remains neither linked- nor byte-exact. Focused reports:
  `A:/Galaxy on Fire new version/_work/materialdraw_recovery_before_20261012.json`
  and `A:/Galaxy on Fire new version/_work/materialdraw_recovery_after_20261012.json`.
- Full ARM corpus: 4,714 comparisons, 4,323/4,524 original functions
  covered, 201 missing, zero skipped/wrong-type. Average source-shape
  rises 92.5371% to 92.5407%, strict fuzzy 80.8212% to 80.8323%.
  Linked-exact stays 2,378 and byte-exact 1,008; the exact per-symbol
  sets are unchanged. Full report:
  `A:/Galaxy on Fire new version/_work/materialdraw_recovery_full_20261012.json`.

No full native executable, original assets, GL frame or ARM32 gameplay
was exercised. The `SpriteSystemDraw` batching producer still calls
an unresolved matrix shim and is the next matrix-queue audit target.
