# RepairBeam target, geometry, healing and audio routing (2026-10-26)

The Android 2.0.16 ARM implementation of `RepairBeam::RepairBeam` at
`0xa7100` and `RepairBeam::update` at `0xa72f8` was compared against
`analysis/gof2_libgof2hdaa_full_ida.c`, the independent Ghidra listing,
and Thumb disassembly of `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).

The constructor now takes its canvas from `Globals::Canvas`. Update
uses the actual Level, PlayerEgo, Status, Ship, Item and KIPlayer methods
for target selection. It restores the equipment range and healing
attributes, dead/dying and friend/shield gates, eligible targets within range,
charge accumulation, one-point damage when charge reaches 1, hull
healing, and ego shield regeneration. Both geometry transforms are
updated with `(dt, false)`. The original 64-entry ship-to-event table
at ELF `.rodata` address `0x1fbf80` is used for sound play/stop and 3D
position updates through `Globals::sound`.

The beam geometry path now reads `KIPlayer::shipGroupFlag` at `+0x7c`
and its `AEGeometry*` at `+0x08`. For ship group 44, the original uses
the normalized geometry direction times `-300.0`; group 49 also adds
normalized up times `200.0`. The same two offsets are applied to the
ego direction/up at the source. The constants are confirmed by the
literal pool at `0xa7690/0xa7694`, and the original scales the beam
as `(0.5, 0.5, length)` before aiming it from the ego offset. This
replaces a cast from `KIPlayer*` to `AEGeometry*`, the wrong flag byte,
and scalar stand-ins for vector-returning PlayerEgo methods.

Native `gof2` and AArch64 static archives build. ARMv7 focused verify
compiles 209/209 translation units and compares 16 symbols with zero
skips. `RepairBeam::update` source-shape rises from **75.1% to 92.9%**
and strict fuzzy from **29.6% to 38.9%**. The focused unit retains all
**9 linked-exact / 0 byte-exact** matches. `RepairBeam::update` itself
is not exact. The strict AArch64 link diagnostic goes from **454 to
434** unresolved whole-archive names and **214 to 194** JNI-reachable
names, with no `RB_` bridge helper left. AArch64 linking still fails.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs, **94.76% source-shape / 84.26% strict fuzzy**,
**2,606 linked-exact / 1,083 byte-exact**, zero skips. Relative to
the preceding full report, both exact symbol sets are unchanged with
zero losses. The full report is stored outside the repository at
`D:\gof2-arm64-crypto\repairbeam_geometry_full_20261026.json` to
avoid filling the project drive.

This is source-backed static recovery, not a runtime proof: beam
rendering, target healing, event routing and audio output have not been
played on a device. The remaining JNI-reachable unresolved set still
contains FMOD Event API calls and game/engine/UI helpers. The repo has
FMOD Ex headers but no Android arm64 implementation. A licensed or
compatible replacement audio backend, Android host packaging, remaining
link closure and arm64 layout/runtime audit are needed before an APK can
start. See `docs/ARM64_BUILD_PROBE_2026-10-25.md` for the ABI probe.
