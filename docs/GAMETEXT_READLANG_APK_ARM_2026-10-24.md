# GameText language reader: ARM shape and original APK gate (2026-10-24)

Primary code reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`GameText::ReadLangFile(unsigned,int)` spans `0x7f64c..0x7f796`.
After reading a two-byte big-endian length, it stores the swapped
16-bit value, allocates one extra byte, reads the UTF-8 payload and
null-terminates it. It allocates each `String` before the language-9
branch. The ordinary branch constructs directly from the converted
wide text; the Arabic branch constructs a temporary wide string and
returns `convertStringFromArabic` into the allocated object. The
recovered reader now follows this order rather than default-constructing
and assigning a `String` in either branch.

Focused ARM source-shape/strict fuzzy changed from **84.3%/36.5%** to
**91.7%/50.8%**. This function is still nonexact. All **208/208** ARM
translation units, native `gof2`, and the **93-test** verifier suite
(one skip) pass. Current full-corpus figures are in `PROJECT_STATUS.md`.

The host gate `tools/game_text_lang_smoke.cpp` reads `assets/gb.lang`
and `assets/ru.lang` in place from the original APK using libarchive,
then feeds their bytes to the production `GameText::ReadLangFile` and
`String` implementations through an in-memory `AEFile::Read/Close`
adapter. The APK SHA-256 is
`515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`.
At Windows `-O0` and `-O2`, every one of **3,401** records in
each file matches an independent UTF-8-to-UTF-16 decode followed by
the original ARM homoglyph substitutions. It also checks the
`Error Language` fallback and that an incomplete first record closes
the handle and releases all allocated entries. The inputs remain
outside Git: `gb.lang` SHA-256
`336987a72cfec16437b75a598cc60ff4f062aa0de5a61d44cfa4e9ae6dc60088`,
`ru.lang` SHA-256
`bc1bc4228d288f84a7389ba94c7f1276f9e30e11eb666d399c28a9deb36c8045`.
The host command, repeated with `-O0` and `-O2`, is:

```powershell
g++ -std=gnu++17 -O0 -Isrc -ffunction-sections -fdata-sections tools/game_text_lang_smoke.cpp src/engine/core/GameText.cpp src/game/core/String.cpp '-Wl,--gc-sections' -larchive -o game_text_lang_smoke.exe
./game_text_lang_smoke.exe <path-to-original-apk>
```

The homoglyph mapping is not a host-decoder guess: original ARM
`String::getWCharFromUtf8` at `0x725d0..0x7270a` has a post-decode
replacement loop at `0x72664..0x72702`; for example it maps Cyrillic
`Р` (U+0420) to ASCII `P` at `0x726dc`, and Cyrillic `с` (U+0441)
to ASCII `c` at `0x726f0`. Without that normalization, both APK
files first differ at record 8.

The existing original-archive gate now also links production `GameText`
with production `AEFile` and the source-extracted Android archive-open
path. Using the same verified APK and OBB at Windows `-O0/-O2`, it
calls `setLanguage(0,3401)` and `setLanguage(5,3401)`, checks 3,401
non-fallback entries and the expected first string for each, then
selects absent `cz.lang` with ID 8 and confirms fallback to
`gb.lang`/language 0. The selected file instances return to their
pre-load count. The existing APK/OBB read and temporary filesystem
write/read gates still pass. Run the combined gate with
`tools/run_file_android_readexist_smoke.py --apk <apk> --obb <obb>
--out-dir <temporary-dir> --cxx <host-c++-compiler>`.

This is an end-to-end **host** language-selection gate, not a recovered
game running on Android. It uses libarchive in place of the original
libzip and does not execute JNI or the Java app lifecycle. Arabic
reshaping remains unverified and has placeholder form tables. The
original `setLanguage` at `0x7f2f0..0x7f310` calls `release` and
replaces the text-table pointer without freeing the old pointer array;
this gate checks selected file closure, not leak-free language reloads.
The next priority is the recovered Android runtime boundary and genuine
settings-to-language startup path; do not infer those from this gate.
