# ObjectGun visible orientation axes (Android ARM)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`ObjectGun::render()` begins at `0x15fda8`.

In the visible-gun branch, the camera matrix lives at `sp+0xc0`.
At `0x15ff7e-0x15ff92`, ARM loads its entries at `sp+0xc0`,
`sp+0xc4`, `sp+0xd0`, `sp+0xd4`, `sp+0xe0`, and `sp+0xe4`;
these are matrix indices `0`, `1`, `4`, `5`, `8`, and `9`.
It writes them to the corresponding `ObjectGun::orientation`
entries at object offsets `0x74`, `0x78`, `0x84`, `0x88`,
`0x94`, and `0x98`. The previous C++ incorrectly copied
camera indices `10` and `11` into orientation indices `8`
and `9`. The remaining visible direction entries at
`0x15ff96-0x15ffb6` are the negated muzzle components.

The corrected source copies camera indices `8` and `9`.
`tools/run_objectgun_canvas_calls_smoke.py` extracts the
production assignment block and tests distinct camera values
for indices `8`/`9` versus `10`/`11` at host `-O0` and `-O2`.
The test also checks the other copied axes and negative muzzle
entries. Native `gof2` and all 207 ARM translation units build;
40 verifier tests pass (one existing skip).

Focused `render` source-shape/strict fuzzy remains
**89.6%/39.7%** before and after; it is not exact. The
full corpus compares **4,323/4,524** original functions,
with **201 missing**, **124 extras**, no skipped or wrong
signatures, and **93.67%/82.77%** average source-shape/strict
fuzzy. It has **2,513 linked-exact / 1,074 byte-exact**;
comparison against `objectgun_dtor_full_20261016.json` finds
zero gains or losses in either exact `(unit,symbol)` set.
Full report: `A:/Galaxy on Fire new version/_work/objectgun_visible_axes_full_20261016.json`.
Whole-frame camera-facing behavior and live ARM32 GPU output
remain unverified.
