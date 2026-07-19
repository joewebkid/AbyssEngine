# Hud Core-Frame Pass

Date: 2026-07-20

## Scope

This pass is based on the Android ARM bodies at `0x1604e4` (`Hud::init`),
`0x1636f4` (`drawEventString`), `0x1637e0` (`drawChallengeModeScore`),
`0x163b90` (`Hud::draw`), `0x1622f4` (`drawEventQueue`), and `0x166018`
(`drawOrbitInformation`). The same function bodies are present in:

- `analysis/gof2_libgof2hdaa_full_ida.c`
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`

## Confirmed And Applied

- HUD canvas, font, screen size, layout, and localized strings route through
  `Globals::{Canvas,font,w,h,layout,gameText}` rather than null `g_Hud_*`
  shim pointers.
- `Hud::draw` obtains the live `Level` from `PlayerEgo::level`.
- `Hud::init` now replays the Android `Image2DCreate` resource map for every
  observed init-time image slot. Known local roles retain their names; the
  remaining source fields are preserved in `HudInitImageSlots` by their
  original 32-bit byte offsets rather than given speculative semantic names.
- The ARM body at `0x1604e4` directly confirms the event queue length as
  `0x14` (20 entries). The local initialization now uses that capacity.
- The recovered layout/image coordinate formulas cover the lock, event,
  secondary-weapon, orbit, pause, hacking, quick-menu, and shield/armor
  setup. `Globals::pause_x` and `Globals::pause_y` now receive the resulting
  pause-button location directly.
- The iPad branch creates the extra `0x4c6`/`0x6aa` images and routes control
  placement through `Globals::setCoordsSteer` / `setCoordsFire`, using the
  persisted `GameSettings::{steerAnchorX,fireAnchorX}` values. The fallback
  anchor table (830/583/415 and 730/513/365) is independently corroborated
  by the recovered `MenuTouchWindow` settings body.
- `drawEventString`, `updateSecondaryWeaponString`, `drawEventQueue`, and the
  station/system/security text flow in `drawOrbitInformation` now use native
  call routing and game-text IDs.
- `menuLevel` is typed as `Level *`; the quick-menu branch uses `menuType`,
  matching the native function parameter rather than comparing a pointer value
  against an integer.
- Local ARM `.rodata` confirms `word_203758` faction-logo resources as
  `0x4a6`, `0x4a3`, `0x4a5`, and `0x4a4`, and confirms `byte_203780` security
  RGB rows as `(255,42,0)`, `(255,108,0)`, `(237,237,0)`, and `(237,0,0)`.

## Needs Confirmation

- Semantic names and downstream draw consumers for most `HudInitImageSlots`
  remain unproven. Their resource IDs and original field slots are confirmed;
  a semantic role is not inferred from the ID alone.
- `quickMenuHeaderImage` has no observed `Image2DCreate` assignment in this
  `Hud::init` body. Its producing path, `initHudMenu`, cargo text templates,
  challenge-score state fields, and the majority of `Hud::draw` remain
  separate recovery work.
- `HudInitImageSlots` and the raw coordinate members are a host-side source
  mirror, not a claim that the 64-bit C++ class has the original ARM ABI or is
  byte-identical.

## Validation

`cmake --build cmake-build-ucrt --target gof2 -- -k 0` linked `libgof2.a`.
This host-native build is a compile gate only; no ARM byte-match result is
claimed for this package.
