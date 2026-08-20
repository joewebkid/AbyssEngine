# Hud Main Action And Target Context ARM Pass

Date: 2026-08-21

## Scope

This package audits the Android ARM `Hud::draw` span immediately after the
boost control and through the primary-action/target-context overlay. The
primary target is `_ZN3Hud4drawExxP9PlayerEgobjj`; Android Hex-Rays at
`0x163b90`, the recovered `Hud.c` body around the native `LABEL_298` join, and
focused ARM disassembly were used together.

The committed baseline was `47.4%` with `3033/3223` instructions. The accepted
source reaches `48.1%` with `3039/3223`. The stack reservation remains the
target `224` bytes and the VFP save set remains `d8-d11`.

## Accepted Recovery

- Primary pressed/idle image selection now follows the native explicit branch:
  touch bit `0x10` or `Hud+0x4a5` (`fireForTutorial`) selects `Hud+0x2e0`;
  otherwise `Hud+0x2e4` is selected.
- The two primary image overloads and the two target-context overlay overloads
  reload `Globals::Canvas` at their native lifetime boundary. Extending this
  reload to the preceding color setup is not source-shape equivalent.
- The target-context predicate performs a fresh `PlayerEgo::isMining()` call.
  The function-entry cached value belongs to event queue, reticle and boost
  decisions; Android does not reuse it here.
- The remaining predicate order is preserved: asteroid docking, mining,
  stream docking, docking procedure, docking point and turret mode.
- The overlay still uses the confirmed raw Radar slots `+0x14`, `+0x0c`,
  `+0x24`, or enemy `+0x04` with active bytes `+0x70/+0x75`. Its coordinates
  remain `Hud+0x3e4/+0x3e6` plus `Hud+0x3ea`.
- The mining tutorial alpha mirror is corrected to native `-1 - alpha` when
  the rising half of the 2000 ms pulse exceeds 255. The former `255 - alpha`
  expression produced a different signed value before the byte conversion.

## Rejected A/B Shapes

- Reloading `Globals::Canvas` for the two color writes before the primary
  image reduced the focused score from `48.1%` to `46.3%`.
- Reloading it only for the final saved-color restore reduced the score to
  `44.6%`.
- Replacing the dock-transfer `String spacer(" ")` lifetime with a direct
  temporary reduced the earlier baseline from `47.4%` to `47.2%`.
- Extending the dock label Y local from integer to float reduced it to `46.5%`.

None of these rejected forms remains in the source. No synthetic stack object,
volatile scratch, or register-forcing construct was introduced.

## Verification

- UCRT64 `gof2`: green; `libgof2.a` links successfully.
- ARM compile: `201/204`; the same three unrelated `SolarSystem *` versus
  integer failures remain.
- Focused `Hud::draw`: `48.1%`, `3039/3223` instructions.
- All 48 Hud functions: `88.5%` average, 26 linked-exact and 19 byte-exact.
- Exact stack reservation and VFP save set are unchanged.

## Next Boundary

The remaining `184`-instruction deficit is distributed across broad local and
hidden-return lifetimes, not an obviously missing visual branch. The next draw
pass should compare the secondary String cleanup/exception tail and the
dock-progress hidden-return slots as one whole-function lifetime package.
