# PlayerAsteroid quality letters — Android ARM evidence (2026-10-11)

`PlayerAsteroid::getQualityString` used empty placeholders for every quality,
so it returned an empty string even for supported qualities. The original
Android 2.0.16 function at `0xe2a00` selects `off_20DC98[quality - 5]` for
qualities 5–7, `"D"` for quality 4, and `"E"` otherwise. The three pointers at
ELF address `0x20DC98` are `0x1CA5F7`, `0x1CA5F5`, and `0x1CA5F3`;
the corresponding NUL-terminated strings are `"C"`, `"B"`, and `"A"`.

Only these five string values were restored. Native `gof2` and all 206 ARM
translation units build; the generated ARM object's string pool contains
`D\0E\0C\0B\0A\0`. The focused comparison for `getQualityString` is
35.2% source-shape and 18.4% strict fuzzy (from 32.9% and 9.9%), without
an exact match. That metric remains low because this source's hand-expanded
`String` construction does not mirror the original constructor call; the letter recovery is
supported by the original data, not by a source-shape gain. The 36 verifier
tests pass (one skipped). No game-level display test was run.

The full ARM corpus remains 4,323/4,524 compared, 92.05% mean
source-shape, 80.12% mean strict fuzzy, 2,325 linked-exact, and 997
byte-exact. There are no skipped units or wrong-type symbols, and a
pairwise comparison against the preceding station-table report found no
lost linked-exact or byte-exact pairs.

The unrelated `RecordHandler` hash functions also have empty salt arrays,
but their key pointers are null as well. They need a separate, end-to-end
semantic repair and are not changed here.

Update 2026-10-24: the manual string construction described above was
replaced with the ARM-observed constructor call; see
[the constructor recovery](PLAYER_ASTEROID_QUALITY_CONSTRUCTOR_ARM_2026-10-24.md).
