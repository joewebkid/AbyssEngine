# SpaceLounge constructor: scene placement and camera easing

Date: 2026-11-01. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::SpaceLounge()` at `0x169ad0`.

After `init()`, the original constructor removes an accepted offer of
type 12 from the station agent list and calls `init()` again
(`0x169b4e`–`0x169f06`). The previous source called an unresolved
`ArrayRemove_AgentPtr` shim. It now calls the typed `ArrayRemove`
operation used by the array implementation.

Once `CutScene` is initialized, the original gets its level's enemy
array. For every agent, it aims the first scene object at its original
position, applies the same orientation to a paired silhouette object,
and shifts that paired object **100** units opposite the facing
direction in X/Z (`0x169bce`–`0x169cbc`). The prior source skipped
the whole loop. The reconstructed loop uses the confirmed `geometry`
field at object offset `+0x08` and the virtual position setter.

The original then creates a 3,000 ms camera easing range between two
race-specific matrices and installs the start matrix on the current
canvas camera (`0x169cde`–`0x169ec6`). Positions come directly from
ELF tables `0x206764` and `0x206794`; yaw values come from `0x1fb8c8`
and `0x1fb8d8`. The earlier source built both matrices from the race
number and dereferenced an unbound camera pointer. All four rows of
both tables were read from file-backed PT_LOAD data and copied as
typed integer/float constants.

Focused ARM comparison for both constructor variants improved from
**70.0% to 92.8% source-shape** and **29.5% to 50.6% strict fuzzy**.
The compiled body grew from 269 to 413 instructions against 437 in the
reference. Neither constructor is exact. Windows and ARM64 static
builds pass; ARMv7 compiles 209/209 units. The focused unit has no
linked-exact or byte-exact losses.

Live scene and camera behavior remains unverified while device testing
is deferred. The `update`, `OnTouchEnd`, `onKeyPress`, and `startChat`
paths remain incomplete; the current `init()` also omits iPad layout
and some scene-light and campaign branches.

The full ARM corpus covers **4,323/4,524** originals over **4,762**
comparison pairs at **96.09%** mean source-shape and **85.83%** strict
fuzzy. It has **2,733 linked-exact**, **1,091 byte-exact**, zero skipped,
and 201 missing originals. The newly instantiated `ArrayRemove<Agent *>`
adds one comparison pair; no prior pair or normalized linked-exact or
byte-exact symbol was lost. Reports:
`D:/gof2-arm64-crypto/space-ctor-20261101-focused.json` and
`D:/gof2-arm64-crypto/space-ctor-20261101-full.json`.
