# Ships 000–012 blend-0 materials (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The recovered `BuildResourceList` passed texture 34042 to material
34300 and 65535 to materials 34301–34312. The original constructs
kind-6 materials **34300–34312** with textures **34000–34012**
respectively and blend value **0** (`BlendMode_dummy`).

The original texture registrations at `0xed438–0xed722` derive
34000–34012 and save them at stack offsets `0x2a4..0x274`.
The material constructors at `0xf1750–0xf1a2a` narrow the saved
values, placing 34000 at `0x1f8` and 34001–34012 at `0x2c0..0x294`.
At `0xf256e`, the original loads 34000 into `r6`; it stays there
until the first blend-0 constructor at `0xf2f16`. Constructors
`0xf2f42–0xf312a` read the other twelve values from those stack
slots. Each constructor passes `r2 = 0` and registers the matching
34300–34312 ID. Material 34304 uses an immediate ID at `0xf2fce`,
but its texture argument follows the same stack chain. This is
instruction-backed, not inferred from adjacent numbering.

All **13/13** diffuse texture paths exist in the original Android
OBB. Live rendering remains unverified.

Windows native and ARM64 static builds pass. Five resource-material
table tests and the full resource-table smoke at `-O0`/`-O2` pass;
its original-OBB check finds all 91 selected platform texture paths.
Focused ARMv7 verification compiles **209/209** translation units.
`BuildResourceList` remains nonexact at **77.3%** source-shape;
strict fuzzy rises **18.0% → 19.1%**, and generated instructions
rise **51,853 → 51,865** against 60,817 original instructions.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.20%** mean source-shape and **85.96%** mean
strict fuzzy, with **2,741 linked-exact**, **1,096 byte-exact**, zero
skipped and 201 missing. The comparison-pair set and both exact sets
are identical to the preceding ships-042–063 baseline: zero lost
pairs or exact matches.

Separately, original ARM `PaintCanvas::MaterialCreate` at
`0x79e9c–0x79ece` exits its texture loop when `FindResource`
returns null, then copies material fields and registers the material
at `0x79ece–0x79efc`. This confirms that the earlier observed
unregistered texture IDs 34050/34053 do not themselves abort material
registration. The reconstructed `MaterialCreate` already follows
that path; the visual result of those incomplete materials is not
verified.
