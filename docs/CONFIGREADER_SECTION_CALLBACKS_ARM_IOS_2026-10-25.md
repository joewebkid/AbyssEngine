# ConfigReader section callbacks: ARM and iOS evidence (2026-10-25)

## Scope and evidence

The previous constructor registered three invented section names with empty
callbacks. In the original Android 2.0.16 ARM binary (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`ConfigReader::ConfigReader` at `0x87258` registers `TIMER`, `KEYMAP`, and
`VIBRATION` at `0x8728e`, `0x872ae`, and `0x872ce`, respectively, passing the
engine as callback context. The callback entries at `0x873a0`, `0x87408`, and
`0x875e8` read one timer line, up to 64 key mappings, and one vibration line.

The original iOS `config.cfg` (SHA-256
`b4eda413840d3531d4e157c7918df6da8c3533313f5651b3055c5f42cc930bd0`)
contains a `TIMER` section with the line `20` and exactly 64 `KEYMAP` lines, ending
in `-1,NA`. It does not contain a `VIBRATION` section, so that branch is
backed by ARM evidence but not by this iOS sample.

## Recovery

The constructor now registers the three confirmed names. `TIMER` sets the
engine timer field for a non-`EOF` line. `KEYMAP` splits each line on a comma,
builds up to 64 temporary `KeyCode` entries, applies the mapping only when all
64 entries are present, then frees the temporary entries. `VIBRATION` sets the
engine flag for `ON`. Malformed or incomplete key maps are not applied.

## Verification and limits

- Focused ARM verification: constructor C1/C2 each improved from **50.1%**
  source-shape and **27.0%** strict fuzzy to **99.5%** and **98.8%**. These
  constructor symbols are not exact matches. `ParseFile` remains **95.0%**
  source-shape and **82.9%** strict fuzzy, unchanged.
- ARM compilation: **209/209** translation units. Native `gof2` builds.
- A host smoke test extracts the production callback and constructor source,
  compiles it at `-O0` and `-O2`, and reads the original iOS configuration. It
  checks all three registrations, the 64-key map, and incomplete/EOF/OFF
  cases. Verifier suite: **95 tests passed, one skipped**.
- Full ARM corpus: **4,323/4,524** original functions covered over **4,741**
  comparisons, mean source-shape **94.63%** and strict fuzzy **84.14%**.
  There are **2,597 linked-exact** and **1,082 byte-exact** matches, with
  **zero losses** from the preceding corpus. Three newly linked-exact helper
  instantiations account for the linked-exact increase, not the constructor.
- The host smoke test substitutes a small `String`, `Array`, file-line, and
  application-manager seam. It does not prove production `ParseFile`, Android
  archive decoding, or live engine initialization. These remain next runtime
  priorities; do not infer full behavior or ABI fidelity from the focused
  source-shape percentage.

The local reports are `_work/config_reader_callbacks_before_20261025.json`,
`_work/config_reader_callbacks_after_20261025.json`, and
`_work/config_reader_callbacks_full_20261025.json` (ignored by Git).
