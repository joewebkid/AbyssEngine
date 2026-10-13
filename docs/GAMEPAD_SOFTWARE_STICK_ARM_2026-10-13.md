# Gamepad-to-software-stick bridge — Android ARM audit (2026-10-13)

Scope: `setValuesForGamepad(float,float)` in `JniBridge` and the software
stick state consumed by `simulateTouch`. Primary evidence is Android 2.0.16
ARM ELF disassembly and initialized `.data`, with the Ghidra output as a
cross-check. No iOS/PC implementation was available for this path.

At ELF address `0x6b290`, `setValuesForGamepad` stores both float arguments
with `strd` into the software-stick block at `0x2150b4` and `0x2150b8`.
At `0x6b0a6`–`0x6b0ae`, `simulateTouch` checks the adjacent joystick flag,
loads that saved pair, and copies it into the eased pair. The initialized
Android block at `0x2150a0` is 28 bytes: flag `1`, followed by six
`0.5f` floats. The freshly built ARM object's `SoftwareStick` initializer
matches those 28 bytes; layout assertions check the saved-pair offsets.

The local setter previously wrote two private floats never read by
`simulateTouch`, while its joystick flag was zero-initialized. It now writes
the shared `SoftwareStick::savedX/savedY`, and that block starts with the
Android-backed enabled flag. Keyboard and pointer paths still use the same
input/eased fields.

Focused ARM verification builds all 206 translation units. Keeping the
shared stick hidden in the ARM object restores the original direct
addressing: the setter rises from 57.3% to 100% source-shape and from 33.3%
to 100% strict fuzzy, becoming linked-exact (not byte-exact). Its family's
linked-exact count rises 38 to 39; byte-exact stays 5. `simulateTouch`
source-shape rises 73.6% to 74.3%, while strict fuzzy moves 20.6% to 20.5%.
Its family retains 8 linked-exact and 2 byte-exact functions; no previously
exact row is lost. Native `gof2` builds successfully, and 37 verifier
tests pass (one skipped).

Full ARM corpus: 4,720 comparisons, 4,323/4,524 original functions covered,
201 missing. Average source-shape rises 92.79% to 92.80%, strict fuzzy
81.18% to 81.20%, linked-exact 2,394 to 2,395, and byte-exact stays 1,008.
Exact `(unit,symbol)` set comparison shows only the setter gain and no
previously exact loss.

This verifies the storage and consumption path, not an end-to-end hardware
gamepad or touch event. Android JNI entry, touch dispatch, and actual device
steering remain untested. A direct host executable could not be linked from
the existing static library because unrelated recovered functions still
have unresolved symbols. The large `simulateTouch` body remains far from
exact and needs a separate behavior review.
