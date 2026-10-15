# PlayerEgo::revive Android ARM recovery (2026-10-15)

## Evidence and restored behavior

Primary target: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`_ZN9PlayerEgo6reviveEv` at Thumb `0xa14a4`. REA/IDA pseudocode was checked
against the instructions, not treated as source code.

- `0xa14c2–0xa14d8`: pass `false` to both particle-system enable calls for
  system ID at `PlayerEgo+0x2fc`. The previous C++ derived the flag from an
  address expression and could enable the system instead.
- `0xa14dc–0xa14fc`: destroy a non-null explosion at `+0x8c`, clear it,
  then activate the follow camera and player.
- `0xa1500–0xa151e`: call `FModSound::play` twice through `Globals::sound`,
  first with its `currentMusicEvent`, then the ship sound ID at `+0x1c`;
  both use null position/velocity and zero pitch. The former code
  dereferenced an uninitialized local global pointer.
- `0xa1522–0xa1570`: show exhaust, restore maximum hull/armor, call
  `PlayerEgo::setPosition(0, 0, 10000)`, set forward/up directions to
  `(0,0,1)`/`(0,1,0)`, and clear the explosion timer. The former position
  branch wrote back the current position and skipped the call for null geometry.

ARM32 layout assertions now protect the three newly relied-on offsets:
`field_0x1c`, `explosion`, and `currentSystem`.

## Checks

The extracted-production smoke tests both explosion-present and
explosion-absent paths, particle disablement, sound IDs/order, restored
health/armor, position, direction, and timer. Focused verifier
source-shape/strict fuzzy improves
**88.4%/64.9% → 93.0%/70.3%** for `revive`; it remains non-exact.

Native `gof2`, 206/206 ARM translation units, adjacent `PlayerEgo`
smokes, the `Player` recovery/sound-table check, and 37 verifier tests
(one skipped) pass. The final full report
`_work/player_revive_final_full_20261015.json` compares **4,323/4,524**
original functions at **93.41%** average source-shape and **82.31%**
average strict fuzzy, with **2,484 linked-exact**, **1,065 byte-exact**,
**201 missing**, **123 extras**, and zero skipped. Both exact symbol
sets are unchanged from `_work/player_autopilot_full_20261015.json`.

## Limits and next work

The smoke uses small host stubs, so it does not prove a live Android ARM32
revive or FMOD behavior. The REA IDA adapter does not certify complete
function-body ranges or all indirect call edges. iOS behavior is not yet
cross-checked. Next priority: controlled ARM32 runtime/ABI observation of
the revive path, then an analogous iOS guest trace where available.
