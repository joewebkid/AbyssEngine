# Layout constructor: platform metrics (2026-10-14)

## Late window placement metrics (follow-up)

Android `Layout::Layout()` at `0xd1028` writes six previously missing
words at `+0x2c0/+0x2c4/+0x2cc..+0x2d8`. The first five were left
zero by the placeholder fill; `+0x2d8` lay just beyond it and was
uninitialized. IDA lines 127985–127993
give iPad HD's packed values, 128065–128069 and 128330–128355 give
N9, and 128254–128355 give ordinary/Retina/iPad selection. Thumb
stores at `0xd1122–0xd1128` and `0xd1786–0xd18b6` cross-check the
offsets and branch structure. `MenuTouchWindow` reads `+0x2c4` for
save-list columns; `HangarWindow` reads `+0x2cc/+0x2d0` for row and
marker placement; `DialogueWindow`, `WantedWindow` and `MissionsWindow`
read `+0x2d4/+0x2d8`. `+0x2c0` has no confirmed consumer here.

| Profile | `+2c0` | `+2c4` | `+2cc` | `+2d0` | `+2d4` | `+2d8` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low | -2 | 60 | 60 | 30 | 60 | 75 |
| ordinary Retina | -4 | 120 | 120 | 60 | 100 | 125 |
| iPad low | -2 | 90 | 90 | 45 | 100 | 125 |
| iPad Large low | -2 | 180 | 180 | 90 | 100 | 125 |
| iPad Retina | -4 | 120 | 120 | 60 | 160 | 200 |
| N9 | -3 | 108 | 106 | 53 | 100 | 125 |
| iPad HD | -2 | 126 | 126 | 63 | 112 | 141 |

The production-constructor smoke checks all six words across these seven
profiles. Native `gof2`, 206/206 ARM units, three Layout/TouchButton
smokes and 37 verifier tests (one skipped) pass. Focused C1/C2
source-shape rises **38.0% → 39.9%**, while strict fuzzy falls
**9.7% → 9.1%**; neither alias is exact. Full report
`_work/layout_late_window_metrics_full_20261014.json` covers
**4,323/4,524** original functions at **93.28%** average source-shape
and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units and no exact-pair
losses or gains against the previous Layout checkpoint. Save-list,
Hangar, dialogue and mission visual behavior with assets and ARM32 runtime
remain unverified. Next consumer-backed candidate: `Layout+0xcc`, read
by `WantedWindow`; audit its platform writes before changing it.

## Hangar row and ChoiceWindow metrics (follow-up)

Android `Layout::Layout()` at `0xd1028` writes ten formerly zero-filled
words at `+0x254..+0x278`. IDA lines 129089–129103 establish the ordinary
Retina/low selection; 129126–129130 and 129193–129197 give N9 and iPad HD
packed words. Raw Thumb at `0xd23cc–0xd23fe` independently confirms the
ordinary stores and the vector-copy addresses. `HangarWindow` reads the
first four words for row captions, blueprint amounts and subtitle Y;
`ChoiceWindow` reads the remaining six for width, padding and max height.

| Profile | `+254` | `+258` | `+25c` | `+260` | `+264` | `+268` | `+26c` | `+270` | `+274` | `+278` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low, iPad low, iPad Large low | 8 | 2 | 16 | 1 | 300 | 8 | 66 | 70 | 97 | 300 |
| ordinary Retina | 8 | 4 | 32 | 0 | 600 | 1 | 132 | 140 | 194 | 600 |
| N9 | 7 | 4 | 28 | 0 | 450 | 12 | 99 | 105 | 145 | 450 |
| iPad HD | 5 | 2 | 22 | 1 | 421 | 11 | 92 | 98 | 136 | 421 |

The production-constructor smoke checks all ten words for six profiles.
Native `gof2`, 206/206 ARM units, three Layout/TouchButton smokes and 37
verifier tests (one skipped) pass. Focused C1/C2 source-shape rises
**35.8% → 38.0%**; strict fuzzy moves **9.8% → 9.7%**. Neither alias is
exact. Full report `_work/layout_hangar_choice_metrics_full_20261014.json`
covers **4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units and no exact-pair
losses or gains against the save-list checkpoint. Asset-backed visuals,
Hangar/Choice interaction and ARM32 runtime remain unverified. Next: audit
the still zero-filled middle constructor fields for confirmed consumers
before recovering another group.

