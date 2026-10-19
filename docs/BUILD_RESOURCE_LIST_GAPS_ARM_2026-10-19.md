# BuildResourceList missing-slot recovery (2026-10-19)

Primary evidence is Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The function begins at `0xe8f54`. Thumb disassembly shows the original
passing 2,488 pointers to `PaintCanvas::SetResourceList` at `0x10a1b4`–
`0x10a1c2`. The recovered source previously assigned only 2,434 of those
array positions, leaving 54 uninitialized. This is an incomplete table,
not evidence that the original intentionally registered null slots.

The following ten missing records are restored from their ARM stack-slot
stores, IDs, constructor arguments and PC-relative strings. The store
offset for the shown low slots is `sp + 788 + slot*4`; the last record uses
`r8 + 9,948` before the `SetResourceList` call.

| Slot | ARM store | ID | Payload |
| ---: | --- | ---: | --- |
| 522 | `0xf0084` | 33410 | Texture `skybox_asteroid_belt_diffuse.aei`, value 0 |
| 525 | `0xf012c` | 33240 | Texture `container_003_terran_diffuse.aei`, value 0 |
| 538 | `0xf03e0` | 33301 | Texture `explosion_emp.aei`, value 0 |
| 540 | `0xf0444` | 33340 | Texture `stations_pirates_dmg_diffuse.aei`, value 0 |
| 571 | `0xf0aa8` | 33302 | Texture `bomb_rocket_fx.aei`, value 0 |
| 577 | `0xf0bf8` | 33311 | Texture `bomb_emp_a_normal_specular.aei`, value 0 |
| 582 | `0xf0d04` | 33330 | Texture `explosion_void_station.aei`, value 0 |
| 589 | `0xf0e6a` | 33303 | Material `(33302, BlendMode_2)` |
| 592 | `0xf0f12` | 33312 | Material `(33310, 33311, BlendMode 28)` |
| 2487 | `0x10a1bc` | 14287 | Mesh `explosion_void_station_alpha_lookat.aem`, material 27302, clone flag false |

The eight recovered file paths exist in the extracted Android asset set.
The asteroid-belt texture is an AEI type-3 uncompressed image, decoded as
format 3; the earlier four skybox files in the host gate decode as format
11. The asset-backed gate now checks record 33410's path and value, loads
the original file, and checks cached reuse at `-O0/-O2`.
Its SHA-256 is `94aeb7e8e1f50f3d3c13374f4cece5f2fb50974aac5a6272cd99b46be784a036`.
The last mesh also passes the in-process resource-ID `MeshCreate` handoff
through production `MeshCreateFromFile`/`MeshReadData`; its animation
collection is observed after the file closes. Its SHA-256 is
`323ba7f1228a5df7df582d8014adccb1a16f4e345675192b5114931c06ca0ace`.
Animated-mesh clone-on-reuse is not exercised by this gate.

After the first ten-record recovery, the table still had **44 unrecovered positions**: 608, 609, 611, 629,
632, 634, 637–640, 643, 645–648, 650–653, 655–658, 660–663, 665,
667, 669, 673, 681, 896, 897, 899, 900, 977, 997, 998, 1700,
1871, 1874, 1877 and 1883. The C++ stack array is now zero-initialized,
so these holes contain null pointers instead of indeterminate host stack
bytes. This is a **host-safety workaround**, not a claim that Android ARM
zeroed the array or that the missing resources were optional. Complete
`BuildResourceList` execution and all resource consumers are **not
validated**. The next priority is the concentrated 608–681 gap cluster,
then a complete-table host gate. The `SetResourceList` handoff is recovered
in the follow-up below; the static-library build alone did not prove it.

Validation at this revision: native `gof2` and all 208 ARM translation
units build; the asset-backed host gate passes at `-O0/-O2`; 40 verifier
unit tests pass (one existing skip). The full ARM corpus compares
4,323/4,524 original functions, with 2,529 linked-exact and 1,076
byte-exact matches. Both exact-match sets are unchanged from the prior
full report. Corpus mean source-shape/strict fuzzy remains 93.87%/83.06%.
Focused `BuildResourceList` source-shape moves from 73.5% to 73.8%; its
strict fuzzy moves from 22.8% to 21.7%, so this is a semantic/table
coverage gain, not an across-the-board machine-code similarity gain.

## Follow-up: material cluster 608–629

The same Android ARM function confirms four more missing material records:

