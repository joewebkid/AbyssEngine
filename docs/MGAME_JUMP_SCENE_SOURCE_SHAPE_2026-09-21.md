# MGame ARM Source-Shape Pass: Jump Scene

Date: 2026-09-21

## Evidence and change

Android `MGame::updateJumpScene` (`0x17f588`, IDA body in
`analysis/gof2_libgof2hdaa_full_ida.c`) and the linked ARM instruction diff
show distinct `usingJumpDrive` and jumpgate offset-calculation paths. The
previous C++ source initialized one `Vector` in a shared scope; Clang merged
the paths into a conditional Z multiplier. Separate branch-local vectors and
`MatrixRotateVector` expressions retain the same X/Y/Z values while producing
code closer to the target's two arithmetic paths. This is a source-shape
improvement, not a claim of byte equality.

| Symbol | Before fuzzy/source | After fuzzy/source | Target/base instructions |
| --- | ---: | ---: | ---: |
| `_ZN5MGame15updateJumpSceneEv` | 90.6% / 97.0% | 93.8% / 98.4% | 335 / 337 |

## Rejected forms

- Rewriting `nextCamId` in a more literal IDA-style control flow let Clang
  collapse the camera-1 equipment path: source-shape fell from 92.3% to
  64.8%. Retaining its original first condition but changing only the docked
  branch still yielded 66.9%. Both trials were reverted.
- Sharing the `OnRender3D` ship/effect tail across paused and running paths
  reduced source-shape from 95.0% to 88.3%; reverted.
- Splitting the `freeCamTouchMove` touch-ID `||` test into consecutive tests
  generated identical instructions (93.4% source-shape); reverted as churn.
- Reversing the jump-transform time comparison generated identical code;
  reverted to avoid unnecessary signed-subtraction overflow risk.
- Holding `PlayerEgo *` locally in the jump exit phase reduced source-shape
  from 98.4% to 98.0%; reverted.

## Validation

- Focused ARM compile with the configured r18b matching flags succeeded.
- Focused `_ZN5MGame` verifier: 45 comparisons, average fuzzy 88.7%, average
  source-shape 97.6%, 16 linked-exact and 9 byte-exact. Baseline before this
  package: 88.6% fuzzy and 97.5% source-shape.
- UCRT64 `gof2` target linked `libgof2.a`. Two pre-existing non-void empty
  key-handler warnings remain in `MGame.cpp`.
- Full verifier with `--no-build`: 4681 comparisons, average fuzzy 75.4%,
  average source-shape 87.4%, 4130/4524 original functions compared. It used
  the newly compiled MGame object and cached objects for other units; it was
  not a fresh full ARM rebuild.
- Reports: `_work/mgame_family_jumpscene_20260921.json` and
  `_work/mgame_full_corpus_jumpscene_20260921.json` (local generated outputs).

The 99% MGame-family target is **not met**. Largest remaining source-shape
debts are `OnTouchEnd` (81.2%), `OnUpdate` (92.0%), `freeCamTouchEnd` (92.0%),
`dockEvent` (92.3%), `nextCamId` (92.3%), and `freeCamTouchMove` (93.4%). The
next pass needs function-body/control-flow evidence, especially around the
large OnTouchEnd modal and HUD action dispatcher; optimizing source text by
appearance alone has repeatedly reduced the match.
