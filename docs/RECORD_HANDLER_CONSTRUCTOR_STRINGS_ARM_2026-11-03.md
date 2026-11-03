# RecordHandler constructor string assignment (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `RecordHandler` constructor at `0xcba2c` default
constructs its three `String` fields at offsets `+0x08`, `+0x14`
and `+0x20`. It then separately constructs, assigns and destroys a
temporary for each confirmed literal: `gof2_save_options`,
`gof2_save_game_` and `gof2_save_game_preview_`. The reconstruction
directly constructed each field from its literal. It now follows
the original assignment and temporary lifetime order. The literal
bytes and original Android path use were previously checked in
`RECORD_ANDROID_SAVE_PATHS_ARM_LIVE_2026-10-24.md`.

The original constructor does not write the first two pointer fields,
`currentMission` and `currentAgent`. The native reconstruction keeps
their zero initialization. Both are read when writing nested agent
and mission data; removing the zero before proving their producer
would introduce an uninitialized read. This is the remaining
two-instruction source difference, not an exact match claim.

Focused ARMv7 verification builds 209/209 translation units and
compares 63 `RecordHandler` symbols. Both constructor variants improve
from 47.4% to 99.0% source-shape, from 28.3% to 98.8% strict fuzzy,
and from 38 to 84 generated instructions against 82 original.
The other 61 functions retain their metrics; the unit keeps 21
linked-exact and one byte-exact comparison. Focused reports:
`_work/record_sd_string_lifetime_focused_20261103.json` and
`_work/record_ctor_assign_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The recovered
save/load path has not been exercised with a genuine fixture or on
a permitted device. Next trace when the two pointer fields are first
read and assigned in the original save writer.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Mean source-shape rises from
96.71% to 96.73%; mean strict fuzzy rises from 86.69% to 86.72%.
Linked-exact remains 2,779 and byte-exact 1,108. No comparison pair
or exact match was lost. Full report:
`_work/record_ctor_assign_full_20261103.json`.
