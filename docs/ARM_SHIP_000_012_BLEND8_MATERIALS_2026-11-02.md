# Ships 000–012 blend-8 materials (2026-11-02)

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The recovered `BuildResourceList` gave material 34400 the wrong texture
34042 and left materials 34401–34412 with texture 65535. The original
registers kind-6 material **34400 + n** with diffuse texture
**34000 + n** and blend value **8**, for each `n = 0..12`.

The texture registrations at `0xed438–0xed722` derive 34000–34012
from `r8 = 0x84fa` minus `0x2a..0x1e`; the IDs are saved in stack slots
`0x2a4..0x274`. The earlier main-material constructors at
`0xf1750–0xf1a2a` narrow those saved IDs and preserve them in the
slots later read by the blend-8 constructor run. At `0xf22ce`, `r8`
becomes `0x86c5`; the registrations at `0xf2572–0xf278e` use
`r8 - 0x65..0x59` for IDs 34400–34412 and pass the corresponding
saved texture IDs with blend 8. This establishes both the material
IDs and their textures without inferring from numbering alone.

The original Android OBB contains all **13/13** referenced diffuse
textures. Windows native and ARM64 static builds pass. The strict
ARM64 shared link has no unresolved symbols and retains the previous
52 Java JNI exports plus `JNI_OnLoad`. Five resource-material table
tests and the `-O0`/`-O2` full resource-table smoke pass; its original
OBB check finds all 91 selected platform texture paths. Actual
rendering of these materials is unverified.

Focused ARMv7 `BuildResourceList` verification compiled **209/209**
translation units. Against the preceding pirate-glow baseline, its
source-shape score rose **77.2% → 77.3%** and strict fuzzy rose
**18.7% → 19.9%**; the function remains nonexact. The full ARM corpus
compared **4,323/4,524** original functions in **4,762** pairs, with
**96.20%** mean source-shape, **85.96%** mean strict fuzzy,
**2,741 linked-exact** and **1,096 byte-exact**, zero skipped and
201 missing. The pair set and both exact sets are unchanged: no
previously exact match was lost.

Adjacent material 34605 is **not changed**. Its original constructor
at `0xf171a` reads a texture value from `[sp, #0x2a8]`, but a
corresponding earlier direct write has not been established from the
available ARM trace. Texture 34505 exists, yet assigning it to 34605
from the numbering pattern alone would be unsupported. This remains
an open ABI/behavior question rather than a claimed recovery.
