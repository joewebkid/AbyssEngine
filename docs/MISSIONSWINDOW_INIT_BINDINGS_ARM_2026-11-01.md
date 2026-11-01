# MissionsWindow initialization bindings (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `MissionsWindow::init()` at `0x14fa68`. Its initial branch
reads `Globals::iPad`, `iPadHD`, `iPadLarge`, `w` and `h`. On iPad HD it
sets width 914 and converts a float height of 703.12 to an integer;
on other iPads it selects 1300×1000 or 650×500. The old source
dereferenced unbound translation-unit pointers and used 1000 as the
iPad HD height. It now uses the observed globals and dimensions.

The original constructs tab buttons from text IDs 3219 and 129 and
uses `Globals::layout` (`0x14fb3e`–`0x14fbe2`). The 32-bit source had
separate unbound constructor shims and one unbound text ID; both
platform paths now use the typed constructors already used on 64-bit.
The campaign title uses the 164-entry ELF table at `0x20285c`.
Visibility reads `Globals::options[0x35]` and `[0x37]`, not bytes of a
mission object (`0x14fd20`).

The original replaces `#Q` with outstanding production amount,
`#*` with the target station name, and `#C` with freelance reward
(`0x14fe4e`, `0x14feda`–`0x14fef0`, `0x1500be`). It selects text 650
or 670 for the completed campaign, 174 for an empty freelance slot,
424 for accept/reject and 423 for the map button. Character images
come from `Globals::imageFactory`; the map button color is -14024449.
The recovered source now follows these branches and removes the
unbound `g_mwi_*` pointers and three local call shims.

## Verification

- Windows native and Android ARM64 static `gof2` targets built.
- Android ARMv7 compiled 209/209 translation units, zero failures.
- Focused `game/ui/MissionsWindow`: 18 comparisons, 9 linked-exact
  (previously 7) and 2 byte-exact (unchanged). `init()` source-shape
  rose from 9.2% to 92.2%; strict fuzzy from 5.3% to 35.7%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons; zero skipped, 201 missing. Average source-shape rose
  from 96.00% to 96.02%, strict fuzzy from 85.79% to 85.80%.
  Linked-exact rose from 2,724 to 2,726 through both
  `MissionsWindow` constructor variants; byte-exact stayed at 1,091.
  Complete exact-symbol set comparison found zero losses.

Other `MissionsWindow` methods still have unbound translation-unit
pointers, especially `draw()` and touch handling. The static builds
and ARM comparison do not prove that the screen is operational. Live
testing remains deferred at the user's request.
