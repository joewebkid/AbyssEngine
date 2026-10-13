# Layout constructor: platform metrics (2026-10-14)

## Evidence and scope

The Android `Layout::Layout()` constructor starts at `0xd1028` in the
IDA export (`analysis/gof2_libgof2hdaa_full_ida.c`, around line 127730).
Its C1/C2 aliases each contain 2,225 target ARM instructions. The
existing source represented only 154 instructions and dereferenced
eight unassigned configuration pointers. This pass replaces those
pointers with the matching `Globals::iPadHD`, `n9`, `retinaDisplay`,
`iPad`, and `iPadLarge` flags.

The first 22 consecutive integer metrics (`Layout+0x4..+0x58`) now
follow the Android branches for iPad HD, N9, ordinary Retina, and
ordinary non-Retina. The directly observed tail values at
`+0x1f0..+0x200`, `+0x238`, `+0x27c..+0x290`, `+0x2bc`, and
`+0x2f4..+0x320` now include reward-box dimensions, choice-window
metrics, help-window placement, and the float bit pattern stored at
`+0x2f8`. Previously restored list-item and Hangar fields remain.

This is **not** a complete constructor recovery. The broad zero-fill
between the early metrics and the tail is still a source placeholder;
the original writes many nonzero platform-specific fields there.
The iPad HD scrollbar values and N9 byte flags in the IDA output have
ambiguous temporary-variable provenance and need direct ARM/dataflow
confirmation. Do not treat host smoke results as original ARM32 runtime
or visual-layout proof.

## Checks

The focused ARM comparison for both C1/C2 aliases rises from 8.7% to
23.9% source-shape and from 2.9% to 5.8% strict fuzzy. Our generated
body grows from 154 to 565 instructions against 2,225 target
instructions; neither alias is exact. The production-method host smoke
now exercises the constructor for ordinary, Retina, N9, iPad HD,
iPad, and iPad Large profiles. Native `gof2`, all 206 ARM translation
units, and 37 verifier tests pass (one skipped).

The full corpus remains at 4,323/4,524 covered (201 missing) across
4,718 comparisons. Average source-shape rises from 92.97% to 92.98%;
strict fuzzy remains 81.41%. All 2,403 linked-exact and 1,008
byte-exact pairs are preserved, and the 123 extra-symbol set is
unchanged.

## Consumer-backed middle fields (follow-up)

The Android constructor writes `Layout+0x70` for the row height later
read by `MenuTouchWindow`, `+0x8c` for the StarMap key-box width,
`+0x90` as float bits copied into `StarMap::dragScale`, and
`+0x98/+0x9c` for `Radio` box placement. The intervening `+0x94`
write is restored with the same branch group. IDA lines 127968–128088
show fixed iPad HD/N9 values; lines 128204–128425 show the ordinary
branches. The original ELF bytes at `0xd2854`, `0xd2858`,
`0xd1d94`, and `0xd1d98` cross-check the float constants used by the
ordinary and N9 paths. `+0x90` remains an `int` slot because the
consumer copies its four bytes as a float.

| Profile | `+0x70` | `+0x8c` | `+0x90` bits | `+0x94` | `+0x98` | `+0x9c` |
| --- | ---: | ---: | --- | ---: | ---: | ---: |
| ordinary low | 30 | 32 | `0x3f266666` | 10 | 420 | 50 |
| ordinary Retina | 60 | 64 | `0x3ea66666` | 20 | 840 | 100 |
| iPad low | 45 | 32 | `0x3ea66666` | 10 | 420 | 50 |
| iPad Large low | 90 | 32 | `0x3ea66666` | 10 | 420 | 50 |
| N9 | 51 | 57 | `0x3ea66666` | 15 | 630 | 75 |
| iPad HD | 63 | 45 | `0x3eeca864` | 14 | 590 | 70 |

The host smoke asserts all six profiles. Focused C1/C2 source-shape
rises from 23.9% to 27.3%, while strict fuzzy moves from 5.8% to
5.7%; each body now has 630 generated instructions against 2,225
target instructions. Neither alias is exact. Native `gof2`, all 206
ARM units, and 37 verifier tests pass (one skipped). The strict fuzzy
dip is recorded rather than treating the source-shape gain as proof
of instruction equality.

The full corpus still compares 4,323/4,524 original functions across
4,718 rows: 92.98% average source-shape, 81.41% strict fuzzy, 2,403
linked-exact, 1,008 byte-exact, 201 missing, and 123 extra symbols.
Comparison against the previous full report finds zero exact-pair
losses or gains and no change in the extra-symbol set. The aggregate
source percentage is unchanged after rounding despite the constructor
improvement.

## HUD initialization metrics (follow-up)

`Hud::init` copies all eight `Layout+0x12c..+0x148` words into its
boost text, touch-zone, analog-stick, radar, and event-line state.
Before this pass the constructor's zero-fill left all eight at zero.
Android IDA lines 128030–128053 contain the iPad HD packed writes;
128464–128469 contain the N9 writes; 128583–128630 contain the
ordinary branches and the final 128-bit selection for the last four
fields. The table is decoded as little-endian 32-bit words, not as
64-bit scalar values.

| Profile | `+12c` | `+130` | `+134` | `+138` | `+13c` | `+140` | `+144` | `+148` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low | 15 | 40 | 80 | 35 | 17 | 11 | 13 | 2 |
| ordinary Retina | 30 | 80 | 160 | 70 | 34 | 22 | 26 | 4 |
| iPad low | 15 | 56 | 108 | 47 | 17 | 11 | 13 | 2 |
| iPad Large low | 15 | 112 | 216 | 94 | 17 | 11 | 13 | 2 |
| N9 | 22 | 60 | 120 | 52 | 25 | 19 | 19 | 19 |
| iPad HD | 21 | 78 | 151 | 66 | 23 | 15 | 18 | 2 |

The production constructor smoke asserts every value in these six
profiles. Native `gof2`, 206 ARM units, and 37 verifier tests pass
(one skipped). Focused C1/C2 source-shape rises 27.3% to 29.3%,
strict fuzzy 5.7% to 7.6%, and generated size 630 to 732 versus
2,225 target instructions. Neither constructor alias is exact.

The full corpus remains 4,323/4,524 covered over 4,718 comparisons,
with 201 missing, 92.98% average source-shape, 81.41% strict fuzzy,
2,403 linked-exact, 1,008 byte-exact, and 123 extra symbols. The
complete exact-pair and extra-symbol sets match the preceding report:
zero losses and zero gains. The aggregate shape percentage is
unchanged after rounding despite the focused gain.

Next: recover the zero-filled `+0xd0..+0x104` MiningGame metrics,
especially its drift divisor and ring scale, then the remaining
`+0x5c..+0x2d8` groups. Confirm ambiguous platform values from ARM,
then validate layout behavior on-device or with a faithful ARM32
runtime. Re-run the full corpus and compare exact and extra-symbol
sets before accepting each group.
