# Status::removeMission empty sentinel (Android ARM)

Primary source: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::removeMission(Mission*)` is at `0xac518–0xac5b4`. Its GOT
load at `0xac522–0xac52e` resolves to the relocation for
`Mission::empty` at `0x2102bc`. After deleting a matching mission,
the original reads that global and stores its value into the mission
array slot (`0xac58c–0xac596`).

The recovered source instead used a null-initialized local pointer
holder and dereferenced it whenever a mission matched. It now stores
`Mission::empty` directly. This preserves the sentinel even when its
value is null and removes a crash from every path that calls
`Status::removeMission`, including station mission completion.

Windows native and ARM64 static `gof2` builds pass. Focused ARMv7
verification compiles 209/209 units. `removeMission` source-shape
rises **82.2% → 85.6%**; strict fuzzy changes **30.9% → 29.7%**;
generated instructions **39 → 43** versus original **58**. The
generated ARM object has `R_ARM_GOT_PREL _ZN7Mission5emptyE`.

Full ARM corpus: **4,323/4,524** original functions over **4,767**
pairs, **96.47%** mean source-shape, **86.35%** strict fuzzy,
**2,761 linked-exact / 1,098 byte-exact**, 201 missing and zero
skipped. No pair or exact-set losses; corpus means are unchanged after
rounding. Runtime mission removal and ownership are still unverified.
The next priority is the campaign-step `0x2b` radio continuation in
`ModStation::OnTouchEnd`. Redmi testing stays stopped.
