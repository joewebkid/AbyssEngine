# ObjectGun geometry offsets (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::update(int)` begins at `0x15f964`.

The owner-geometry branch loads `Gun::offset` at `Gun+0x7c`
through `+0x84`, adds the `-100.0f` literal to its Z value at
`0x15fabe-0x15fad6`, rotates that vector by the player matrix,
and adds it to the position. The previous C++ added `8.0f` to Z.

The turret branch reads the item index at `Gun+0x58` and chooses
its local Z offset at `0x15fb32-0x15fbe0`:

| Item index | Z offset |
| ---: | ---: |
| default | 205 |
| `0x2f` | 260 |
| `0xb5` | 190 |
| `0x31` | 200 |
| `0xb4` | 172 |
| `0x30` | 170 |
| `0xe0` | 147 |
| `0xb6` | 150 |

The `0x15fd78-0x15fd9c` literal pool confirms the 205/115,
147/150, ±45, ±35 and ±16.5 constants used in the surrounding
branches. The `0xb6` and `0xe0` paths overwrite their initial
literal with 150 and 147 respectively. At `0x15fbe2`, ARM reads
only the byte at `Gun+0xa4` to enable the X offset; the previous
C++ tested the entire four-byte union. For `0xb5`, X is ±35;
for `0x30`, it is `20 - offset.x` or `offset.x - 20`; all other
items use 45 instead of 20. The Y offset uses ±13 for `0xe0`
and ±16.5 for `0xb5` when enabled by the byte at `Gun+0xa5`.
These values replace smaller, incorrect C++ constants.

`tools/run_objectgun_canvas_calls_smoke.py` extracts the owner and
turret offset expressions from the production method. Its host
fixture checks every Z selection, the X/Y flag branches, and that
a nonzero high byte at `Gun+0xa4` does not enable X when the low
byte is zero. It passes at `-O0` and `-O2`; native `gof2`, all
**207** ARM translation units and **40** verifier tests (one
existing skip) pass. Focused `update` source-shape/strict fuzzy
changes **87.9%/31.0% -> 90.8%/30.4%**. The strict fuzzy
score falls despite source-backed behavior and ABI corrections;
`update` remains nonexact.

The full ARM corpus compares **4,323/4,524** original functions,
with **201 missing**, **124 extras**, zero skipped or wrong
signatures, **93.67%/82.77%** average source-shape/strict fuzzy,
and **2,513 linked-exact / 1,074 byte-exact**. Against
`objectgun_render_semantics_full_20261016.json`, both exact
`(unit,symbol)` sets are unchanged. Full report:
`A:/Galaxy on Fire new version/_work/objectgun_update_offsets_full_20261016.json`.

The fixture checks input offsets, not the full player/turret
matrix path or live ARM32 placement on screen. Those effects
remain unverified. The next priority is first-party behavior/ABI
among the 124 extra symbols and other nonexact large functions.
