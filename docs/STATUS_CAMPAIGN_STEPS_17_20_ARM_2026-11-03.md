# Campaign transitions, steps 17–20 (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

The original switch assigns all four missions to the campaign slot:
step 17 creates `Mission(156,0,56)` after setting the current ship's
race to zero; step 18 creates `Mission(156,0,55)`; step 19 creates
`Mission(189,0,55)` and then sets its status to 6; step 20 creates
`Mission(4,0,55)`. The original ARM at `0xab4d2–0xab4ee` confirms
the race change before the step-17 constructor. ARM at
`0xab6ae–0xab6c6` confirms step-19 type, station, campaign setter,
and status value.

The reconstruction already had the four mission tuples but added
steps 17, 18, and 20 outside the campaign slot. It also omitted the
step-17 race change and step-19 status. Those paths are corrected.
The source-extracted host smoke now exercises steps 0–20 at `-O0`
and `-O2`, including the new race and status effects:
`tools/run_status_campaign_opening_replacement_smoke.py`.

Focused ARMv7 verification compiles **209/209** translation units.
`nextCampaignMission` source-shape rises **10.3% -> 10.6%** and
strict fuzzy **5.1% -> 5.8%**; generated instructions increase from
210 to 219 against 2,884 original. The 146-symbol `Status` unit
retains **103 linked-exact** and **66 byte-exact** matches without
exact-set loss. Focused report:
`_work/status_campaign_steps_17_20_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The full ARM
corpus compares **4,323/4,524** original functions in **4,769**
pairs, with 201 missing and zero skipped. Mean source-shape and strict
fuzzy remain **96.75% / 86.74%** after rounding. Linked-exact stays
**2,779** and byte-exact **1,108**. Pair and exact sets have no losses
against `_work/status_campaign_steps_10_16_full_20261103.json`; only
the target function's scores changed. Full report:
`_work/status_campaign_steps_17_20_full_20261103.json`.

Live campaign progression remains unverified. Next reconcile steps
21–23, which also change non-mission state.