| Slot | ARM store | ID | Constructor payload |
| ---: | --- | ---: | --- |
| 608 | `0xf122c` | 34700 | texture IDs 33120, 33121; blend 28 |
| 609 | `0xf126a` | 34701 | texture ID 34500; blend 2 |
| 611 | `0xf12ca` | 34703 | texture ID 33120; blend 0 |
| 629 | `0xf1678` | 34601 | texture ID 34501; blend 2 |

Slot 610 already existed, but incorrectly used texture ID 0. The ARM
constructor at `0xf127c`–`0xf1280` passes 33120 with blend 8; source now
matches that pair. Slot 609's 34500 value is traced from the ARM
`r6 - 1` value used for resource 293, and the original slot 293 record
also has ID 34500. These are table/constructor facts, not proof that a
complete resource-list consumer can resolve all referenced textures;
IDs 33120 and 33121 do not yet have direct records in the recovered C++
table. The remaining unrecovered slot count is 40. A complete-table host
gate remains necessary.

The follow-up builds natively and compiles all 208 ARM units. The
asset-backed skybox/mesh gate passes at `-O0/-O2`; 40 verifier tests pass
(one existing skip). The full corpus still compares 4,323/4,524 original
functions and retains all 2,529 linked-exact and 1,076 byte-exact pairs
without losses. Mean source-shape/strict fuzzy is 93.87%/83.05%, versus
93.87%/83.06% before this follow-up. Focused `BuildResourceList`
source-shape is 73.9%, versus 73.8%; strict fuzzy falls from 21.7% to
13.6%. This is a recorded strict-fuzzy metric regression despite the
ARM-backed table corrections; exact matching of this giant function
remains open.

## Follow-up: bulk resource registration

Android ARM `PaintCanvas::SetResourceList` at `0x77f40`–`0x77f4a`
rearranges its three arguments and tail-calls the bulk
`ArrayAdd<Resource*>(Resource* const*, unsigned int, Array<Resource*>&)`
specialization, defined at `0x77f4e`. The existing C++ body instead
forwarded to unresolved `paintcanvas_ext_set_reslist`. It now calls the
already-instantiated `ArrayAdd` directly, leaving the ARM function
100% source-shape/strict fuzzy and linked-exact. The native PaintCanvas
object now refers to the defined bulk-add specialization rather than
the unresolved shim.

The existing asset-backed `Level` host gate now source-extracts this
production `SetResourceList` body, bulk-registers its four selected
table records, checks their order, and then exercises the same resource-ID
mesh and texture paths against original Android files at `-O0/-O2`.
This validates the handoff on a selected subset, not execution of all
2,488 table entries or their consumers. Full-table registration remains
the next integration gate.
Native `gof2`, 208/208 ARM units and 40 verifier tests pass (one existing
skip). The full ARM corpus remains 4,323/4,524 compared, mean
source-shape/strict fuzzy 93.87%/83.05%, 2,529 linked-exact and 1,076
byte-exact, with no exact-pair losses against the material-cluster report.

## Follow-up: ship-material run

The contiguous ARM stores from `0xf1776` through `0xf212e` contain 41
ship-material records at slots 634–674. Every record calls the two-texture
`ResourceMaterial` constructor (PLT `0x65e74`) with blend 28. The first
texture IDs come from the earlier diffuse resource records (34000–34012,
34016–34041, and 34043); the second IDs come from their matching normal/
specular records (34100–34112, 34116–34141, and 34143). Slot 673 uses
34042 and 34142; the latter has a direct C++ record, while 34042 does not.
The ARM call at `0xf20d6` explicitly uses 34042 for slot 673, so this
unresolved reference is preserved rather than replaced with a guessed
texture. The intervening absent ID suffixes 13–15 are also preserved.

The same ARM function supplies slot 632 at `0xf1708` as material 34604
using texture 34504, blend 2. Slot 681 at `0xf22aa` is material 34250
using texture IDs 34050/34150, blend 28. Neither of those two texture
IDs has a direct record in the recovered C++ table yet. In total this
follow-up adds 28 omitted records and corrects 15 previously present
ship-material payloads that used placeholder texture IDs. The 2,488-slot
table now has **12 unrecovered positions**: 896, 897, 899, 900, 977,
997, 998, 1700, 1871, 1874, 1877 and 1883.

