# CutScene render and time layout (2026-10-16)

Android ARM `CutScene::render3D` at `0x9985c` stores its elapsed frame
time at `CutScene+0x58`, updates and renders `Level`, then renders the
four optional geometries in `+0x28..+0x34` order. Its final array loop
reloads the array size and base for each indexed element. The recovered
range loop instead cached an end pointer. Restoring the indexed loop
and the class layout makes the whole 46-instruction `render3D` body
**100% source-shape / 100% strict fuzzy, linked-exact**.

The original `process` at `0x98e8c` writes two copies of the full
64-bit `GetCurrentTimeMillis()` result at `+0x40` and `+0x48`, keeps
a signed-delta 64-bit accumulator at `+0x50`, and writes frame delta
at `+0x58`. `initialize` at `0x989c0` also copies the complete 64-bit
time to the first two slots. The prior source split these words into
32-bit halves without the required alignment, masked time to 16 bits
and cleared the high halves. They are now three naturally aligned
`uint64_t` fields. ARM-only layout assertions lock the confirmed
offsets through `mode+0x88`. A host test extracts the production time
fragments and checks high-word retention, 16-/32-bit boundary
crossings and signed accumulator updates at `-O0/-O2`.

`isInitialized` now loads the original `+0x5c` byte and becomes
**100% linked- and byte-exact**. `process` rises from **86.3%/13.2%**
to **87.5%/42.1%** source-shape/strict fuzzy; `initialize` is
**86.9%/35.7%**. Both remain nonexact. Native `gof2`, **208/208** ARM
translation units, 40 verifier tests (one existing skip), and the
host time smoke pass. Full report
`A:/Galaxy on Fire new version/_work/cutscene_time64_full_20261016.json`
compares **4,323/4,524** originals, with **201 missing**, **123
extras**, **93.73%/82.84%** average source-shape/strict fuzzy,
**2,517 linked-exact** and **1,076 byte-exact**. Against
`cutscene_update_full_20261016.json`, exact `(unit,symbol)` sets gain
two linked and one byte pair, with zero losses.

The large `process` state machine and `initialize` ownership paths
still require source-backed branch testing; live ARM32 gameplay/GPU
timing was not exercised. Next priority: audit a bounded mode branch
inside `CutScene::process` against ARM before changing that body.
