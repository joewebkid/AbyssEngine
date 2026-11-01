# SpaceLounge initialization: ARM-backed bindings

Date: 2026-11-01. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::init()` at `0x169f80`.

## Restored in this package

- Read agents through `Status::getStation()->getAgents()` instead of an
  unresolved local shim (`0x169fb4`–`0x169fb8`).
- Use `Globals::layout`, `Globals::w`, and `Globals::gameText` for the panel
  geometry and the five original button labels, IDs 860, 861, 776, 804, 807
  (`0x16a114`–`0x16a396`). The first two colors and all five button
  placements follow the ARM path. Release old button objects before
  recreating the array (`0x16a1ac`–`0x16a1da`).
- Restore the four race-specific camera positions from the ELF table at
  `0x206794` and the yaw values at `0x1fb8d8`. Apply the resulting matrix to
  the current camera and camera easing through typed APIs (`0x16a3c0`–
  `0x16a4d4`).
- Remove the extra `updateScreenPositions()` call at the end: the original
  function does not make that call, and the current reconstruction of that
  method still has unbound pointers.

## Verification and limits

The focused ARM comparison for `init()` improved from **12.6%** to **60.1%**
source-shape and from **6.0%** to **24.6%** strict fuzzy. The rebuilt
function has 383 instructions versus 48 before and 749 in the Android
reference. It is still not an exact match. Windows native and ARM64 static
libraries built successfully; the focused ARM build compiled 209 units.

Remaining original branches include agent silhouettes and hit rectangles,
default agent image parts from an unresolved BSS descriptor, light and post
effect setup, iPad geometry, and the mission 24 auto-chat path. The related
`startChat()` and some drawing paths also remain incomplete. These are
separate reconstruction obligations; live screen behavior is unverified
while device testing is deferred.

The full ARM corpus covers **4,323/4,524** original functions over 4,754
comparison pairs, with **96.04%** mean source-shape, **85.81%** strict fuzzy,
**2,727 linked-exact**, **1,091 byte-exact**, and zero skipped. The two new
comparison pairs are the `TouchButton` array constructor and `ArraySetLength`
instantiations; the latter gained linked-exact. Normalized symbol/unit exact
sets show zero linked-exact and byte-exact losses against the prior corpus.
The 201 missing originals are unchanged. Reports:
`D:/gof2-arm64-crypto/space-init-bindings-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-init-bindings-20261101-full.json`.
