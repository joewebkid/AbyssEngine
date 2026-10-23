# Fighter state-1/state-7 matrix and movement tail (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xded44–0xdf25a`. The recovered IDA
`PlayerFighter.c` at lines 2398–2640 cross-checks the flow but is not
treated as original C++ source.

Both maneuver and normal-steering paths begin with a copy of the
geometry's local matrix. After the normal path updates Fighter `+0x170`,
it orthogonalizes that direction against the matrix's old up vector:
`right = normalize(cross(oldUp, direction))`, then
`up = normalize(cross(direction, right))`, then writes all three basis
vectors with `MatrixSetRotation`. The maneuver path retains the copied
matrix. The previously recovered roll smoothing and timer join at
`0xdf042` remains shared.

After that join, the original builds an identity-based bank matrix
with Z angle `smoothRoll / 4096 * π` (ARM constants `0x39800000` and
`0x40490fdb`) and applies it to the optional Fighter `+0x0c`
subgeometry. If Player byte `+0x68` is clear, movement advances the
local matrix translation by its direction column times
`float(int(currentSpeed * float(dt)))`; an active roll additionally
multiplies by Fighter's roll matrix at `+0x258`. The resulting local
matrix is written to the canvas, copied to Player `+0x04`, and composed
with the optional subgeometry matrix. When the byte is set, that
movement/local-matrix update is skipped. In both cases, Level's
particle manager at `+0x8c` toggles three Fighter handles at
`+0x130/+0x134/+0x138` to the byte's value, then the main geometry
updates its reference matrix. The Player byte's broader meaning is not
inferred from this branch alone.

The production-method smoke covers the orthogonal basis, integer-
quantized displacement, optional roll and subgeometry composition,
three particle handles, the byte-set branch and reference-matrix
update at `-O0/-O2`. All 11 Fighter smokes, native UCRT64 `gof2`, all
208 ARM translation units and 93 verifier tests pass (one existing
skip). Focused comparison of 44 Fighter pairs changes whole-`update`
source-shape/strict fuzzy **87.8%/35.3% → 90.3%/35.0%**. The function
remains nonexact; focused **21 linked-exact** and **10 byte-exact**
pairs are retained.

The full ARM corpus compares **4,323/4,524** original symbols across
**4,735** pairs, with **201** still absent and no skipped or wrong-type
symbols. Rounded mean source-shape/strict fuzzy remains
**94.05%/83.38%**. All **2,557 linked-exact** and **1,082 byte-exact**
pairs are retained: pairwise comparison against the preceding full
report found no missing pair or exact loss, and only the target
function's scores changed.

The shooting branch at `0xdef12`, and the post-reference-matrix
landmark/enemy reactions and virtual update at `0xdf260–0xdf450`, are
still unreconstructed. `PF_update_body` marks the remaining boundary.
This is not a full game-frame validation; the shooting branch is the
next behavioral priority.
