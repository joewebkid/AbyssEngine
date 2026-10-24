# RecordHandler Android save-path literals (2026-10-24)

The recovered `RecordHandler` constructor previously initialized all
three file-name prefixes from empty strings. In the original Android
2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
the constructor at `0xcba2c` loads literal addresses `0x1cafec`,
`0x1caffe`, and `0x1cb00e`. The bytes at those addresses are respectively:

| `RecordHandler` field | ARM literal | Use |
| --- | --- | --- |
| `optionsPath` | `gof2_save_options` | Options file |
| `recordDir` | `gof2_save_game_` | Full-save slot prefix |
| `backupDir` | `gof2_save_game_preview_` | Preview slot prefix, despite the recovered field name |

The original game also ran on a connected Android 12 device with the
verified APK/OBB. Its native log identified
`/storage/emulated/0/Android/data/net.fishlabs.gof2hdallandroid2012/files/`
as both `dataDirectory` and `rootPath`. Startup created a **126-byte**
`gof2_save_options` there, independently confirming the first literal
and root-directory handoff. A copy of that file is retained only in
ignored local `_work/bins/`; its SHA-256 is
`8ed79ef59bc96c6382b0dc53d6f171d9fa277e980be93c7ccdecbd015a01b3a9`.
The game also copied FMOD data into this directory. No full-save or
preview path is yet confirmed by a live file.

The three C++ literals now reproduce the ARM strings. Native `gof2`
builds; the NDK r18b ARM build compiles **208/208** units. Focused
constructor source-shape/strict fuzzy remains **47.4%/28.3%** before
and after (two aliases), with no exact match. The literal correction
changes file behavior but not the verifier's normalized instruction
shape. The fresh full corpus compares **4,323/4,524** original
functions over **4,737** pairs, **94.31%/83.73%** mean source-shape /
strict fuzzy, **2,578 linked-exact** and **1,082 byte-exact**. Exact
symbol-pair comparison against the pre-edit corpus finds zero losses or
gains, with zero skipped units. The verifier suite passes **93** cases
with one existing skip.

The installed original application is not the recovered native game;
this check does not establish a recovered end-to-end save/load cycle.
Next: obtain one genuine Android full save and preview, then exercise
the recovered serializer and `AEFile` path against those fixtures.
