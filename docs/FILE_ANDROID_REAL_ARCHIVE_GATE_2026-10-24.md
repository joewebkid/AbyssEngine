# Original Android APK/OBB read gate (2026-10-24)

The local `net.fishlabs.gof2hdallandroid2012-1.apk` contains an ARM
`libgof2hdaa.so` with SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
identical to this project's Android 2.0.16 comparison binary. The local
`main.47947006.net.fishlabs.gof2hdallandroid2012.obb` is a ZIP with
the ARM-observed `47947_GOF2CONTENT_ETC/assets/` prefix. These archives
remain outside Git; the optional test reads them in place.

Independent ZIP decoding confirmed the overlapping item file differs:

| Archive entry | Bytes | SHA-256 |
| --- | ---: | --- |
| APK `assets/data/bin/items.bin` | 27,628 | `639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c` |
| OBB `47947_GOF2CONTENT_ETC/assets/data/bin/items.bin` | 27,628 | `55b953629d994d553604418988d43d2b1935f93061da7040a464acad57677e95` |
| OBB `47947_GOF2CONTENT_ETC/assets/data/bin/stations.bin` | 3,408 | `761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3` |

The optional `--apk`/`--obb` gate in
`tools/run_file_android_readexist_smoke.py` first checks these entry
hashes and the ARM-library identity. A Windows host harness then uses
the production-extracted `loadAPKAndZip`, `FileExist`, `OpenRead`, ZIP
constructor and `Seek` against the original archives. The already
installed libarchive supplies a host adapter for ZIP entry access; the
test verifies that overlapping `items.bin` resolves to the **APK** bytes,
patch-only `stations.bin` resolves to the **OBB** bytes, and offset 4
returns the expected suffix of the APK item file. Its archive handles
are released after the run, including the unselected handle opened by
the recovered main-first lookup. No original game bytes are written to
the repository or test output directory.

The real-archive gate now also compiles production `AEFile`, `AENormalFile`
and `AEPakFile` with a host `FileInterface` adapter around the extracted
Android archive-open path. At both `-O0` and `-O2`, `AEFile::OpenRead`,
`Read` and `Close` consume the APK item file and patch-only OBB station
file, verify their SHA-256 values, reuse handle 1, release each selected
stream exactly once even after a second close, and reject a missing file
without changing its caller's handle. The adapter implements archive
`Read` using the same `zip_fread` contract; Android's JNI and actual
libzip implementation are not executed. The existing synthetic path
gate and direct original-archive offset gate still pass.

Native `gof2`, all **208/208** ARM translation units and **93** verifier
tests pass (one existing skip). There is no production source edit; the
fresh full ARM corpus is
**4,323/4,524** originals over **4,737** pairs, mean **94.31%
source-shape / 83.73% strict fuzzy**, **2,578 linked-exact** and
**1,082 byte-exact**, with zero exact-set losses against the preceding
report.

The ZIP adapter is **not** Android's statically linked libzip 0.9.x.
This gate proves archive identities, path selection and returned bytes
under an independent host decoder, plus the production `AEFile` handle
table for two successful reads. A later extension also checks a temporary
filesystem write/read-back through `AEFile`; see [the save I/O gate](AEFILE_ANDROID_SAVE_IO_HOST_2026-10-24.md).
It does not cover Android libzip, JNI setup, complete game-save
serialization, or live loading on a device. ADB saw no device at review
time.

## Live original-application gate (2026-10-24)

The connected Surya Android 12 device independently unpacked
`assets/data/bin/items.bin` from a temporary copy of the original APK:
SHA-256 `639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c`,
matching the host gate. That temporary APK copy was removed. The
original 2.0.16 APK (SHA-256
`515fc9a651057130c7b2b1c47bb497c61ebaebb0c567d413b5bd5453c13b221c`)
was then installed with the owner's approval. The original OBB was
copied to the game's OBB directory and verified on-device at
**1,011,128,870 bytes** and SHA-256
`42cfe13f0f3b9f99f38a1e77c74ac1cf41e542b3becbffea0f5bb7cfc4a6e4cb`.

The downloader advanced to `GOF2HD2012`; logcat reported the original
`libgof2hdaa.so` loaded and identified
`/storage/emulated/0/Android/obb/net.fishlabs.gof2hdallandroid2012/`
as `zipDirectory`. A device screenshot showed the original game's
Russian main menu. This confirms the installed original can initialize
and render through that environment; it does **not** execute the
recovered C++ in this repository or prove individual `OpenRead` calls.
The game's own startup copied FMOD files into its data directory and
created `gof2_save_options`; no game-record save was created in this gate.
