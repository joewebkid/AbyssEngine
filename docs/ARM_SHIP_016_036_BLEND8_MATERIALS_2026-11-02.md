# Ships 016–036 blend-8 materials (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The recovered `BuildResourceList` registered kind-6 materials
**34416–34436** with texture 65535. The original registers each
material **34400 + n** with diffuse texture **34000 + n** and blend
value **8**, for every `n = 16..36`.

The original texture registrations at `0xed75e–0xedc0e` save IDs
**34016–34036** derived from `r8 = 0x84fa` minus `0x1a..0x06`.
The first 13 are saved at stack offsets `0x270..0x240`, and the
remaining eight at `0x208..0x1dc`. At `0xf1a62–0xf1f4c`, the
main-material constructors read those saved values, narrow them to
16 bits, and save them at `0x290..0x240`. The blend-8 constructor
run at `0xf27ae–0xf2b1e` reads those values in order; its registrations
derive IDs **34416–34436** from `r8 = 0x86c5` and pass blend 8.
Each stack source was checked for an intervening write before its
consumer. This establishes the texture arguments from the original
instructions, independently of the matching numeric suffixes.

All **21/21** diffuse texture paths registered in the source occur
once in the original Android OBB. Windows native and ARM64 static
builds, five resource-material table tests, and the full resource
table smoke at `-O0` and `-O2` pass. The smoke checks 91/91 selected
platform texture paths against the original OBB. Live rendering of
these materials remains unverified.

Focused ARMv7 verification compiled **209/209** translation units.
`BuildResourceList` remains nonexact at **77.3%** source-shape;
strict fuzzy changed **19.9% → 19.0%** while generated instructions
rose **51,831 → 51,883** against 60,817 original instructions. The
material arguments are accepted for their direct ARM evidence and
resource behavior, not because of a fuzzy-score improvement.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.20%** mean source-shape and **85.96%** mean
strict fuzzy, with **2,741 linked-exact**, **1,096 byte-exact**, zero
skipped and 201 missing. The comparison-pair set and both exact sets
are identical to the preceding ships-000–012 baseline: zero lost
pairs or exact matches.

Adjacent material 34605 is unchanged because its original texture
argument has not yet been established from the ARM stack trace.