## Save-list placement and adjacent metrics (follow-up)

Android `Layout::Layout()` at `0xd1028` writes `+0x108..+0x128`
in three platform branches. IDA lines 128014–128017 give the iPad
HD packed words, 128460–128463 the N9 words, and 128473–128581
the ordinary/Retina vector selection and later scalar writes.
Raw Thumb `0xd119a–0xd11ac`, `0xd1a9c–0xd1aac` and
`0xd1c48–0xd1c8c` independently show stores at `+0x108`,
`+0x118` and `+0x128`. For ordinary non-Retina, the vector mask
selects words `[0,1,2,1]`; Retina selects `[-2,2,1,-1]`.
`+0x124` was already set by the ListItem icon pass, so this package
restores the other eight slots.

| Profile | `+108` | `+10c` | `+110` | `+114` | `+118` | `+11c` | `+120` | `+128` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low, iPad low, iPad Large low | 0 | 1 | 2 | 1 | 7 | 25 | 15 | 1 |
| ordinary Retina | -2 | 2 | 1 | -1 | 14 | 50 | 28 | 2 |
| N9 | -1 | 2 | 1 | -1 | 13 | 37 | 22 | 2 |
| iPad HD | -2 | 1 | 2 | 1 | 9 | 35 | 21 | 1 |

`MenuTouchWindow` reads the first four slots for save-list button
placement, row base and background strips. The `+0x118..+0x120`
slots replace the same-size padding but retain offset-based names
because no consumer is confirmed here; `+0x128` likewise has no
confirmed consumer. The six-profile production constructor smoke
checks every value. Native `gof2`, 206/206 ARM units, three Layout/
TouchButton smokes and 37 verifier tests (one skipped) pass. Focused
C1/C2 source-shape/strict fuzzy rises **34.3%/9.5% → 35.8%/9.8%**;
neither alias is exact. Full report
`_work/layout_save_metrics_full_20261014.json` covers
**4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units and no
exact-pair losses or gains against the previous Layout checkpoint.
Live save/load interaction, asset-backed visuals and ARM32 runtime
remain unverified. Next consumer-backed candidate:
`Layout+0x254..+0x278` for HangarWindow.

## Window, button and touch metrics (follow-up)

Android `Layout::Layout()` at `0xd1028` writes the `+0x5c..+0x88`
group in the same platform branches as its first 22 metrics. IDA
lines 127981–127989 give iPad HD's packed writes, 128080–128083
and 128356–128360 give N9, and 128174–128288/128362–128381 give
ordinary profiles. Raw Thumb `0xd10a0–0xd1118` confirms the
constructor's packed stores and the separate `+0x74` write in the
iPad HD branch. `+0x70` was already recovered, so this pass fills
the eleven neighboring slots that the old zero-fill still erased.

| Profile | `+5c` | `+60` | `+64` | `+68` | `+6c` | `+74` | `+78` | `+7c` | `+80` | `+84` | `+88` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low | 30 | 44 | 60 | 303 | 131 | 115 | 263 | 9 | 10 | 80 | 30 |
| ordinary Retina | 60 | 88 | 120 | 606 | 262 | 230 | 526 | 18 | 20 | 160 | 60 |
| iPad low | 45 | 64 | 80 | 403 | 136 | 115 | 263 | 9 | 10 | 80 | 30 |
| iPad Large low | 90 | 128 | 160 | 806 | 272 | 115 | 263 | 9 | 10 | 80 | 30 |
| N9 | 51 | 79 | 108 | 545 | 170 | 207 | 473 | 13 | 15 | 120 | 45 |
| iPad HD | 63 | 90 | 112 | 566 | 191 | 161 | 369 | 12 | 14 | 112 | 42 |

