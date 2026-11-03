# Station sound resources and arrival time

Reference: original Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
The `ModStation::OnInitialize` state-100 route at `0xd5764–0xd5852`
was checked against the original NDK ARM objdump. The route advances
the state to 80.

The original calls `Globals::startNewSoundResourceList()`, then adds
16 sound IDs in this order: `0x5f, 0x7a, 0x6c, 0x60, 0x61, 0x62, 0x63,
0x65, 0x64, 0x66, 0x68, 0x69, 0x6a, 0x6b, 0x67, 0x7e`. It sets the
intro countdown to 50, arms the drag flag, and copies the current
station name. It compares the full 64-bit playing time with the
64-bit timestamp in `Status+0x70/0x74`. Once at least **30,001** time
units have elapsed, it creates a `Generator` and asks it to update
the current station's trade goods, except at station `0x6c`.

The former reconstruction passed a sound-pointer value as a
`Globals` object and used unbound wrappers for the station name and
time. It read only the low 32 bits of playing time and passed the
new `Generator` as its own station argument. The route now uses the
real `Globals`, `Status`, `Station`, and `Generator` APIs. A smoke
extracts this production branch and checks all 16 IDs and their
order, the name, countdown, full 64-bit timestamp, the 30,001-unit
boundary, and the excluded station at `-O0` and `-O2`.

Focused ARM verification compiles **209/209** translation units.
For the 2,235-instruction `OnInitialize`, source-shape improves
**71.5% → 72.2%**, strict fuzzy **14.7% → 27.3%**, and generated
instructions **1,264 → 1,283**. The `ModStation` unit remains
**11 linked-exact / 3 byte-exact**. Windows native and ARM64 static
builds pass.

The full ARM corpus compares **4,323/4,524** original functions in
**4,767** pairs, with **96.45% mean source-shape / 86.32% strict
fuzzy**, **2,759 linked-exact / 1,098 byte-exact**, 201 missing and
zero skipped. Pair membership and both exact sets are unchanged;
mean strict fuzzy rises from 86.31%.

Actual audio preload and station trade changes remain unverified in
the running game. The `ModStation::OnRelease` audio cleanup still
reads an unbound sound pointer and is the next bounded priority.
Redmi testing remains stopped by user request.
