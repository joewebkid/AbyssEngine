# Opening campaign transitions, steps 0–9 (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

The original switch creates a campaign mission at every step from 0
through 9. Step 0 creates `Mission(11,0,78)` and copies slot zero into
the `mission` field at offset `0x194`. Steps 1 and 3 set the new
mission's status to 10 and 25, respectively. Step 7 clears
`unsaleable` on equipment and cargo, then takes each replacement price
from `Globals::items` at that list position. Step 9 replaces the
ship's sort-19 equipment with one copy of catalog item 86 and creates
`Mission(11,0,79)`. The original ARM instructions at `0xab3c2`,
`0xab3ea`, and `0xab406` independently show sort 19, catalog byte
offset 344 (`86 * 4`), and station 79. The IDA switch and this ARM
sequence agree on the step-9 operation order.

The reconstruction skipped mission creation at step 0, treated steps
2, 4–9 as non-campaign missions, omitted both status values, sourced
step-7 prices from the owned items, and called `makeItem` through a
null pointer at step 9. Those paths now follow the observed original
behavior. The source-extracted smoke exercises each step 0–9 at
`-O0` and `-O2`, including mission type, station, campaign slot,
status value, step-7 prices, and step-9 equipment replacement:
`tools/run_status_campaign_opening_replacement_smoke.py`.

Focused ARMv7 verification compiles **209/209** translation units.
`nextCampaignMission` source-shape rises **7.5% -> 9.8%** and strict
fuzzy **4.4% -> 4.7%**; generated instructions increase from 150 to
199 against 2,884 original. The 146-symbol `Status` unit retains
**103 linked-exact** and **66 byte-exact** matches, with no exact-set
loss. Focused report:
`_work/status_campaign_steps_0_9_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The full ARM
corpus compares **4,323/4,524** original functions in **4,769**
pairs, with 201 missing and zero skipped. Mean source-shape and strict
fuzzy remain **96.75% / 86.74%** after rounding. Linked-exact remains
**2,779** and byte-exact **1,108**. Pair and exact sets have no losses
against `_work/status_gamma_float_abi_full_20261103.json`; only the
target function's scores changed. Full report:
`_work/status_campaign_steps_0_9_full_20261103.json`.

An actual campaign playthrough remains unverified. The rest of this
large function is substantially incomplete; the next bounded priority
is to reconcile the following campaign steps and test their side
effects.
