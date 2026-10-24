# Level skybox transitions: Android ARM recovery (2026-10-24)

The Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
is the primary source. Its `Level::switchSkyboxForIntro` at `0xc663c`
passes `this+4` directly to `PaintCanvas::MeshCreate(0x4591, ..., false)`,
passes `this+0x198` to the **three-argument**
`TextureCreate(0x275a, ..., false)`, and calls `KIPlayer::setDead()` for
each asteroid when the array exists. The original `setDead` at `0xa6208`
stores state `4` at offset `0x88` before passing `false` onward. The prior
C++ used a callback-loader `TextureCreate` overload and only deactivated
asteroids; that omitted the dead-state transition.

The adjacent `switchSkyboxForSupernovaReversal` at `0xc668c` likewise
passes the `skyboxMesh` and `field_198` fields directly to the same
three-argument mesh/texture APIs. It obtains the system texture index
twice, adds `0x4588` and `0x2751`, respectively, then sets
`skyboxTexture` to `-1`. The C++ now preserves those calls and fields.

Focused NDK r18b ARM comparison (`-Oz`, 208/208 translation units, zero
failures) of the two changed symbols:

| Symbol | Before source-shape / strict fuzzy | After | Exact after |
| --- | ---: | ---: | --- |
| `switchSkyboxForIntro` | 66.7% / 40.5% | 100% / 100% | linked-exact |
| `switchSkyboxForSupernovaReversal` | 77.3% / 36.2% | 97.3% / 64.0% | nonexact |

Native `gof2` builds. These are source/ABI and instruction-shape
checks, not a live intro or supernova visual test. The remaining
Supernova mismatch is register/global-load ordering, not a known
behavioral discrepancy. A physical Android runtime check of the two
transitions and the pending first-friend ship-insertion gate remain open.

The full verifier run compares 4,323/4,524 original functions across
4,736 pairs, with mean 94.12% source-shape and 83.45% strict fuzzy.
Linked-exact rises from 2,557 to 2,558; byte-exact remains 1,082.
Pairwise comparison against the preceding full report finds zero exact
losses and only the Intro linked-exact gain. There are 201 missing
originals and zero skipped units. All 93 verifier tests pass (one skip).
