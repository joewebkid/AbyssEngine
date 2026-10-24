# Status::inEmptyOrbit station mask — Android ARM recovery (2026-10-24)

The Android 2.0.16 ARM function at `0xabc18` checks station indices
`102..133` by subtracting `102`, shifting `1` by that bounded offset, and
testing against the literal **`0xC0000187`** (`0xabc62–0xabc6c`; the literal
resides at `0xabc98`). The prior C++ placeholder used a zero mask, so its
bit test was false for every index in range and skipped seven original
empty-orbit stations. The redundant `d & 0xff` in the shift count did not
change behavior because the branch already bounds `d` below 32.

The recovered test uses `(1u << offset) & 0xC0000187u`. The set bits select
stations **102, 103, 104, 109, 110, 132 and 133**; station 111 still follows
its separate campaign-mission rule. A source-extracted production `Status`
gate with original Level assets passes at Windows `-O0/-O2`, checking set and
unset mask bits, the station-111 threshold, and the existing stage-0 Level
boundary. Native `gof2`, **208/208** ARM units, and **93** verifier tests
(one skip) pass.

Focused ARM source-shape / strict fuzzy changed **61.8% / 21.7% → 73.2% /
23.8%**. This function is not linked- or byte-exact: the current compiler
inlines `Status::inAlienOrbit`, while the original calls it. The full corpus
still compares **4,323/4,524** originals over **4,736** pairs, with **201**
missing, zero skips, **94.23% / 83.60%** mean source-shape / strict fuzzy,
**2,735** 100%-fuzzy, **2,568 linked-exact** and **1,082 byte-exact**.
Before/after exact-symbol sets are identical. These aggregate means are
unchanged at two decimal places.

The gate proves the branch outcome for selected host fixtures, not a complete
station scene or live Android gameplay. Next priority remains a
`PendingProduct` production-to-reload round trip through `RecordHandler`.
