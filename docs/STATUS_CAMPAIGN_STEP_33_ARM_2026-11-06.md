# Campaign transition, step 33 (Android ARM32)

Date: 2026-11-06

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
`Status::nextCampaignMission(bool)` starts at `0xa9a8c`.

## Original-binary evidence

The Thumb branch table sends step 33 to `0xa9fb2`. The original calls
`Ship::removeCargo(164, 50)`, then scans the `bluePrints` array at
Status offset `0x18` for the first blueprint whose index is `85`.
When found, it calls `BluePrint::unlock()`, creates an item from
catalog index `164` with `Item::makeItem()`, and passes that item to
`BluePrint::addItem(item, 50, 10)` (`0xa9fb2`–`0xa9fe4`,
`0xab7ac`–`0xab7de`). Whether a blueprint is found or not, it then
constructs `Mission(11, 0, 30)` and joins the campaign setter at
`0xa9c30`. The previous source omitted these cargo and blueprint
effects, used station `29`, and classified the mission as ordinary.

## Recovery and verification

The source now follows that path. The production-source smoke runs
steps 0–33 at `-O0` and `-O2` and checks cargo arguments, first
matching blueprint, unlock, catalog item, production amount and
station, and the campaign mission tuple.

- Focused ARM32 build: **211/211** translation units.
  `nextCampaignMission` source-shape **12.6% -> 14.0%** and strict
  fuzzy **5.5% -> 5.8%**; generated instructions **268 -> 300**
  against **2,884** original. The `Status` unit retains 103
  linked-exact and 66 byte-exact functions.
- Windows native `gof2`, ARM64 static `gof2`, and strict ARM64 shared
  link: pass. Raw ARM32 unresolved names remain **174**.
- Full ARM corpus: **4,323/4,524** original functions in **4,772**
  pairs, mean source-shape **96.87%**, strict fuzzy **86.98%**,
  **2,802 linked-exact**, **1,116 byte-exact**, 201 missing, zero
  skipped, zero wrong-type, and 151 extra symbols. Both exact
  membership sets are unchanged.

Live campaign progression and full ARM32 shared linking remain
unverified. Next inspect steps 34–35 separately.
