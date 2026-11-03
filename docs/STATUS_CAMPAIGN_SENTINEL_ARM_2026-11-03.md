# Campaign mission sentinel ownership (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.

Original `Status::setCampaignMission(Mission*)` at `0xa9a04`
sets the incoming mission's campaign flag, then deletes the previous
slot-zero mission only when it is nonnull and differs from
`Mission::empty`. Original `Globals::init` at `0xe3618` constructs
that singleton if it is absent. The recovered `Globals::init` already
does the same; `Status::removeMission` already stores the same sentinel.

The recovered campaign setter compared against a separate static
pointer that was permanently null. With a nonnull `Mission::empty`,
replacing the campaign slot could delete the shared singleton. It
now compares directly with `Mission::empty`, preserving original
ownership and slot update order.

Focused ARMv7 verification compiles 209/209 translation units.
`setCampaignMission` source-shape rises from **80.2% to 94.3%** and
strict fuzzy from **55.3% to 55.6%**; generated instructions increase
from 18 to 25 against 29 original. The 146-symbol `Status` unit keeps
103 linked-exact and 66 byte-exact matches with no exact-set loss.
Focused report: `_work/status_campaign_sentinel_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The existing
weapon-file-stack host scenario was attempted, but stopped at its
`Wanted` location assertion (line 509). Repeating it with the old
setter produced the same failure, so that broader gate is independently
red. The source-extracted focused host smoke now checks that a nonnull
shared sentinel survives replacement, that the next ordinary mission
is deleted exactly once, and that the incoming mission gets its
campaign flag. It passes at `-O0` and `-O2`:
`tools/run_status_campaign_sentinel_smoke.py`. Live campaign
replacement remains unverified. Next investigate the pre-existing
`Wanted` movement assertion and the real game lifecycle.

The full ARM corpus compares **4,323/4,524** original functions in
**4,769** pairs, with 201 missing and zero skipped. Mean source-shape
and strict fuzzy remain **96.75% / 86.74%** after rounding.
Linked-exact stays **2,779** and byte-exact **1,108**. Pair and exact
sets have no losses against
`_work/status_cargo_price_sources_full_20261103.json`. Full report:
`_work/status_campaign_sentinel_full_20261103.json`.
