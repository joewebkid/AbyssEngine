# Station material texture links (2026-10-28)

## Why this group

The preceding ARM64 test APK reached `CutScene::initialize` and crashed in
`String::Set(char const*)` while opening a texture from
`PaintCanvas::MaterialCreate`. The fault address `0xffffffff00000000`
matches the first eight bytes of an ID-0 `ResourceMaterial` interpreted as
a 64-bit path pointer. The tombstone does not identify the selected station
or material. `BuildResourceList` has several zero-valued texture links, so
this section restores only one adjacent, source-confirmed station group.

## Android ARM evidence

Original `libgof2hdaa.so` SHA-256:
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
In `BuildResourceList` at `0xf0f24–0xf11d6`, the calls to the one- and
two-texture `ResourceMaterial` constructors pair these IDs with their
texture arguments. The earlier texture-resource registrations at
`0xed14c–0xed3b0` define the stack-held arguments. The resulting
material-to-texture links are:

| Material IDs | Source-backed texture IDs |
| --- | --- |
| 20004, 20003 | 33010/33011; 33010 |
| 20005, 20006 | 33006/33007; 33006 |
| 20104, 20105 | 33008/33009; 33008 |
| 20007, 20008 | 33004/33005; 33004 |
| 20045, 20047, 20082 | 33012/33013; 33012; 33012 |
| 20032, 20031, 20033 | 33014/33015; 33014; 33014 |

All referenced texture IDs have kind-2 resource registrations in the same
list. The 14 material records are slots 593–600 and 602–607. Blend values
and constructor overloads were preserved.

## Verification and limits

Native and ARM64 static libraries build; ARMv7 builds 209/209 translation
units. The focused `BuildResourceList` function moves from **76.9% to 77.0%
source-shape**. Strict fuzzy moves from **18.8% to 18.4%**; this large
function is neither linked-exact nor byte-exact. The full ARM corpus compares
**4,323/4,524** originals across **4,745** pairs: **94.86% source-shape**,
**84.43% strict fuzzy**, **2,638 linked-exact**, **1,086 byte-exact**, and
zero skips. The pair set and both exact-match sets have no losses against
the preceding full report. The complete output is
`D:\gof2-arm64-crypto\station_material_full_20261028.json`.

The signed ARM64 test package is
`D:\gof2-arm64-crypto\gof2-arm64-station-material-debug-20261028.apk`
(SHA-256 `2938898fb09dd739bed14e93a57ab838311f570c811d3ae74825bd8381dff7d7`).
Its embedded library matches the linked output byte for byte and retains
52 Java JNI entry points plus `JNI_OnLoad`; the APK verifies under v1, v2,
and v3 signature schemes. It installed on `25053RT47C`, but the first run
still crashed with the same invalid path-pointer value. The selected
material was not recorded. A separate diagnostic APK was installed to log
the material ID before a future crash; the device switched to another app
before that late transition. Diagnostic logging was removed from the source
and is absent from the signed package above.

There are still 26 `ResourceMaterial` entries with literal zero texture
arguments, including three resources whose own ID is zero. This count does
not establish which entries are wrong: some may intentionally carry a zero
field. The recovered station group alone cannot prove that every random
station or later cutscene will load. A type guard in `TextureCreate` could
avoid interpreting a material as a texture, but the original behavior for
that guard has not been established and it would not restore missing links.
