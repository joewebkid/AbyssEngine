# Ships 013–036 blend-0 materials (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The recovered `BuildResourceList` used texture 65535 for all 24
kind-6 materials **34313–34336**. The original passes texture
**34000 + n** to material **34300 + n** for each `n = 13..36`,
with blend value **0** (`BlendMode_dummy`).

For 013–015, the original explicitly forms the texture arguments
from `0x84fa - 0x1d`, `0x84fa - 0x1c`, and `0x84fa - 0x1b` at
`0xf3152–0xf31c2`; the corresponding registrations are at
`0xf3162–0xf31dc`. For 016–036, the original texture registrations
at `0xed75e–0xedc0e` save 34016–34036. Main-material constructors
at `0xf1a62–0xf1f4c` narrow and save those IDs at stack offsets
`0x290..0x240`. Blend-0 constructors at `0xf31ee–0xf3562` read
those same slots in order and register 34316–34336. Each slot's
last write before the blend-0 read was checked. The assignments
come from original instruction flow, not from the ID pattern alone.

Textures **34016–34036** each have a registration and one matching
file in the original Android OBB (**21/21**). Textures **34013–34015**
have neither registrations nor `ship_013_`–`ship_015_` files in that
OBB. The original materials still pass those IDs; visual behavior
for the three dangling references is unverified.

Windows native and ARM64 static builds pass. Five resource-material
table tests and the full resource-table smoke at `-O0`/`-O2` pass;
its original-OBB check finds all 91 selected platform texture paths.
Focused ARMv7 verification compiles **209/209** translation units.
`BuildResourceList` remains nonexact at **77.3%** source-shape;
strict fuzzy changes **19.1% → 17.8%**, and generated instructions
change **51,865 → 51,874** against 60,817 original instructions.
The links are retained for direct ARM behavior evidence, not for a
fuzzy-score improvement.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.20%** mean source-shape and **85.96%** mean
strict fuzzy, with **2,741 linked-exact**, **1,096 byte-exact**, zero
skipped and 201 missing. The comparison-pair set and both exact sets
are identical to the preceding ships-000–012 blend-0 baseline: zero
lost pairs or exact matches.
