# libzip 0.9.3 — exported header

Galaxy on Fire 2 reads its asset archives (the APK / `*.zip` patch files) through
**libzip 0.9.3** (Dieter Baron & Thomas Klausner, <https://libzip.org>), statically linked
into the Android `libgof2hdaa.so` (APK 2.0.16).

## How the version was identified

libzip leaves no version string in the binary, so the version was bracketed from its symbol
set and error-string table in Ghidra:

- The internal symbols (`zip_open`, `zip_fopen`, `zip_fread`, `zip_fclose`, `zip_stat`,
  `zip_source_*`, `_zip_dirent_torrent_normalize`, `TORRENTZIPPED-`, …) are unambiguously libzip.
- The mutation API still uses the **pre-0.11** names `zip_add` / `zip_add_dir` /
  `zip_get_num_files` (renamed to `zip_file_add` / `zip_dir_add` / `zip_get_num_entries` in 0.11),
  so it predates 0.11.
- The `_zip_err_str[]` table ends at `ZIP_ER_DELETED` (23). The encryption/read-only codes
  `ZIP_ER_ENCRNOTSUPP` (24) and `ZIP_ER_RDONLY` (25) — added in **0.10** — are absent. That pins
  it to the **0.9 series** (latest 0.9.3, released 2010), consistent with the 2.0.16 build era.

The vendored `zip.h` matches the binary exactly: `zip_add` returns `int` (not the 0.10+
`zip_int64_t`), `zip_fopen_index(struct zip *, int, int)`, and `struct zip_file` is the named
forward-declared type behind `FileInterfaceAndroid`'s `_ZN20FileInterfaceAndroidC1EP8zip_filebiii`
constructor.

## Source

`lib/zip.h` from the upstream `rel-0-9-3` tag:
<https://github.com/nih-at/libzip/blob/rel-0-9-3/lib/zip.h>

The 0.9.3 header is self-contained (only `<sys/types.h>`, `<stdio.h>`, `<time.h>`) — it pulls in
neither `zipconf.h` nor `<zlib.h>`. Only `zip.h` is committed; the compiled library is not.

## License

3-clause BSD (see the header comment in `zip.h`).