The `+0x5c` height and `+0x78` width feed several windows; `+0x60`
and `+0x64` feed Hangar buttons; `+0x7c/+0x80/+0x84` feed the slider,
TouchButton and medal row. `+0x68/+0x6c/+0x88` are direct constructor
writes but have no confirmed consumer in this package; their names
remain offset-based. Replacing their padding preserves all following
offsets. The production constructor smoke checks all six profiles.

Native `gof2`, 206/206 ARM units, three Layout/TouchButton smokes and
37 verifier tests (one skipped) pass. Focused C1/C2 source-shape/
strict fuzzy rises **30.9%/7.6% → 34.3%/9.5%**, with no exact alias.
Full report `_work/layout_window_touch_metrics_full_20261014.json`
covers **4,323/4,524** original functions at **93.28%** average
source-shape and **82.18%** strict fuzzy, with **2,479 linked-exact**,
**1,065 byte-exact**, **123 extras**, zero skipped units and no
exact-pair losses or gains against the preceding full report. The
other zero-filled constructor slots, asset-backed visual output and
ARM32 runtime remain unverified. Next consumer-backed candidate:
`Layout+0x108..+0x128`.

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

## Mining-game parameters (follow-up)

`MiningGame` reads the constructor's `Layout+0xd0..+0x104` group for
its center and progress-bar placement, marquee animation, drift
division, ring scaling and selection, and ore-label offsets. The
original Android constructor at `0xd1028` writes iPad HD constants at
IDA lines 128018–128027 and 128008–128009, N9 constants at
128438–128447 and 128454–128455, and ordinary Retina/low branches at
128498–128555. The float words are decoded as IEEE-754; in
particular, the layout-supplied drift divisor is nonzero in every
profile. The `+0xdc` slot is now typed as `float` and its sole
`MiningGame` reader uses that field directly.

| Profile | center `+d0` | marquee `+d4` | bar Y `+d8` | near `+ec` | far `+f4` | HUD inset `+fc` | label Y `+100` | text Y `+104` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| ordinary low (including iPad low) | 15 | 20 | 37 | 40 | 200 | 5 | 2 | 3 |
| ordinary Retina | 30 | 40 | 74 | 80 | 400 | 10 | 4 | 5 |
| N9 | 22 | 36 | 55 | 60 | 300 | 0 | 5 | 4 |
| iPad HD | 21 | 28 | 52 | 56 | 281 | 7 | 2 | 5 |

| Profile | marquee speed `+dc` | animation `+e0` | drift divisor `+e4` | ring scale `+e8` |
| --- | ---: | ---: | ---: | ---: |
| ordinary low | 10.0 | 1.0 | 20.0 | 1.0 |
| ordinary Retina | 20.0 | 2.0 | 10.0 | 2.0 |
| N9 | 15.0 | 1.5 | 12.0 | 1.5 |
| iPad HD | 14.0625 | 1.40625 | 28.125 | 1.40625 |

The six-profile production constructor smoke checks these fields.
Native `gof2`, 206 ARM translation units and 37 verifier tests pass
(one skipped). Focused C1/C2 source-shape rises from 29.3% to 30.9%;
strict fuzzy remains 7.6%. The generated body is 811 instructions
against 2,225 target instructions, so neither alias is exact. The
focused `MiningGame` constructor stays at 1.3% source-shape and 0.6%
strict fuzzy after typing `+0xdc`.

The full corpus remains at 4,323/4,524 covered across 4,718
comparisons, 201 missing, 92.98% average source-shape and 81.41%
strict fuzzy. All 2,403 linked-exact and 1,008 byte-exact pairs are
preserved; the 123 extra-symbol set is unchanged. The aggregate
source-shape does not change after rounding.

The original also writes `+0xf0/+0xf8` and other fields not consumed
by this package; those remain zero-filled in our source. Most
`MiningGame` service/global holders remain unassigned, so neither the
host constructor smoke nor the static-library build proves the mining
game can run. Its ARM32 behavior and rendering remain untested.

Next: bind the unassigned `MiningGame` layout/service holders to
confirmed runtime globals, then recover the remaining `Layout`
`+0x5c..+0x2d8` groups. Confirm ambiguous platform values from ARM,
then validate layout behavior on-device or with a faithful ARM32
runtime. Re-run the full corpus and compare exact and extra-symbol
sets before accepting each group.
