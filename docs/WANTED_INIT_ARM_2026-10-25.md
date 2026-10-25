# WantedWindow initialization recovery (2026-10-25)

Primary reference: Android 2.0.16 ARM `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`WantedWindow` constructor starts at `0xe0a44`, `init` at `0xe0b38`,
and `selectWanted` at `0xe1034` (Thumb code addresses, low bit cleared).

The original constructor gets text height from `Globals::Canvas` and
`Globals::font` (`0xe0ab2`) before calling `init` (`0xe0ae4`). The
recovered constructor dereferenced a null-initialized font pointer.
The original `init` reads `Globals::status`, `layout`, `iPad`, `iPadHD`,
`iPadLarge`, `w`, `h`, and `gameText` (`0xe0b70..0xe0e24`); the recovered
source used nine null-initialized pointer slots and a separate status
alias. The original `selectWanted` reads `Globals::imageFactory`,
`galaxy`, `gameText`, and `layout` (`0xe1084`, `0xe10e0..0xe1164`,
`0xe140c..0xe165c`); the recovered source used three more null slots
and a separate galaxy alias. This change connects those reads to the
observed globals.

At `0xe0b90..0xe0bde`, the original filters Wanted entries by system
race matching the entry's board. For race zero it includes entries
only after campaign mission 127; for other races it includes them from
mission 162. The prior source excluded every nonzero race and admitted
race-zero entries at the wrong boundary. The filter now follows the
ARM condition. No empty-list fallback was added: the original proceeds
to `selectWanted(0)` even when filtering produces no entries. Whether
callers guarantee a nonempty list remains unverified.

Focused ARM source-shape/strict fuzzy, before → after:

| Function | Before | After | Exact? |
| --- | ---: | ---: | --- |
| `init` | 47.1% / 26.9% | 91.1% / 39.4% | No |
| `selectWanted` | 65.2% / 26.4% | 89.5% / 49.6% | No |
| constructor C1 and C2 | 91.6% / 59.2% each | 89.9% / 59.8% each | No |

Across these four symbols, mean source-shape rises from **73.9%** to
**90.1%** and mean strict fuzzy from **42.9%** to **52.2%**. The slight
constructor source-shape decrease is retained because it removes a
real null dereference and uses the ARM-confirmed global values.

Native `gof2` and **209/209** ARM translation units build. The
source-extracted `init` test runs at `-O0/-O2`, covering the 127/128
and 161/162 boundaries, board mismatch, and all four screen layouts.
It substitutes UI/world services and stubs `selectWanted`, so it does
not prove the complete screen runs. Existing Wanted touch and hit-test
host checks also pass. **95** verifier tests pass (one skip).

The full ARM corpus compares **4,323/4,524** original functions over
**4,743** symbol/unit pairs, with **94.69%** mean source-shape and
**84.20%** mean strict fuzzy. There are **2,600 linked-exact** and
**1,082 byte-exact** pairs. Comparing exact symbol/unit sets against
the preceding Wanted touch-end report shows **zero losses** and one
new linked-exact pair (`ArraySetLength<TouchButton*>` in this unit);
byte-exact is unchanged. The focused and full reports are
`_work/wanted_init_before_20261025.json`,
`_work/wanted_init_after_20261025.json`, and
`_work/wanted_init_full_20261025.json` (not committed).

`selectWanted` still contains independently unverified and visibly
divergent display text construction, especially active location labels,
terminated placeholders, reward formatting, and the detail/body text.
`draw` still contains unresolved pointer slots. Those are the next
bounded UI recovery targets. The connected Android 16 handset accepts
only `arm64-v8a`, while the original APK contains only `armeabi-v7a`;
an attempted install returned `INSTALL_FAILED_NO_MATCHING_ABIS`.
Neither the APK nor its OBB was installed on that handset.
