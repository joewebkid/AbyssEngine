# CutScene mode-2 ship placement (2026-10-17)

The Android ARM `CutScene::initialize()` mode-2 branch at
`0x987b6..0x9886e` checks `Status::getPlayingTime() > 0` before
calling `Level::getEnemies()`. It requires a nonnull list with at
least two entries and nonnull penultimate and last players. The
original then passes the same stack vector to each player's virtual
`setPosition(const Vector&)` slot at `vtable+0x44`.

For the existing random integers `rx = nextInt(20000)` and
`ry = nextInt(60000)`, the vectors are:

| Player | X | Y | Z |
| --- | ---: | ---: | ---: |
| Penultimate | `rx - 24000` | `-30` | `ry + 39500` |
| Last | `rx - 23400` | `50` | `ry + 38600` |

The Y literals have float32 bits `0xc1f00000` and `0x42480000`
in the ARM pool at `0x98a38..0x98a3c`. The X/Z integer arithmetic,
signed float conversion, call registers and array indices were
checked directly in disassembly. The previous C++ used Y=0 for
both, called the float-argument overload, and accessed enemies even
when playing time was zero. The corrected branch follows the
observed call order and vector values without bypassing the common
initialization after mode dispatch.

The extracted host `-O0/-O2` test covers both positions, zero and
negative playing time (no enemy-list call), null/short lists and
missing players. Focused ARM `initialize` source-shape/strict fuzzy
rises from **93.1%/37.9%** to **95.8%/61.9%**, still nonexact.
Native `gof2` and **208/208** ARM units pass. The host test does
not establish live ARM32 gameplay or camera/GPU output. Full-corpus
results are in [PROJECT_STATUS.md](PROJECT_STATUS.md) and
[VALIDATION.md](VALIDATION.md).
