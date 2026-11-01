# SpaceLounge camera update and random binding

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::update(int)` at `0x170738` and
`SpaceLounge::getSoundId(Agent *)` at `0x1714f8`.

The original update caps frame time at 50, advances the camera ease,
and switches to idle movement after the eased matrix equals its maximum
(`0x17074a`–`0x170818`). The previous source compared 64 bytes from
60-byte buffers and reversed the transition. The recovered path uses
the typed matrix comparison, a phase step clamped to **0.05–0.12**,
an amplitude capped at **3.5**, the camera's saved base matrix, and
the original Y rotation and vertical translation (`0x170818`–
`0x170a5a`). It preserves the original discarded random draw on the
first idle frame because it advances the generator state.

The touch-held camera controls now use `Globals::layout`, `Globals::w`
and `Globals::h` for their bounds. The middle-row horizontal rotation
uses **±0.01** per update; the previous source used **±7.5**. Vertical
and depth movements follow the ARM branches (`0x170a76`–`0x170b94`).

Both original functions call the real `AERandom::nextInt(int)` through
`Globals::rnd`; `getSoundId()` also reads the special mission texts
through `Globals::gameText` (`0x171516`–`0x171730`). The previous
source declared a namespace function without a definition and used
uninitialized local pointers. These are replaced by the existing
`AERandom` class and game globals. The native archive now contains a
definition of `AERandom::nextInt(int)` and references to that same
method, with no old namespace-function reference.

Focused ARM comparison: `update()` **70.8% → 92.1% source-shape** and
**9.5% → 55.7% strict fuzzy**; `getSoundId()` **83.2% → 84.9%** and
**50.3% → 48.4%**, respectively. The lower strict fuzzy value for
`getSoundId()` is recorded rather than hidden; the original global
bindings and call target are independently visible in ARM. Neither
function is exact. Windows and ARM64 static builds pass; ARMv7
compiles 209/209 units. The focused unit has no linked-exact or
byte-exact losses.

Live camera, touch-held movement, and sound selection are unverified
while device testing is deferred. The first idle update may read
`headBobSteps` before its first assignment; the original also
appears to do so, and this first-frame state needs runtime review.
`SpaceLounge::OnTouchEnd`, `onKeyPress`, and `startChat` remain major
incomplete paths.

The full ARM corpus covers **4,323/4,524** originals over **4,762**
comparison pairs at **96.09%** mean source-shape and **85.84%** strict
fuzzy. It has **2,733 linked-exact**, **1,091 byte-exact**, zero skipped,
and 201 missing originals. The normalized exact and comparison-pair
sets are unchanged from the preceding corpus. Reports:
`D:/gof2-arm64-crypto/space-update-random-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-update-random-20261102-full.json`.