The source-record test checks all 41 ship-material ID/texture pairs and
the two edge records. It does not exercise their render-time consumers.
The complete `BuildResourceList` host path and full `Level::init` remain
unverified; the remaining 12 slots and these unresolved texture references
are the next priority.
Native `gof2` and all 208 ARM units build. The selected asset-backed
`SetResourceList`/loader gate passes at `-O0/-O2`; 42 verifier tests pass
(one existing skip). Focused `BuildResourceList` source-shape/strict fuzzy
improves from 73.9%/13.6% to 74.7%/17.6%, still nonexact. Full ARM
corpus remains 4,323/4,524 compared, mean 93.87%/83.05%, 2,529
linked-exact and 1,076 byte-exact; comparison of the exact-pair sets
against the preceding full report shows zero losses.

## Follow-up: visitor-glow materials and relocated meshes

The ARM table stores at `0xef2ea`–`0xef3e4` place five texture records
at slots 457–461: IDs 33400–33404, with the original `terran`, `vossk`,
`nivelian`, `midorian` glow paths and `bar_visitor_shadow.aei`. Source
previously put planet meshes 18180/18186 into slots 457/459, assigned
ID 0 to the Vossk texture, and omitted the Terran/Nivelian textures.
The ARM PC-relative strings and halfword ID stores confirm the corrected
order. All five asset paths exist in the extracted Android asset set.

The ARM material stores at `0xf49c6`–`0xf4aac` place IDs 34800–34804
at slots 896–900. Their constructor inputs are texture IDs
33400–33404 respectively; slots 896–899 use blend 2 and slot 900
uses blend 1. The value 33402 for slot 898 is traced from `r9` at
`0xef35c` through `[sp,#124]` at `0xf0052`; its old C++ placeholder
65535 was incorrect. Similarly, the other arguments trace through
`r6+99`, saved `r8`, and `[sp,#132]`/`[sp,#128]`. These records now
have corresponding direct texture records in C++.

The original planet meshes are at slots 1877 (`0x1012ce`, ID 18180,
`map_planet_000.aem`) and 1883 (`0x101436`, ID 18186,
`map_planet_006.aem`), both using material 20027 and clone flag false.
The two remaining holes in the nearby plane run are slots 1871
(`0x101166`, ID 18083, material 27284) and 1874 (`0x10121a`, ID 18086,
material 27287). The ARM halfword stores also correct existing slot
1869 from ID 27281 to 18081 and slot 1876 from 27286 to 18088;
their material arguments stay 27281 and 27286. The relevant asset
paths exist in the Android extract. These are source-backed table
facts, not proof of full-table registration or rendering.

Eight omitted slots are restored, leaving four unresolved positions:
977, 997, 998 and 1700. The source-record regression test covers
the restored records and IDs. Native and 208/208 ARM translation units
build; 44 verifier tests pass (one pre-existing skip), and the selected
asset-backed skybox gate passes at `-O0/-O2`. Focused
`BuildResourceList` source-shape/strict fuzzy moves from 74.7%/17.6%
to 74.9%/18.4%, still nonexact. The complete 2,488-entry table and
its consumers remain unexecuted in the host gate.
The fresh full corpus compares 4,323/4,524 original functions, with
mean source-shape/strict fuzzy 93.87%/83.05%, 2,529 linked-exact and
1,076 byte-exact. The exact-pair sets have zero losses against the
preceding full report; 201 original functions remain missing.

## Follow-up: final four table positions and full registration gate

Android ARM confirms the four remaining records and a misplaced
texture/mesh pair:

| Slot | ARM store | ID | Payload |
| ---: | --- | ---: | --- |
| 310 | `0xed11e` | 33010 | Texture `stations_terran_diffuse.aei`, value 0 |
| 977 | `0xf5a8c` | 34817 | Material `(34816, blend 2)` |
| 997 | `0xf5ea6` | 34810 | Material `(24204, 24205, blend 28)` |
| 998 | `0xf5ee8` | 34811 | Material `(24206, 24207, blend 28)` |
| 1700 | `0xfe90c` | 6801 | Mesh `projectile_025_anim_add.aem`, material 24201, clone false |

The original slot 310 gets ID 33010 from the halfword at `0xed11a`;
its PC-relative string points to `stations_terran_diffuse.aei`.
ID 6801 is constructed at `0xfe8e2`–`0xfe90c` and stored at
`r8+6800`, which is slot 1700, not 310. Both asset paths exist in
the extracted Android data. Material IDs and constructor arguments
are the immediate values around the listed ARM stores. The table now
assigns every position 0–2487 exactly once, so the temporary host-only
zero-initialization is removed.

