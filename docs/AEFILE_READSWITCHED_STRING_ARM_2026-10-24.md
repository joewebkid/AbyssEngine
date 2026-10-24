# AEFile switched-string return ABI (2026-10-24)

Android 2.0.16 ARM `AEFile::ReadSwitched(String&, unsigned)` at `0x70d40`
returns a zero/one success flag in R0. The recovered C++ uses `uint32_t`,
consistent with the other switched-read overloads; ARM alone does not
establish the original source return type. The earlier declaration and
definition returned `void`; the C++ mangled name does not expose return
type, so this mismatch was not apparent from linking. ARM stores zero at
`0x70d7a` after the payload read, sets one at `0x70d8c` after `String::Set`,
and returns zero from the length-read failure branch at `0x70d96`.

The production declaration and body now return zero when either the
switched 16-bit length read or the payload read fails, and one after a
successful assignment. Failed reads leave the destination String unchanged.
The loader's source-extracted test stub was updated to match this ABI.

The source-extracted Windows `-O0/-O2` preview gate checks big-endian length
plus narrow payload, the returned success flag, and unchanged String on
each read failure. The station-loader smoke and native `gof2` build pass.
Focused ARM verifies **208/208** translation units and moves this function
from **93.9% source-shape / 85.0% strict fuzzy** (38 generated vs 42 target
instructions) to **100% / 100%** (42 vs 42), with a new linked-exact match.
The **93** verifier tests pass with one existing skip.

Fresh full ARM corpus: **4,323/4,524** original functions compared over
**4,737** pairs; mean source-shape / strict fuzzy **94.24% / 83.62%**,
**2,739** 100%-fuzzy, **2,572 linked-exact**, **1,082 byte-exact**,
**201** missing and zero skips. Set comparison against the preceding full
report finds exactly one new linked-exact pair, this function, and no lost
linked- or byte-exact pairs.

The host gate uses synthetic file bytes; a genuine original save fixture,
unusual low-level zero-byte read behavior and live Android execution are
not yet verified. Next priority: obtain a genuine save/preview fixture and
exercise the complete load lifecycle before claiming end-to-end behavior.
