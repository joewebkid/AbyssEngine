# SpaceLounge agent screen positions

Date: 2026-11-01. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::updateScreenPositions()` at `0x170478`.

The original function reads enemies from `this->cutScene->level`, obtains
the current camera through `Globals::Canvas`, and computes a half-width
of **150.0** world units along the camera-right vector (`0x1704a4`–
`0x1704ee`). For each agent, it projects the two world positions into
`agentRects[2*i]` and `agentRects[2*i+1]`; the second position is raised
by **450.0** units (`0x170532`–`0x1705a6`). The previous source used
unbound local pointers, a width of 0.5, and invalid projection arguments.

The ARM path then aims each paired scene object toward the agent, places
the silhouette **100.0** units behind the resulting direction in X/Z,
and applies a **3.1416** radian Y rotation to the main object for race 0
(`0x1705aa`–`0x1706d2`). Both scene-object matrix calls address the
`geometry` field at offset `+0x08`; the previous source used the
`parentGeometry` field. The reconstructed method uses the typed matrix,
vector, camera, and projection APIs observed at those calls.

Focused ARM `source-shape` improved from **32.1% to 86.5%** and strict
fuzzy from **13.5% to 19.7%**. The compiled body grew from 52 to 198
instructions against 258 in the original. It is not exact. Windows and
ARM64 static builds pass; ARMv7 compiles 209/209 units. The focused
`SpaceLounge` comparison has no linked-exact or byte-exact losses.

The area vectors are now populated, but live touch and rendering behavior
is unverified while device testing is deferred. Other `SpaceLounge`
methods, including `drawLounge()`, `startChat()`, and the constructor,
still have unbound pointers or incomplete branches.

The full ARM corpus covers **4,323/4,524** originals over **4,761**
comparison pairs at **96.06%** mean source-shape and **85.82%** strict
fuzzy. It has **2,733 linked-exact**, **1,091 byte-exact**, zero skipped,
and 201 missing originals. The normalized complete exact sets and the
comparison-pair set are unchanged from the preceding corpus. Reports:
`D:/gof2-arm64-crypto/space-screen-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-screen-20261101-full.json`.
