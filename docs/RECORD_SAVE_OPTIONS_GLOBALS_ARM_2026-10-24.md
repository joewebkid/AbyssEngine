# RecordHandler::saveOptions global wiring (2026-10-24)

Primary source: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
function `_ZN13RecordHandler11saveOptionsEv` at `0xcd8c8`.
PC-relative loads and the ELF `R_ARM_GLOB_DAT` relocations identify all
five state objects formerly represented by null-initialized local
pointers:

| Use | GOT slot | Original symbol |
| --- | --- | --- |
| Settings bytes | `0x21016c` | `Globals::options` |
| Optional player name | `0x2103b0` | `Globals::instantActionPlayerName` |
| Record index | `0x2103b4` | `Globals::lastRecordWritten` |
| First-start flag | `0x2103b8` | `Globals::first_start_ever` |
| Lite-start flag | `0x2103bc` | `Globals::startLiteVersionWithMoreCredits` |

The writer now reads these existing `Globals` fields directly. This
removes dereferences of five never-assigned null pointers and recovers
the ARM data dependencies without changing the field serialization
order. The original game's empty player-name fallback is retained.
The 126-byte options file observed on-device contains 94 bytes of
fields (including an empty name) and a 32-byte hash, consistent with
this writer's layout; it is not committed to the repository.

The focused `saveOptions` result improves from **82.7% source-shape /
27.2% strict fuzzy** (223 recovered versus 226 original instructions)
to **98.9% / 93.5%** (221 versus 226). It is not linked-exact or
byte-exact. Native `gof2` and 208/208 ARM units build, and the
93-test verifier suite passes with one existing skip. The full corpus
compares **4,323/4,524** originals over **4,737** pairs at
**94.32%/83.74%** mean source-shape/strict fuzzy. **2,578 linked-exact**
and **1,082 byte-exact** pair sets are unchanged, with no losses and
zero skips.

This is source and static-binary validation, not a recovered end-to-end
save/load run. `loadOptions` still has unresolved state wiring and the
Android hash uses JNI-provided data. The next bounded priority is to
map the reader's state references and language calls against original
ARM before claiming options round-trip compatibility.
