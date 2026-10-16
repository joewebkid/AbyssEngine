# Bounded AERandom behavior, Android ARM (2026-10-16)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Direct disassembly of `_ZN11AbyssEngine8AERandom7nextIntEi` at
`0x71ad0`–`0x71b5c` shows a 32-bit wrapping negation and power-of-two
test (`0x71ada`–`0x71ae6`). That branch advances the 48-bit seed once
and returns the high 31 bits of the signed product with the bound
(`0x71aea`–`0x71b12`). The other branch divides to obtain a remainder,
retries rejected values, and commits the accepted seed only after the
loop (`0x71b1a`–`0x71b52`).

The C++ source expressed the test as `bound & -bound` on signed `int`.
For `INT_MIN`, ARM wraps the negation but C++ signed negation is
undefined. The production method now performs that test with
`uint32_t`, preserving the ARM result without changing the two
branches or the 48-bit seed algorithm. No known gameplay caller in
this audit passes `INT_MIN`; this is an ABI/edge-case correction.

`tools/aerandom_bound_smoke.cpp` models the ARM seed transition,
product, remainder and retry check separately from the production
method. Four initial seeds, ten bounds (zero, powers of two,
nonpowers, large bounds and `INT_MIN`) and 64 calls per case compare
both return values and the seed after every call: **2,560** checks.
The large nonpower bound exercises the rejection loop. Optimized
host compilation passed before and after the change, but a `-ftrapv`
build terminated on the original signed negation and passes after
the correction. Reproduction:

```powershell
C:/msys64/ucrt64/bin/g++.exe -std=c++17 -O2 -ftrapv -Isrc tools/aerandom_bound_smoke.cpp src/engine/core/AERandom.cpp -o _work/aerandom_bound_trap.exe
./_work/aerandom_bound_trap.exe
```

Native `gof2`, 206/206 ARM units and 40 verifier tests (one skip)
pass. Focused source-shape and strict fuzzy are unchanged at
**96.4% / 50.5%**; `nextInt(int)` is not exact. Focused reports:
`A:/Galaxy on Fire new version/_work/aerandom_bound_before_20261016.json`
and `A:/Galaxy on Fire new version/_work/aerandom_bound_after_20261016.json`.
The full-corpus report is
`A:/Galaxy on Fire new version/_work/aerandom_bound_full_20261016.json`.
It compares **4,323/4,524** original unique functions, with **201 missing**,
**124 extras**, zero skipped, **93.62%/82.69%** average
source-shape/strict fuzzy, **2,507 linked-exact** and **1,074 byte-exact**.
Exact `(unit, symbol)` pair sets have zero losses or gains against the
preceding full report.

The host oracle does not replace execution on ARM32 and does not
exercise negative non-power-of-two bounds, which are not used by the
reviewed particle/UV callers. Wider runtime behavior and exact code
generation remain separate work.
