# Campaign transitions, steps 10–16 (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

The original switch assigns every step 10–16 mission to the campaign
slot. The mission tuples `(type, parameter, station)` are, in order:
`(11,0,76)`, `(11,0,79)`, `(150,0,0)`, `(4,0,79)`, `(11,0,98)`,
`(4,0,98)`, and `(11,0,98)`. At step 12 it sets the new mission's
status to `missionCount + 1` and then hides it. The ARM sequence at
`0xaa346–0xaa36a` confirms type 150, station zero, the campaign
setter, and the status derived from Status offset `0x1c4`; the tail
at `0xab522–0xab534` passes false to `Mission::setVisible`.

The reconstructed tuple values were already present. Six of these
seven steps incorrectly placed their missions outside the campaign
slot, and step 12 omitted its status and visibility updates. The
campaign flags and step-12 follow-up calls now match the original.
The existing source-extracted host smoke has been extended through
step 16 and runs at `-O0` and `-O2`, checking the tuple, campaign
assignment, status, and visibility:
`tools/run_status_campaign_opening_replacement_smoke.py`.

Focused ARMv7 verification compiles **209/209** translation units.
`nextCampaignMission` source-shape rises **9.8% -> 10.3%** and strict
fuzzy **4.7% -> 5.1%**; generated instructions increase from 199 to
210 against 2,884 original. The 146-symbol `Status` unit keeps
**103 linked-exact** and **66 byte-exact** matches with no exact-set
loss. Focused report:
`_work/status_campaign_steps_10_16_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The full ARM
corpus compares **4,323/4,524** original functions in **4,769**
pairs, with 201 missing and zero skipped. Mean source-shape and strict
fuzzy remain **96.75% / 86.74%** after rounding. Linked-exact remains
**2,779** and byte-exact **1,108**. Pair and exact sets have no losses
against `_work/status_campaign_steps_0_9_full_20261103.json`; only
the target function's scores changed. Full report:
`_work/status_campaign_steps_10_16_full_20261103.json`.

Live campaign play and the later branches remain unverified; next
reconcile steps 17–20 and their ship/mission side effects.
