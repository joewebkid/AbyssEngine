# SpaceLounge touch-end layout, help, and intro camera

Date: 2026-11-02. Reference: Android `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`,
`SpaceLounge::OnTouchEnd(int,int)` at `0x171a5e`–`0x172ecc`.

The original first sends a normal lounge touch release to
`Globals::layout`. A consumed back action closes the list or returns
to mode 0 and clears a generic agent's event; its list help path
copies text ID **643** from `Globals::gameText` into a temporary
`String` (`0x171b06`–`0x171b68`). The later general help path uses
text ID **627** (`0x172d44`–`0x172d88`). The previous source looked
up both texts and the layout through unbound zero-initialized slots.
The new code uses the observed global objects and the original text
IDs.

If the intro is still active, the original reads the current system's
race, builds an identity-based camera matrix from position table
`0x206794` and yaw table `0x1fb8d8`, sets the current canvas camera,
and collapses the camera ease to that matrix (`0x172d98`–`0x172ec8`).
The ARM call at `0x172ebe` resolves to
`EaseInOutMatrix::SetRange(Matrix, Matrix)`; the original copies the
matrix into both by-value argument slots before the call.
These table values were already recovered independently for
`SpaceLounge::init()` from the same ELF. The former touch-end branch
read a zero-filled local array, passed a camera handle as a pointer,
and did not use the observed yaw. The new branch uses the typed
matrix, `Globals::Canvas`, and the confirmed table values.

Focused ARM comparison for the entire large `OnTouchEnd()` function:
**17.2% → 16.1% source-shape** and **9.9% → 13.5% strict fuzzy**;
the rebuilt function has 339 instructions versus 2,630 in the
original. The source-shape decline is recorded because the repaired
early branches change alignment within a function whose mode-specific
actions are still largely absent. It is not linked-exact or
byte-exact. The focused unit retains 14 linked-exact and 2 byte-exact
matches across 42 comparisons. ARMv7 compiles 209/209 units; Windows
and ARM64 static builds pass.

Live touch and camera behavior remains unverified while device testing
is deferred. The offer, purchase, mission, and dialogue actions in
`OnTouchEnd()` remain incomplete; `onKeyPress` and `startChat` also need
reconstruction.

The full ARM corpus compares **4,323/4,524** original functions over
**4,762** pairs at **96.10%** mean source-shape and **85.86%** strict
fuzzy (both unchanged at reported precision). It retains **2,733
linked-exact** and **1,091 byte-exact** matches, zero skipped, and
201 missing originals. The normalized comparison-pair and exact-match
sets are unchanged from the previous full report. Reports:
`D:/gof2-arm64-crypto/space-touch-end-early-20261102-focused.json` and
`D:/gof2-arm64-crypto/space-touch-end-early-20261102-full.json`.