The new host gate source-extracts all 2,488 production `ADD_RES`
statements, the resource constructors, and the production
`PaintCanvas::SetResourceList` body. At both `-O0` and `-O2`, it
executes the entire table construction and bulk registration, checks
2,488 non-null records and the five corrected records above. Its
canvas shell is bounded: this does not execute the initial
`TextureCreateGlobal`, the post-table `AddResource` tail,
`loadPortraits`, `loadLowTexturesAndMaterials`, GPU services, or
`Level::init`. Those paths remain separate integration work.
Native `gof2` and 208/208 ARM units build; 45 verifier tests pass
(one existing skip), and the selected original-asset gate remains green
at `-O0/-O2`. Focused `BuildResourceList` source-shape/strict fuzzy
improves 74.9%/18.4% to 75.0%/21.2%, still nonexact. The full corpus
compares 4,323/4,524 functions, with mean source-shape/strict fuzzy
93.87%/83.06%, 2,529 linked-exact and 1,076 byte-exact. No prior
exact pair was lost; 201 original functions remain missing.

## Follow-up: iPadHD post-table resource branch

After the 152 generic image entries, Android ARM reads a relocated
`Globals::iPadHD` byte through GOT slot `0x21038c` at `0x10a25a`–
`0x10a262`. A zero value branches to `0x10a6c4`, skipping the 1440
texture package; a nonzero value constructs its first twelve records at
`0x10a274`–`0x10a4ca`. The C++ tail had registered these textures
unconditionally. It now guards the package with `iPadHD`.

The last record in the first twelve was a null placeholder. ARM loads
`data/textures/gof2_shop_button_ipad_1440.aei` at `0x10a4a8`–
`0x10a4b0`, then sets ID 27343 at `0x10a4b4`. The following iPad-large
sub-branch has a *second* ID 27343: ARM loads
`data/textures/gof2_shop_button_ipad_large.aei` at `0x10a656`–
`0x10a65e`, sets the ID at `0x10a664`, and uses the shared add-resource
continuation at `0x10bf32`. Both files exist in the extracted assets.

The bounded host gate source-extracts this branch in addition to the
2,488-entry table: with `iPadHD=0` it adds no 1440 entries, and with
`iPadHD=1` it appends 12 records and checks the first font and final
shop-button path/ID. This exercises both outcomes at `-O0/-O2`.
The later `retinaDisplay`, iPad-large, default-font and portrait
branches are not reconstructed by this change; the complete
`BuildResourceList` function remains unverified end to end.
Native `gof2` and 208/208 ARM units build; 45 verifier tests pass (one
existing skip), and both the selected original-asset gate and extended
table/branch gate pass at `-O0/-O2`. Focused `BuildResourceList`
source-shape/strict fuzzy stays 75.0%/21.2%, nonexact. Full corpus
remains 4,323/4,524 compared, 93.87%/83.06% mean source-shape/strict
fuzzy, 2,529 linked-exact and 1,076 byte-exact, with zero previous
exact-pair losses.

## Follow-up: nested iPad/Retina atlas package

The previous branch conflated its two shop-button records. The first
record's ARM PC-relative string is `_ipad_1440.aei` (`0x10a4a8`), not
`_ipad_large.aei`; the latter is constructed at `0x10a656`. After the
first twelve records, ARM reads `Globals::iPad` through GOT slot
`0x21032c` (`0x10a4ce`–`0x10a4d8`); false branches to `0x10aa34`.
For iPad, it reads `Globals::retinaDisplay` through GOT slot `0x2103e4`
(`0x10a4dc`–`0x10a4e6`); false branches to `0x10be20`. Only the
true/true path constructs the seven existing `ipad_large` atlas entries
and the second shop button before joining at `0x10bf32`.

The C++ now nests those eight records under both flags and uses the
correct first shop-button path. The source-extracted host gate tests
`iPadHD=0`, `iPadHD=1/iPad=0`, `iPadHD=1/iPad=1/Retina=0`, and all flags
enabled at `-O0/-O2`. The false-path additions at `0x10aa34` and
`0x10be20`, plus the separate non-HD platform groups, remain to be
recovered; these isolated branch tests do not imply complete
`BuildResourceList` execution.
Native `gof2` and all 208 ARM translation units build; 45 verifier
tests pass (one existing skip). The original-asset gate and the
expanded full-table gate pass at `-O0/-O2`. Focused `BuildResourceList`
source-shape/strict fuzzy is 75.0%/21.3%, against 75.0%/21.2%
previously; it is not exact. The full corpus remains 4,323/4,524
compared, mean source-shape/strict fuzzy 93.87%/83.06%, 2,529
linked-exact and 1,076 byte-exact, with zero exact-pair losses.
