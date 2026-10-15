# PlayerEgo jump-drive charge in update (2026-10-15)

The Android 2.0.16 ARM binary is the primary source. A targeted REA/IDA
read of `PlayerEgo::update` (`0x9ce38`) shows the missing Thumb block at
`0x9d8a6–0x9d8f6`. The byte at `PlayerEgo+0x204` gates the block. The
frame interval at `+0x134` is added to the charge at `+0x1fc` and
compared with the maximum at `+0x200`. Reaching the maximum writes zero
to the byte at `+0x38`, then calls `Hud::hudEvent(26, this, 0)`.
The later branch tests `volatileGoods` at `+0x398` and adds
`float(dt) * 0.001f * 0.5f` to `Player::flShake` at `+0x60`.

The C++ mirrors that order in `PlayerEgo::update`. ARM-only layout
assertions pin the three charge fields. No drive-flag reset was added:
this instruction block does not show one, so a repeated event in
successive frames remains possible until some other code changes the
state. The write to `+0x38` is only one byte of the currently named
`gunMuzzleRoot` pointer. Its higher-level meaning and pointer safety
are unresolved; the implementation preserves the observed byte store,
not an invented whole-pointer assignment. REA reports 90 resolved direct
callee addresses for `update`, with partial relationship coverage;
unresolved indirect calls and unrecognized targets are not ruled out.

The extracted-production smoke covers inactive charging, sub-threshold
growth, volatile shake, exact-threshold HUD event, the low-byte write,
and the unchanged charging flag. Adjacent jump-drive and update-beam
smokes pass. Native `gof2` and all 206 ARM translation units compile;
37 verifier tests pass with one skip. Focused `update` source-shape
rises **79.5% → 80.1%** (2,138 → 2,171 source instructions against
2,913 original), while strict fuzzy falls **32.3% → 30.8%**. The
function is still neither linked-exact nor byte-exact. We retain the
evidence-backed behavior rather than optimize solely for this metric.

The full ARM report `_work/player_drive_charge_full_20261015.json`
compares **4,323/4,524** original functions: **201 missing**,
**123 extras**, zero skipped, **93.41%** average source-shape,
**82.31%** strict fuzzy, **2,484 linked-exact**, and **1,065
byte-exact**. Both exact symbol sets are unchanged from
`_work/player_jump_full_20261015.json`.

Supplementary runtime path: the local iOS GOF2 HD 1.1.4 IPA contains
a decrypted ARMv7 Mach-O (`www.fishlabs.net.gof2hd`, minimum iOS 4.3;
executable SHA-256 `74ece5bedf561eed4e71b8d0b875e201c47858a2649a59d65af57390a0554958`).
The separate SuperDuperHyperHle project records a GOF2 first-battle
run and has a guest GDB remote server (`src/gdb.rs`, `--gdb=`), making
bounded breakpoints/register/memory reads a candidate for checking the
analogous iOS charging path. No such live trace was taken here. iOS
1.1.4 differs from Android 2.0.16, so matching behavior would
corroborate semantics, not Android byte-exact code or an identical
object layout. Next: identify the analogous iOS symbol/addresses, run a
controlled charge-to-jump trace, then recover the flight transition.
