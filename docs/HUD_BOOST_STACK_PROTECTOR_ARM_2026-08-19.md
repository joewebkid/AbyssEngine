# Hud Boost And Stack Protector ARM Pass

Date: 2026-08-19

## Scope

This package restores the Android control-flow shape of the boost button in
`Hud::draw` at `0x163b90` and resolves the remaining eight-byte ARM frame
difference without adding a synthetic local buffer.

The evidence sources are Android `Hud.c` lines 4174-4249, the delinked
`_ZN3Hud4drawExxP9PlayerEgobjj` body, and NDK r18b Clang 7 A/B builds.

## Boost Branch

The local body now follows the native sequence:

- decrement cloak/quick-menu and boost flash timers independently;
- derive boost alpha as `0` or `200`, or `int(rate * 75)`, then add `55`;
- read `getBoostRate()` again on the fractional path as Android does;
- combine mouse, mining and hacking state before the docked check;
- keep touch-pressed, idle and flash-pulse drawing as three separate control
  paths instead of collapsing them into one Boolean and one image selector;
- reset the flash pulse to `80` only on the native flashing path.

With the basic protector this source change moved `Hud::draw` from `33.4%`
and `3002` instructions to `34.0%` and `3006` instructions. The behavior is
source-backed; it is not claimed byte-exact.

## Protector Audit

The Android function reserves `224` bytes and stores a stack canary at
`sp+220`. The local basic-protector build reserved `216` bytes and emitted no
canary. A diagnostic compile of the same source with
`-fstack-protector-strong` reproduced all of the following without a source
change:

- `sub sp, #224`;
- the original GOT load of `__stack_chk_guard`;
- the saved canary at `sp+220`;
- the corresponding epilogue check;
- a `38.7%` fuzzy score and `3027/3223` instructions.

The strong protector also matches the target canary pattern in several other
address-taken String-heavy Hud functions. Therefore the canonical ARM verify
flags now use `-fstack-protector-strong`. No dummy `char` array, volatile
scratch, manual canary read or decompiler guard arithmetic was added.

## Full Corpus A/B

Both builds compared the same 4436 functions from 201 successful translation
units. The same three unrelated `SolarSystem *` versus integer compilation
failures remained.

| Metric | Basic protector | Strong protector |
| --- | ---: | ---: |
| Average fuzzy match | 70.1% | 71.64% |
| Linked-exact | 1848 | 1871 |
| Raw-byte-exact | 858 | 858 |
| Exact functions lost | 0 | 0 |

There are 425 fuzzy improvements and 118 fuzzy reductions. No linked- or
byte-exact function regressed. Large reductions such as
`Hud::drawOrbitInformation`, `Radar::drawCurrentLock` and
`HangarWindow::autoEquipSecondaryWeapons` indicate that their current local
address-taken temporaries differ from the original source shape; they are now
explicit follow-up audits rather than reasons to retain the less accurate
global compiler flag.

Across the 48 `_ZN3Hud` comparisons the new result is `70.0%` average, 22
linked-exact and 16 raw-byte-exact. `Hud::init` is `24.2%`, and
`updateSecondaryWeaponString` is `38.2%`.

## Verification

- UCRT64 `libgof2.a`: green.
- ARM corpus: `201/204` translation units.
- `Hud::draw`: `33.4%` -> `38.7%`.
- Generated body: `3002` -> `3027`; Android target: `3223`.
- Stack reservation and VFP save set now match: `224` bytes and `d8-d11`.
- Full corpus: `70.1%` -> `71.64%`, with 23 additional linked-exact
  functions and no exact regression.

## Remaining Work

`Hud::draw` still lacks 196 target instructions. The next body pass should
focus on mission/cargo String expression lifetimes and the remaining progress
and mining joins. The frame/canary problem is closed and must not be revisited
with artificial stack objects.
