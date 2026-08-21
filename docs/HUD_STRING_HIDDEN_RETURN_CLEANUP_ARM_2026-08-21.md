# Hud String Hidden-Return And Cleanup ARM Pass

Date: 2026-08-21

## Scope

This package audits the temporary `AbyssEngine::String` graph in Android
`Hud::draw`, focusing on the secondary-weapon label and the dock/jump/cloak/
mining progress family. The primary evidence is the Android body at
`0x163b90`, especially recovered lines `4109..4172` and `4294..4533`, plus
focused ARM disassembly of `_ZN3Hud4drawExxP9PlayerEgobjj`.

The committed baseline was `48.1%` with `3039/3223` instructions. The accepted
package reaches `49.5%` with `3051/3223` while preserving the exact 224-byte
stack reservation and `d8-d11` VFP save set.

## Secondary Label Cleanup

Android constructs the label as one nested expression:

1. fetch GameText `1274 + Item::getIndex()`;
2. construct `String(" (")`;
3. construct the amount String;
4. concatenate those two;
5. construct `String(")")`;
6. concatenate the suffix;
7. construct its orientation copy with `false`;
8. concatenate GameText and the suffix into the final String.

The six intermediate Strings are destroyed immediately after the final
`operator+`, before text width and drawing. The final String remains alive
through the draw and color restore. The previous named-local form delayed all
intermediate destructors until the branch exit and generated the wrong
exception cleanup graph.

The secondary branch now also follows two native ownership rules:

- `PlayerEgo+0x0c` is an initialized `Level *` during HUD rendering, so the
  original direct `Level+0x69` manual-secondary read has no nullable guard;
- secondary images, banner, text metrics and tint use the branch-local
  `Globals::Canvas` lifetime visible in Android.

## Progress String Slots

The dock label now uses `baseLabel + String(" ")` directly. Clang constructs
the space String at stack `+196`, performs the intervening native arithmetic,
writes the hidden `operator+` result at stack `+208`, and destroys the
temporary before any width/draw calls. This matches the recovered
`v298/v299` lifetime and allows the final slot to be reused by later labels.

Jump/cloak GameText `318/317` and mining GameText `618` now explicitly call
`String(const String &, false)`. The prior copy-initialization selected the
local inline copy constructor and `Set`, while Android calls the out-of-line
bool constructor.

## A/B Results

- Secondary temporary chain alone: `48.1% -> 47.8%`, `3042/3223`; retained
  only after testing the complete related lifetime package.
- Adding the dock hidden-return lifetime: `47.8% -> 49.4%`, `3044/3223`.
- Removing the non-native nullable Level guard: `49.4% -> 49.3%`; retained as
  direct source-backed runtime behavior.
- Restoring the local secondary Canvas owner recovered `49.4%`.
- Explicit bool copy constructors for progress labels reached `49.5%` and
  `3051/3223`.

This is why the temporary chain must not be judged as an isolated fuzzy-score
edit. Its value appears when all related hidden-return and cleanup slots are
present together.

## Verification

- UCRT64 `gof2`: green; `libgof2.a` links successfully.
- ARM compile: `201/204`; the same three unrelated `SolarSystem *` versus
  integer failures remain.
- Focused `Hud::draw`: `49.5%`, `3051/3223` instructions.
- All 48 Hud functions: `88.6%` average, 26 linked-exact and 19 byte-exact.
- No synthetic scratch, volatile register forcing, or fake stack object was
  introduced.

## Next Boundary

The remaining 172-instruction deficit now points primarily at the earlier
mission/cargo String expressions and their exception cleanup entries. The next
whole-function package should replace remaining inline copy-constructor sites
with the proven native bool-constructor/temporary forms, while preserving the
already accepted cargo call order and Canvas ownership.
