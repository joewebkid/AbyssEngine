# Generator Android ARM recovery and unique-body 98% gate (2026-10-08)

The fixed Generator family improves from **92.791% unweighted / 90.023%
original-instruction-weighted** to **98.673% / 97.460%** source-shape. The
unweighted gate passes; the weighted 98% gate **does not pass**. These figures
are assembly source-shape evidence, not game-completion or semantic-equivalence
percentages. No verifier, matching flags, or score weights were changed.

## Scope and frozen baseline

Worktree: `C:/Users/Dima/Documents/Galaxy on Fire new version/gof2hd-98-generator`.
Branch: `codex/generator-arm-98-20261008`.
Clean starting HEAD: `20bf2679a88ada953d3b393bc42a0aa54a557264`.
Only `src/game/mission/Generator.cpp/.h` and this family's evidence snapshots
are changed. No adjacent mission family or shared status document is edited.

Before editing, a clean NDK r18b build and focused report fixed **11 unique
original addresses, 13 symbols, and 3329 total original instruction units**.
C1/C2 and D1/D2 share addresses and receive one weight each. All original
Generator members are included, including its empty constructor/destructor;
no member is dropped because of its size or score. Weights are the existing
verifier's unchanged `n_target` values. Baseline and final JSON preserve each
address, aliases, weight, and per-body score:

- [Frozen baseline](validation/generator_unique_baseline_2026-10-08.json)
- [Final comparison](validation/generator_unique_final_2026-10-08.json)

The full baseline's 13 Generator rows agree with the separate focused baseline
on every source score and compiled instruction count. Means above use the
verifier's displayed per-function scores. Recomputing the same five signals
before display rounding gives **92.794754% / 90.028615%** baseline and
**98.672772% / 97.464150%** final; both versions give the same pass/fail decision.
This arithmetic does not replace or modify the verifier.

| Unique body | Android address | Original weight | Baseline source | Final source | Final strict fuzzy |
| --- | --- | ---: | ---: | ---: | ---: |
| constructor C1/C2 | `0x9539c` | 1 | 100.0% | 100.0% | 100.0% |
| destructor D1/D2 | `0x9539e` | 1 | 100.0% | 100.0% | 100.0% |
| computerTradeGoods | `0x953a0` | 38 | 94.1% | 100.0% | 97.4% |
| isKaamoSpecialItem | `0x95404` | 15 | 100.0% | 100.0% | 100.0% |
| getItemBuyList | `0x95428` | 674 | 91.1% | 97.0% | 67.9% |
| getShipBuyList | `0x95bcc` | 760 | 84.6% | 96.7% | 67.2% |
| getLootList | `0x96504` | 181 | 89.3% | 99.2% | 95.6% |
| generateStationIndex | `0x9672c` | 118 | 83.9% | 99.1% | 97.9% |
| createAgents | `0x96884` | 531 | 93.6% | 99.1% | 90.0% |
| createAgent | `0x96e8c` | 292 | 92.1% | 97.9% | 93.8% |
| createMission | `0x971b8` | 718 | 92.0% | 96.4% | 57.5% |

Focused exact counts stay **5 linked-exact / 4 raw-byte-exact symbols**.
Those correspond to three/two unique bodies respectively. The Kaamo predicate
was already linked-exact despite its zero-filled table: relocation-masked code
agreement did not validate that data. Its recovered table is now independently
checked, and its linked-exact pair remains present.

## Primary evidence

Android ARM 2.0.16 `libgof2hdaa.so`, SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original ELF, exported symbols, ARM/Thumb instructions from
`0x9539c..0x97984`, and the workspace's
`analysis/gof2_libgof2hdaa_full_ida.c` Generator bodies were inspected together.
Decompiler output is a guide; instructions settle RNG bounds, constructor
arguments, float constants, repeated loads, and missing call arguments.
No iOS/PC/Java constants or behavioral guesses are substituted for Android.

Literal tables were read through ELF load-segment address/file-offset mapping:

