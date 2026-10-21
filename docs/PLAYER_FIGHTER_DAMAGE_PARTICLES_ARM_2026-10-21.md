# Fighter bounding volumes and damage particles (Android ARM, 2026-10-21)

Primary reference: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
The `PlayerFighter::update(int)` block at `0xddc56–0xddd94` was checked
against ARM disassembly and REA/IDA pseudocode. It follows the camera
override and is reached on all paths that continue through the frame.

When `boundingVolumes` is present, the original iterates every element
and invokes its virtual `update(x, y, z)` with the Fighter's current
`renderPosition`. It does not test individual elements for null. The
code then reads `Player::getHitpoints()`. A set byte at Fighter `+0x1fc`
marks the damage-particle state. At or above 33% of maximum hitpoints,
the byte is cleared and both emission systems are disabled. Below that
threshold, an unset byte enters the low-health path. With particle
quality at `Globals::options + 0x28` greater than zero, both systems
are enabled only when `KIPlayer::isDocked()` is false; while docked,
both are explicitly disabled. The byte is set even when the quality
setting is zero or negative. Existing state is left alone on repeated
frames without a threshold crossing. `Level` manager slots `+0x78`
and `+0x84`, Fighter particle handles `+0x80` and `+0x84`, and the
state byte `+0x1fc` match the ARM loads and stores. The quality float
is read with `memcpy` from the packed options bytes, avoiding aliasing
and alignment assumptions on the host.

The production-method Fighter smoke checks a two-element bounding-volume
array, strict low-health transition, repeat-frame idempotence, the 33%
boundary, zero-quality state transition, and docked emission handling.
All 11 Fighter smokes, native UCRT64 `gof2`, all 208 NDK r18b ARM units,
and 93 verifier tests pass (one existing skip). Focused whole-`update`
source-shape/strict fuzzy changes **42.2%/12.3% → 44.3%/12.2%**; the
small fuzzy decrease is reported explicitly. The 44-function Fighter
unit retains **21 linked-exact** and **10 byte-exact** pairs.

The full NDK r18b corpus compares **4,323/4,524** original functions
across **4,735** pairs at **94.04%/83.38%** mean source-shape/strict
fuzzy. It retains **2,557 linked-exact** and **1,082 byte-exact**
matches, with zero skipped, wrong-type, or exact-pair losses and 124
extra symbols. Comparing `(unit, symbol)` pairs with the preceding
follow-camera report finds only the expected `PlayerFighter::update`
score change. Full report: `fighter-update-damage-particles-full.json`
(SHA-256
`e05f4270ae82adcc3fdaa29d70f9528cefeb6ccccbd91512c7466830ee8b08ec`).

The original frame continues after `0xddd94`; movement, docking and
the common tail remain unresolved behind `PF_update_body`. These smoke
tests do not validate a complete game frame.
