# AERandom ARM-derived sequence check (2026-10-15)

## Evidence and scope

REA/IDA inspected the shipped Android 2.0.16 ARM ELF
(`libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The `next(int)` body at `0x71a54`, `nextInt()` at `0x71aa4`, and
`nextInt(int)` at `0x71ad0` show the 48-bit LCG step, high-bit extraction,
power-of-two multiply path and non-power-of-two rejection loop. In particular,
the ARM `UMULL`/`MLA`/`ADC` sequence updates both 32-bit words; `STRD`
commits the state; the bounded path uses `__aeabi_idivmod` and a signed
`BLT` on its wrapped rejection expression. REA did not provide complete
function-body ranges or complete indirect-call relationships; this note does
not claim either.

`tools/verify/aerandom_oracle_smoke.cpp` translates those **machine
operations**, independently of `AERandom.cpp`, into unsigned 32-bit limb
arithmetic. It compares each result and both state words after every draw
for six seeds (including negative and extremal 64-bit seeds), 128 rounds per
seed, bit widths 1/2/16/17/31/32, and zero, power-of-two and non-power-of-two
bounds through `INT_MAX`. The loop checks that rejection actually occurs.
Zero bound is deliberately covered because the ARM power-of-two test accepts
it; negative bounds and out-of-range bit widths are not asserted as a public
contract. Seed constructor and `setSeed` are checked against the observed
low-word XOR and high-halfword XOR layout. Time-seeded construction and
`reset` are outside this deterministic sequence test.

The non-power-of-two rejection expression in `AERandom.cpp` formerly added
positive signed integers that can overflow for large bounds. C++ does not
define that overflow, while the observed ARM `SUB`/`ADD`/`BLT` sequence
uses the sign of the wrapped 32-bit result. It now uses unsigned 32-bit
addition and tests bit 31. This preserves the observed ARM rule while
removing that native undefined behavior for the tested nonnegative bounds.

## Outcome

On UCRT64, the extracted production implementation passed **2,304 draws and
112 rejection iterations**, with state equality after every call. The native
`gof2` build and all **206/206** ARM translation units remained green.
Focused ARM comparison of all 11 `AERandom` symbols: **99.7%** average
source-shape, **95.5%** strict fuzzy, **2 linked-exact** and **2 byte-exact**
(the destructor pair). `nextInt(int)` remains **96.4%** source-shape and
**50.5%** strict fuzzy after the defined-arithmetic fix; focused shape and
exact pairs did not regress.

The full ARM corpus remains **4,323/4,524** original functions compared,
**93.43%** average source-shape, **82.33%** strict fuzzy, **2,485
linked-exact** and **1,065 byte-exact**, with **201 missing**, **123 extras**
and zero skipped. Both exact-symbol sets are unchanged versus the preceding
`player_end_explosion_full_20261015.json` report. All 37 verifier tests pass
(one expected skip).

This is a deterministic check against an ARM-instruction-derived model, not
execution of the original `.so`, a proof about unspecified inputs, or a live
particle-system test. Next prioritize controlled original-binary/runtime
observations for behavior still uncertain at callers; do not infer system-wide
decompilation readiness from this family's high source-shape.