| Android address | Contents | Entries |
| --- | --- | ---: |
| `0x1fb590` | Kaamo special items; seller refresh and item exclusions share this table | 9 |
| `0x1fb5b4` | Kaamo stock eligibility, including its two zero entries | 10 |
| `0x1fb5dc` | Ship races | 64 |
| `0x1fb6dc` | Loot type chances: 10, 40, 2, 10, 100 | 5 |
| `0x1fb6f0` | Station selection exclusion table | 52 |
| `0x1fb7c0` | Six Kaamo ship offers | 6 |
| `0x1fb7d8` | Enemy-agent race order: 2, 3, 0, 1 | 4 |

ELF exports independently anchor `Globals::status=0x2181b0`,
`rnd=0x2181cc`, `options=0x2181e8`, `items=0x2182ac`,
`ships=0x2182b0`, and `imageFactory=0x2182b8`. The decompiler's
`dword_21821D` flags are therefore `options[0x35]` and `options[0x37]`.
The patch uses these established slots instead of uninitialized local pointer
stand-ins or separate singleton aliases. Existing volatile table-read qualifiers
are retained; no register barriers, fake calls, assembly padding, or flag changes
are introduced to lift a score.

## Restored behavior and contracts

- Trade goods: read the amount before bounded `nextInt(3)`, then reread the
  item for the change call, as at `0x953a0`.
- Station selection: 100/100/135 bounds, current-system station-count bound,
  system-15 selection, exclusion data, route checks, and the original visibility
  overwrite. A visible system can overwrite an earlier blocked-station flag;
  this observed behavior is deliberately preserved.
- Agent lists: bounded count/seller rolls, the six ship-choice candidates,
  original repeated offer/mission accesses, one wingman offer, enemy agents,
  reward boost, and the scripted mission-23 seller's **100000** price.
  Agent-name temporaries are passed directly to the real by-value constructor,
  preserving native allocation/name/constructor/destruction order.
- Missions: used-type state/reset and 999-attempt guard, early-campaign type
  selection, destination/race constraints, all bounded RNG, excluded delivery
  items, and item-derived difficulty. Type-0 goods use the **95** coefficient.
  Rewards use distance **1200**, difficulty **5500**, type-9 **1.2**, type-7
  **0.7**, delivery max price **1.7**, and type-11 **0.6**. Reward rounding stays
  floating-point until the native conversion point. Costs retain the bounded
  reward/10 roll. The real exported `Mission(int,String,int*,int,int,int,int)`
  is called with the ARM argument order (see `0x97790`), replacing the undefined
  `Mission_ctor_full` shim. The unused shim declaration is removed from the header.
- Ship stock: source-known races, 6-count roll, forced first ships, uniqueness
  retry, exact bonus bounds, bounty entries 6/12/18/24 and ships 45/46/47/48,
  and repeated status/array reads between state-changing calls.
- Item stock: intro `makeItem(1,0)` calls, original RNG reset, campaign divisor
  **45**, probability changes **0.01**, exclusion conditions, arithmetic
  half-tech shift at `0x95506`, source call order, quantity scaling divisor **50**,
  special quantity branches, and actual `makeItem(amount)` factories. Station
  126 retains both original factory calls and appends the second returned item.
- Loot: type probabilities and item-count bounds, 100-attempt rejection loop,
  exclusion/tech checks, commodity/non-commodity quantity rolls 9/3,
  fallback 154..163, and the energy-cell opportunity with bound 100.

Public Generator member signatures and its stateless one-byte layout remain
unchanged. Existing neighboring pointer/return models and Mission internals are
not silently retyped or repaired in this package.

## Validation

All build/report/temp outputs are on `C:/gof2-generator-20261008/`.
Original binary/maps and NDK are read-only through:

```
GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work
GOF2_VERIFY_LOCAL_NDK=1
GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b
```

- Fresh focused baseline/final verifier reports: no skipped units; all fixed
  addresses, aliases, weights and exact pairs retained.
- Native UCRT64 `gof2` static library: builds successfully after final edits.
- ARM32 compile-time ABI probe with unchanged matching flags: passes
  `sizeof(Generator)=1`, `Mission=0x78`, `Agent=0x98`, `Array<int>=0x0c`, and
  Status used-type/generated-station slots at `0x50/0x14c`. This verifies the
  allocations/slots touched here, not complete runtime ownership or native ABI.
