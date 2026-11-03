# Engine constructor defaults from Android ARM

Reference: Android ARMv7 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`Engine::Engine()` at `0x83eec–0x840be` (`C1` and `C2` share the body).
REA/IDA assembly and pseudocode were checked against the recovered fields;
the addresses and stored bit patterns below are binary evidence.

- At `0x83fa2–0x83fae`, the temporary vector assigned to `this+0x3cc`
  is `(0.5f, 0.0f, 0.3f)`. The reconstruction used zero for its last
  component, which is later passed into post-effect routing.
- At `0x84062`, the constructor stores `0x3f333333` (`0.7f`) at
  `this+0xcc`. Cube-mapping shaders read this field as a uniform; it was
  previously left uninitialized.
- At `0x84080`, the constructor stores zero at `this+0x3e8` and
  `0x47435000` (`50000.0f`) at `this+0x3ec`. The recovered code had set
  both fog distances to zero; shader paths consume `fogMaxDist`.
- At `0x83ffe–0x84006`, byte `this+0xff` is cleared, the statistics
  state at `this+0x4a4` is cleared, and the halfword `0x0100` is stored
  at `this+0xfd`. In little-endian layout this means bucket byte
  `+0xfd = 0`, enabled byte `+0xfe = 1`, and byte `+0xff = 0`.
  The former `field_0xfd = 0x0100` wrote that value starting at
  `+0xfe`, leaving the enabled byte zero and the next byte one.
- The original also clears complete light, effect, viewport, eye-position,
  and accelerometer/gravimeter field groups. The missing zero writes
  for `+0x344`, `+0x350`, `+0x3d0`, `+0x404`, `+0x47c`, both viewport
  heights, and several single-byte fields are now restored. ARM offset
  assertions guard the statistics and fog layout.

Focused ARM validation compiled **209/209** translation units. Both
constructor aliases improved from **88.9% to 90.1% source-shape** and
**38.7% to 55.7% strict fuzzy**. Windows native and ARM64 static builds
pass. The full corpus compared **4,323/4,524** original functions in
**4,763** pairs: **96.25% mean source-shape**, **86.04% strict fuzzy**,
**2,746 linked-exact**, **1,098 byte-exact**, 201 missing and zero
skipped. Compared to the previous full report, the comparison-pair,
linked-exact and byte-exact sets are unchanged.

The constructor remains substantially nonmatching, and these static
checks do not verify live shader output or every uninitialized field.
Next inspect the remaining constructor ordering and test rendering on
a permitted host. Redmi testing remains stopped by user request.
