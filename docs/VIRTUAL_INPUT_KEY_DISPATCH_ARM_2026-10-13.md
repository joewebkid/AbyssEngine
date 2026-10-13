# Virtual key dispatch — Android ARM audit (2026-10-13)

Scope: `VirtualInput` key press/release, their delayed-release caller, and the
Android speed-up/slow-motion getters. Primary evidence is the Android 2.0.16
ARM ELF at `0x6a158` (`keyPressed`) and `0x6a458` (`keyReleased`), cross-checked
against `analysis/gof2_libgof2hdaa_full_ida.c` and
`analysis/gof2_gidra_engine_full_source.c` in the shared workspace. No iOS/PC
implementation was used for this package.

Confirmed recovered relationships:

- Android `keys` has 4,224 bytes for 48 entries of stride `0x58`; the
  predicate is at `+0x54`, touch-coordinate pointers at `+0x28/+0x2c`.
  ARM layout assertions now cover size and key offsets.
- `keyReleased` clears the four named overlay flags before the null-engine
  check, then updates the same X/Y steering inputs read by `simulateTouch`.
  It also clears the 32-bit `Globals::bankZ` value on release of keys 6/7.
- `keyPressed` uses the `+0x54` predicate, the stored follow-up action, and a
  separate combo-progress value. A touch match exits the three-pass search
  only when its pass is nonzero; the zero pass can continue matching. Touch
  positions are read from the two coordinate pointers and saved after
  `OnTouchBegin` as in the ARM flow.
- The key paths' speed-up/slow-motion bytes are now the same bytes returned
  by `SpeedUp`/`SlowMotion`; they were previously disconnected static state
  in different translation units. ELF symbol sizes and ARM stores establish
  `Globals::bankZ` as a four-byte value, not a pointer; `keyboard_swipe`
  remains a four-byte value.

| Function | Before source-shape | After source-shape | Before strict fuzzy | After strict fuzzy |
| --- | ---: | ---: | ---: | ---: |
| `keyPressed` | 1.1% | 95.8% | 0.0% | 76.3% |
| `keyReleased` | 0.1% | 97.7% | 0.0% | 69.0% |
| `SlowMotion` | 59.4% | 93.0% | 28.6% | 90.9% |
| `SpeedUp` | 59.4% | 93.0% | 28.6% | 90.9% |

Focused ARM verification built all 206 translation units without errors.
Native `gof2` and all 37 verifier tests pass (one test skipped). Full ARM
corpus: 4,720 comparison rows, 4,323/4,524 original functions covered,
201 missing. Average source-shape 92.70% → 92.79%, strict fuzzy 81.07% →
81.17%. Linked-exact 2,388 → 2,389 (`SendStoredKeyUpEvents` is the new exact
row), byte-exact 1,008 unchanged, and no loss among previously exact
`(unit,symbol)` pairs. The four functions above are not exact.

Remaining behavior is not verified end-to-end: the ELF's `keys` symbol is
4,224 initialized bytes in `.data` (first entries include `Left` and `Right`),
but local `g_virtualKeys` is still zero-initialized. The original table and
its relocated address fields must be reconstructed before native key input
can be called functional. `setValuesForGamepad` also still writes separate
local floats rather than the stick values read by `simulateTouch`; handle it
with the table/runtime package. No live device input or ARM32 runtime test
was performed.
