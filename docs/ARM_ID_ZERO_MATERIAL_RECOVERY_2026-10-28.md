# ARM material resource ID recovery (2026-10-28)

## Source evidence

The original Android ARM `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
supplies the following values in `BuildResourceList`. The previous source
contained zero placeholders in three material records and their texture links.

| Slot | Material ID | Texture IDs | ARM evidence |
| --- | ---: | --- | --- |
| 612 | 34704 | 33122, 33123 | `0xeadc6–0xeadcc` forms ID; constructor at `0xf12dc–0xf1306` |
| 694 | 34263 | 34063, 34163 | ID `0xec698–0xec69c` / `0xf254e–0xf2552`; textures `0xf253c–0xf2546` |
| 750 | 34463 | 34063 | `0xf2eea` passes the texture; `0xf2f04` stores slot 750 |
| 835 | 34363 | 34063 | ID `0xec710–0xec714`; texture passed at `0xf3dca` |
| 836 | 32563 | 34063 | texture passed at `0xf3df8` |

The common texture value 34063 is formed as 34042 + 21 at `0xee1c0`, stored
at `[sp+160]` at `0xef340`, loaded at `0xf2378`, and kept in `fp` from
`0xf253e` through the calls for slots 694, 750, 835 and 836. The second
texture of slot 694 is 34042 + 121, stored at `[sp+156]` at `0xef300` and
loaded at `0xf253c`. Linear objdump output hides the `0xef300` store because
it decodes the preceding literal pool as an instruction; the branch at
`0xef2fa` reaches `0xef300`.

No kind check was added to `PaintCanvas::TextureCreate`: the original ARM
function has none. No kind-2 registrations for texture IDs 33122, 33123,
34063 or 34163 were established by this change. Their availability at
runtime remains unverified.

## Runtime result

The previous two-texture material in slot 694 had ID 0 and texture IDs 0/0.
Its first eight payload bytes are `00 00 00 00 ff ff ff ff`, the same bit
pattern as the invalid pointer `0xffffffff00000000` seen when a material
payload was interpreted as a texture path. `FindResource` searches by ID
without checking resource kind. This is a strong explanation for that crash,
but the exact runtime lookup chain was not captured before the change.

A signed ARM64 APK with this change installed on the `25053RT47C` test phone.
The old pointer crash was not observed on the next run. The process progressed
to `PlayerEgo::PlayerEgo`, then crashed at a null dereference in
`Globals::addSoundResourceToList(int)` while creating the scene. This is the
next confirmed startup blocker. The ARM64 backtrace resolves its caller to
`PlayerEgo.cpp:1780`, which passes the still-null `Globals::gGlobals` pointer;
initialization currently sets `Globals::globals`. The original ARM call path
must be checked before repairing that distinct issue. The APK is
`D:\gof2-arm64-crypto\gof2-arm64-idzero-material-debug-20261028.apk`
(SHA-256 `c78434dd3c5c820fa645802f88b695b351e7831218480f8c683a1cb16ecab5b0`).
It packages the linked ARM64 library with 53 JNI exports and passes APK v1,
v2 and v3 signature verification. Sound remains disabled in this test build.

## Verification

Native and ARM64 static libraries build; the ARMv7 source compiles 209/209
translation units. Focused `BuildResourceList` source-shape remains 77.0%,
while strict fuzzy changes from 18.4% to 18.1%. The complete ARM corpus
compares 4,323/4,524 original functions across 4,745 pairs with zero skips:
94.86% source-shape, 84.43% strict fuzzy, 2,638 linked-exact and 1,086
byte-exact. Relative to the previous full report, no compared pair or exact
match was lost.

Next, investigate the null sound resource holder against original ARM and
the silent-build path, then repeat the foreground device test. Keep the
material texture registrations under review if runtime evidence reaches them.
