# GameText constructor and language paths (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The `GameText` constructor (`0x8165c`) stores `0xffff` through the
`GameText::currentLang` GOT entry (`0x20fef4`) at `0x81682..0x81690`.
It constructs the string at `.rodata:0x1ca53f`, `Error Language`, and
assigns it to the object's fallback string at `0x8169c..0x816a8`.
The recovered constructor instead reset a separate local halfword and
discarded an empty temporary string. It now resets the shared state
and stores the ARM-backed fallback text. `GameText::getText` returns
that field when a key cannot be resolved.

In `GameText::setLanguage(short,int)` (`0x7f2c0`), ARM builds and
assigns a temporary `String` in each language branch before storing
the selected language. The recovered implementation used an extra
array of pointers and one shared assignment after the switch. The
direct branches now use the original `.rodata:0x1ca4cd..0x1ca53e`
filenames: `gb`, `de`, `fr`, `it`, `es`, `ru`, `pl`, `pt`, `cz`, `ar`,
`gb`, `gb`, `ptl`, `mex`, `ko`, `ja` (`.lang`), with `gb.lang` as the
default and absent-file fallback. This preserves the previously
confirmed `(language, 3401)` call contract and APK-first lookup.

Focused ARM source-shape/strict fuzzy changed as follows:

| Function | Before | After | Exact |
| --- | --- | --- | --- |
| `GameText::GameText` (both aliases) | 87.1% / 82.2% | 100.0% / 98.3% | No |
| `GameText::setLanguage(short,int)` | 51.0% / 28.6% | 98.0% / 89.3% | No |

The constructor now has 58/58 instruction count; the language
function has 339 recovered versus 346 original instructions. Native
`gof2`, 208/208 ARM units and the 93-test verifier suite (one skip)
pass. Full-corpus figures are in `PROJECT_STATUS.md`.

This does not prove a recovered live APK language load. The related
`ReadLangFile` remains **84.3% source-shape / 36.5% strict fuzzy**, and
the Android settings hash/runtime integration are not exercised here.
Next: a bounded host fixture using the authorized APK language file
to check decoded count, fallback and ownership, then review any
remaining ARM differences in `ReadLangFile`.