- Matching ARM build: **206/206 source-mapped translation units, zero failures**.
- Verifier discovery: **36 run, 35 pass, one optional real-object test skipped**.
  The omitted real-object test is then run directly with the final ARM
  `ArrayInstantiations.o`: **2/2 alias tests pass**, covering the seven audited
  array ABI aliases. Thus all 36 distinct verifier checks are exercised.
- Actual selected Generator host bodies with deterministic mocked external
  services: **pass**. This checks Kaamo IDs/negative cases, amount-before-RNG
  trade order, protected/no-items stations, blocked-station retry, visibility
  overwrite, local/system-15 choices, explicit/no-loot cases, both quantity
  bounds, 100 ingredient rejections plus fallback, and energy-cell replacement.
  Harness and executable are saved locally as `generator_smoke.cpp/.exe`.
  The smoke asserts the one-byte class size. On PE/COFF, unexercised Generator
  text/unwind sections were removed from a temporary host object to isolate the
  selected functions; no game sources or ARM objects were altered for the smoke.

Full-corpus `--fail-on-wrong-type` exits **0** for baseline and final.
Every baseline/final comparison is keyed by **(unit, symbol)**:

| Full-corpus measure | Baseline | Final |
| --- | ---: | ---: |
| Comparison rows | 4689 | 4689 |
| Compared original symbols | 4323/4524 | 4323/4524 |
| Missing original symbols | 201 | 201 |
| Wrong-type misses / skipped units | 0 / 0 | 0 / 0 |
| Linked-exact / byte-exact pairs | 2305 / 997 | 2305 / 997 |
| Mean source-shape / strict fuzzy | 91.80% / 79.73% | 91.81% / 79.79% |

There are **zero lost or gained comparison rows, zero lost or gained linked-exact
pairs, and zero lost or gained byte-exact pairs**. No Generator ABI signature or
coverage regression is found. The full final Generator rows agree with the
focused final on every source score and instruction count. Extra (non-original)
symbols decrease from 116 to 115 after removal of the null-guard system-index
helper, replaced by the actual source-known accessor chain.

Local records: `full-baseline.json`, `full-final.json`,
`corpus-pair-regression.json`, `focused-baseline.json`, `focused-final.json`,
`native-final.log`, `tests-final.log`, `real-alias-test.log`, and `arm-abi-probe.log` under the C:
output directory. The compact checked-in snapshots contain the fixed-family
address/alias/weight evidence without the complete generated corpus output.

Reproduce the full final verifier gate from this worktree with the environment
above and the existing NDK, using an output directory on C:

```powershell
python tools/verify/verify.py --build-dir C:/gof2-generator-20261008/final-verify --report C:/gof2-generator-20261008/full-final.json --fail-on-wrong-type
python -m unittest discover -s tools/verify -p 'test_*.py'
python tools/verify/test_asmdiff_aliases.py C:/gof2-generator-20261008/final-verify/base/engine/core/ArrayInstantiations.o
```

## Remaining limit

The weighted gate is still below 98%. Most weight is in `getShipBuyList`,
`getItemBuyList`, and `createMission` (2152/3329 units). These remain 96.7%,
97.0%, and 96.4%; their call counts agree with the original, but instruction
inventories, branch form, register/stack allocation, and literal-pool placement
still differ. For example, the ship-stock body has 760 original versus 730
compiled instructions with 100% call-count agreement; many residual inventory
differences are zero-offset load encodings and repeated PC-relative pool loads.
This pass establishes a measured result, not a proof that 98% is impossible.
Further gains require evidence for original source/dataflow choices or compiler
behavior; inventing work, forcing loads, changing flags, or changing the scorer
would not be a valid completion of the requested gate.

No ARM-device executable gameplay, whole-family runtime/ownership check, or
native-port ABI correctness is claimed by the host static build or mocked smoke.
Known neighboring integer-return pointer models remain a limitation for 64-bit
native execution. The three large generation paths are source/ASM checked, not
executed with real game services. No push, PR, or integration/main merge is made.
