# PlayerFighter constructor initial values — 2026-10-15

## Evidence

The original Android 2.0.16 `libgof2hdaa.so` has SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
REA/IDA disassembly of `PlayerFighter::PlayerFighter` at `0xdc2f8`
shows a double-word store at `0xdc5be` to `this+0x1a4/+0x1a8`:
`0x40000000` (the already-correct rotate `2.0f`) and `0x3bf9fff6`
(shootError `0.00762939f`). Later `0xdc752–0xdc766` loads
`0x443b8000` and `0x41723ace` and writes them to `this+0x294/+0x298`.
The previous source initialized all three latter fields to zero.

The separate `setShootError(int)` at `0xdccf4–0xdcd00` converts its
argument to `float` and writes `this+0x1a8`; the existing setter is
correct and remains unchanged. The two words at `+0x294/+0x298`
look like float encodings (750 and approximately 15.139357), but
the store alone does not establish their logical type. Their C++ fields
remain opaque integer words containing the exact original bit patterns.

## Validation and limits

Native `gof2`, both PlayerFighter production-fragment smokes and 37
verifier tests (one skip) pass. All 206 ARM translation units compile.
The constructor's focused source-shape stays **94.5%** and strict fuzzy
improves **37.9% → 38.6%**; it is not exact. The full-corpus metrics are
recorded in `PROJECT_STATUS.md` and the local report
`_work/player_fighter_constants_full_20261015.json` in the parent
workspace.

No original ARM32 constructor execution was observed. These three
initial values are statically established; the runtime effects and
semantic role of the two opaque tail words remain unverified.
