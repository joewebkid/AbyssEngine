# Campaign transitions, steps 21–23 (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
Original `Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

The original switch assigns all three new missions to the campaign
slot. Step 21 creates `Mission(11,0,55)`. Step 22 first sets byte 6
in the data of the `Array<bool>` at Status offset `0x38` to one,
then creates `Mission(11,20000,10)`. The recovered field at `0x38`
is `systemVisibilities`; ARM instructions at `0xab658–0xab67c`
confirm the byte store and mission arguments. Step 23 stores 9 and
48 at Status offsets `0x7c` and `0x80` before creating
`Mission(4,0,48)`; ARM at `0xaa046–0xaa068` confirms the paired store
and constructor arguments. The fields are kept under their existing
offset-based names because their full ownership and use are not
established by this branch alone.

The reconstruction had the mission tuples but added all three outside
the campaign slot and omitted both state changes. Those paths are
corrected. The source-extracted host smoke now covers steps 0–23 at
`-O0` and `-O2`, checking the campaign slot, mission arguments,
visibility byte, and paired fields:
`tools/run_status_campaign_opening_replacement_smoke.py`.

Focused ARMv7 verification compiles **209/209** translation units.
`nextCampaignMission` source-shape rises **10.6% -> 11.2%**, while
strict fuzzy falls **5.8% -> 4.8%**. Generated instructions increase
from 219 to 234 against 2,884 original. The observed behavior is
retained despite the local strict-fuzzy regression. The 146-symbol
`Status` unit keeps **103 linked-exact** and **66 byte-exact** matches
without exact-set loss. Focused report:
`_work/status_campaign_steps_21_23_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. The full ARMv7 corpus
compares **4,323/4,524** originals across **4,769** pairs, with mean
**96.75%** source-shape, **86.74%** strict fuzzy, **2,779 linked-exact**
and **1,108 byte-exact** pairs. There are **201 missing** symbols and
zero skipped units. Against the steps 17–20 baseline, the comparison-pair
set and both exact-match sets have zero gains and zero losses. Full report:
`_work/status_campaign_steps_21_23_full_20261103.json`. Live campaign
progression remains unverified. Next inspect steps 24–26, including
their cargo and mission side effects.
