# PlayerEgo jump-drive initialization and activation (2026-10-15)

The Android 2.0.16 ARM binary is the primary source. A targeted REA/IDA
read of `PlayerEgo::PlayerEgo` at `0x998cc` shows the constructor clearing
the drive state at `0x99a30–0x99a38`. At `0x99d56–0x99d78`, it requests
equipment sort 18, stores attribute 37 at `PlayerEgo+0x200`, and registers
sound resources 32 and 33. Without that equipment, it first clears the
maximum and then, if the ship has an integrated jump drive, takes attribute
37 from global item 85 (`0x99da4–0x99dcc`). The equipment branch takes
precedence over the integrated-drive branch.

REA/IDA also reads `PlayerEgo::startJumpDrive` at `0x9a6c4–0x9a700`:
the `+0x204` charging flag blocks re-entry; otherwise the method plays
sound 33 through `Globals::sound` with null vectors and zero gain,
emits HUD event 25, clears charge at `+0x1fc`, then sets the charging
flag. The previous C++ used an unconnected FMOD pointer instead of
`Globals::sound`, risking a bad dereference. REA is a targeted view of
the same Android binary, not independent corroboration of a separate
release. Its IDA adapter does not establish complete function body ranges
or all call edges, so the instruction addresses and existing ARM verifier
remain the stronger evidence for this package.

The extracted-production smoke covers the no-equipment, equipped, and
integrated-drive constructor branches and the start/re-entry event order.
The adjacent cloak-constructor smoke and native `gof2` build pass. The
previous package run compiled all 206 ARM translation units and the
37 verifier tests passed with one skip; the tests and both smokes were
rerun after the REA check.

Focused constructor source-shape/strict fuzzy rises **73.9%/41.6% →
77.8%/47.3%** for both C1/C2 aliases, with **450/604 → 486/604**
instructions. Focused `startJumpDrive` rises **91.9%/85.1% →
100.0%/96.2%**, with **21/26 → 26/26** instructions. None of these
three symbols is linked-exact or byte-exact.

The full ARM report compares **4,323/4,524** original functions,
with **201 missing**, **123 extras**, and zero skipped. Corpus averages
are **93.41%** source-shape and **82.31%** strict fuzzy; there are
**2,484 linked-exact** and **1,065 byte-exact** functions. Both exact
sets have zero gains and zero losses against the preceding boost report.
REA's whole-binary overview exceeded the upstream response limit on this
string-rich library; targeted function inspection worked. No live ARM32
flight/audio behavior has been verified. Next: the jump-charge update
and transition to the flight sequence, one bounded function group at a
time.
