# ARM64 shared-library link gaps (2026-11-04)

A strict `--whole-archive --no-undefined` ARM64 link of the current `libgof2.a`
against the project libzip/crypto archives and Android system libraries reports
**35 unresolved symbols**, all referenced from `ModStation.cpp`. These are
reconstruction helper names, not 108 proven absent symbols in the original ARM
library. Many can map to existing C++ methods; each mapping needs source and ABI
confirmation before replacement. The raw linker evidence is
`D:/gof2-arm64-crypto/startup-link-touchend-campaign-20261104.log` (outside Git).

The earlier strict link had 159 unresolved symbols. REA confirmed that
`PaintCanvas::StartDraw2FBO` calls the existing
`Engine::ActivateRender2TextureFBO`; replacing an unresolved wrapper removed
one. REA then confirmed the destructor sequence in `ModStation::OnRelease`;
direct typed deletion removed 11 more without adding new unresolved symbols.
REA also confirmed the opening `CutScene(23)` construction and
`initialize()` call in `ModStation::OnInitialize`. Direct calls remove
two more unresolved helpers. The unused read of the null
`g_oi_status` placeholder at function entry is also removed; the
original begins directly with the cutscene branch. The original
`0xA0`-byte allocation is retained on ARM32 with placement
construction, and the allocation grows to `sizeof(CutScene)` if the
recovered class is larger on ARM64.
REA evidence:
`ev_fd7c0959b6f25f9dff6c57f51ecb6d1759641081817eebbf90c22041e9b3cc5b256`.
The resulting counts are
159 → 158 → 147 → 145 → 143 → 137 → 135 → 131 → 125 → 122 → 117 → 114 → 109 → 108 → 103 → 96 → 92 → 90 → 88 → 83 → 70 → 59 → 41 → 39 → **35**.

REA Android ARM `ModStation::OnTouchEnd` evidence
`ev_209fbd44797382bbddb290d593c3c6b7a8d876525f1b6a283ba2837fadb1f1ef`
confirms the touch-release inertia handoff, campaign advance, replacement
of the cutscene ship with its index and race, and the shared campaign
reward path. The old code changed camera coordinates on touch release,
discarded the race and reward, and called placeholder helpers. The
restored path removes four unresolved helper names: **39 → 35**, with
no new names. Focused `OnTouchEnd` source-shape rises **83.2% → 84.6%**;
strict fuzzy changes **23.0% → 21.8%**. This is not an exact match,
and live gameplay remains unverified until a newly linked APK runs.
Full ARM report `_work/modstation_touchend_campaign_full_20261104.json`
compares 4,323/4,524 originals in 4,770 pairs, with mean source-shape
**96.77%**, strict fuzzy **86.76%**, 2,780 linked-exact and 1,108
byte-exact matches, 201 missing and zero skipped. Pair and exact-match
sets have no losses. Windows native, ARM64 static and ARMv7 209/209
builds pass.

The late `ModStation::OnUpdate` path in REA evidence
`ev_71b291d89b3d7ddb28763bd8cbfcf89586bad2363a58ae69b981ac5589b66fe2`
uses the float interpretations of fields `0xf4`/`0xf8` for camera
drag decay, updates `CutScene::cameraRotX`, and moves or rotates
`rootGeom` by touch zone while updating the corresponding `CutScene`
position and angle fields. The former light interpolation and constant
geometry rotation were incorrect. Restoring this path removes both
remaining `OnUpdate` helpers: **41 → 39** strict link gaps, with no
new names. Focused source-shape rises **81.8% → 87.4%**, strict fuzzy
**34.4% → 36.8%**, without an exact match. Full ARM report
`_work/modstation_update_touch_scene_full_20261104.json` compares
4,323/4,524 originals in 4,770 pairs, mean source-shape **96.76%**,
strict fuzzy **86.76%**, 2,780 linked-exact, 1,108 byte-exact,
201 missing, zero skipped and no pair or exact-set losses. Windows
native, ARM64 static and ARMv7 209/209 builds pass. The touch velocity
handoff from `OnTouchEnd` and live scene movement remain unverified.

REA Android ARM `ModStation::OnUpdate` evidence
`ev_71b291d89b3d7ddb28763bd8cbfcf89586bad2363a58ae69b981ac5589b66fe2`
confirms the mission-116 station bitmask and mission-148–151 dialogue
transitions. The special dialogue uses text IDs 2716–2719, speaker
texts 1516/1597, sounds 1561–1564, zero portrait data at BSS
`0x217bf8`, and portrait arrays at `0x21562c`/`0x215640`. The old
code had null text roots, invalid portrait pointers, missing status
bits and truncated mission pointers. REA `Status::missionFailed`
evidence `ev_9ba7febcbbbf5f45b523144cc88595161750b893fb23c1171003d3d23540eea3`
confirms its `Mission*` return type. The current `removeMission`
implementation deletes the mission, so `OnUpdate` now reads its type
first. Eighteen guessed helpers were removed; strict link gaps fall
**59 → 41** without new names. Focused `OnUpdate` source-shape changes
**83.3% → 81.8%** and strict fuzzy **41.8% → 34.4%** as guessed
branches are replaced with the original behavior. Full ARM report
`_work/modstation_update_missions_full_20261104.json` compares
4,323/4,524 originals in 4,770 pairs: mean source-shape **96.76%**,
strict fuzzy **86.76%**, 2,780 linked-exact, 1,108 byte-exact,
201 missing, zero skipped, and no lost pairs or exact-match members.
Windows native, ARM64 static and ARMv7 209/209 builds pass. At that
stage two `OnUpdate` names remained in the late scene path; live
behavior still needs a newly linked APK.

REA Android ARM `ModStation::OnUpdate` evidence
`ev_71b291d89b3d7ddb28763bd8cbfcf89586bad2363a58ae69b981ac5589b66fe2`
confirms the free-camera `EaseInOut` advance and random range renewal,
direct current-camera translation, elapsed-time updates for the news
ticker and active windows, and the text-601 notice flag at
`Globals::hints[0x2a]`. Eleven guessed helper calls were replaced by
typed methods; strict link gaps fall **70 → 59**, with no new names.
Focused `OnUpdate` source-shape rises **76.3% → 83.3%** and strict
fuzzy **27.1% → 41.8%**, without an exact match. Full report
`_work/modstation_update_camera_window_full_20261104.json` compares
4,323/4,524 originals in 4,770 pairs, mean source-shape **96.76%**,
strict fuzzy **86.76%**, with 2,780 linked-exact, 1,108 byte-exact,
201 missing, zero skipped and no pair or exact-set losses. Windows
native, ARM64 static and ARMv7 209/209 builds pass. Camera movement
and the notice still need live validation in a newly linked APK.
At that stage the mission-result branch still had null placeholders
and 20 unresolved `OnUpdate` names; those paths were recovered in the
next step above.

REA decompilation of Android ARM `ModStation::checkHints`
(`ev_725994ccb8c97a5ae0f408bb00cd8bde30e6fa31687fc965f53572c0a2d77590`)
resolved all 13 helper names. The first hint reads `Globals::options[0x34]`;
the four wanted notices read `Globals::hints[0x33..0x36]`, use text 3232,
and substitute `#SHIP_NAME` and `#WANTED_NAME`. Medal notices use texts
649/650, blueprint 232 uses 651, and the later campaign hint uses 3233.
The wingman path now follows the original gates, with dialogue 442/1662
and the 20,000-credit award only when status byte `0xf9` is set, or
dialogue 313 for an available wingman. The unknown wider lifecycle of
byte `0xf9` and live dialogue behavior remain unverified. Focused
source-shape rises **25.0% → 63.5%**, strict fuzzy **12.7% → 35.0%**;
there is no exact match. The full report
`_work/modstation_checkhints_recovered_full_20261104.json` compares
4,323/4,524 originals in 4,770 pairs, mean source-shape **96.76%**,
strict fuzzy **86.75%**, with 2,780 linked-exact, 1,108 byte-exact,
201 missing, zero skipped, and no lost pairs or exact-match members.
Windows native, ARM64 static, and ARMv7 209/209 builds pass. At that
stage the 70 remaining names belonged to `OnUpdate` (31) and
`OnTouchEnd` (39).

REA then confirmed that this function reads `Globals::status` at
offsets `0xf8`, `0x108`, and `0x114`, and invokes the existing
`Status::getCurrentCampaignMission` and `Station::getIndex` methods.
Four null-placeholder field reads were replaced, and 21 calls to each
of the two methods now use their typed receivers. This removes two
unresolved helper symbols. Focused `OnInitialize` source-shape rises
72.2% → **73.5%**, while strict fuzzy changes 27.3% → **25.0%**.
The full ARM report (`_work/modstation_init_status_full_20261104.json`)
still compares 4,323/4,524 originals in 4,770 pairs, with 96.75%
mean source-shape, 86.75% strict fuzzy, 2,780 linked-exact, 1,108
byte-exact, 201 missing library/ABI symbols, and zero skipped. The
pair, linked-exact, and byte-exact sets have no losses against the
preceding report. Windows native and ARM64 static builds pass; ARMv7
compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.

The two `g_oi_status` reads in victory-notice checks were unresolved
at this checkpoint. The newer notice recovery below maps the original
flags to `Globals::options[0x35]` and `[0x37]` using ELF symbols.

REA also confirms six existing `Status` methods in this function:
`inAlienOrbit`, `inSupernovaSystem`, `inBlackMarketSystem`, `gameWon`,
`dlc1Won`, and `activateNewWanted`. Eleven calls now use the typed
`Globals::status` receiver, removing six more unresolved helpers.
Focused source-shape changes 73.5% → **73.3%**, while strict fuzzy
rises 25.0% → **29.2%**. The full ARM report
(`_work/modstation_init_predicates_full_20261104.json`) still has
4,323/4,524 originals in 4,770 pairs, 96.75% / 86.75% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
with no pair or exact-set losses. Native and ARM64 static builds pass;
ARMv7 compiles 209/209 units. REA separately confirmed the existing
`inSupernovaSystem` body (`ev_16b962d376e3076dfb7d1bfb2715774d0469b4d9be497a5264615aa1838934af`).
The called `activateNewWanted` body remains only 89.5% source-shape;
its full semantics and live station initialization are unverified.

In the next bounded section, 21 `Status::getStation` calls already used
as `Station*` now use the typed, linked-exact getter, avoiding pointer
truncation on ARM64. The original also calls
`Station::stationHasPirateBase` on the returned station and checks
`Status::getFreelanceMission` for a null mission; restoring these
receivers removes two more unresolved helpers. Focused `OnInitialize`
source-shape rises 73.3% → **73.9%**, strict fuzzy 29.2% → **31.8%**.
The full ARM report (`_work/modstation_init_typed_station_full_20261104.json`)
still compares 4,323/4,524 originals in 4,770 pairs, 96.75% /
86.75% means, 2,780 linked-exact, 1,108 byte-exact, 201 missing,
and zero skipped, with no pair or exact-set losses. Windows native and
ARM64 static builds pass; ARMv7 compiles 209/209 units. The remaining
`Status_getStation_oi` uses include invalid `Item*`/`Ship*` casts,
which require original data-flow recovery. The existing pirate-base
method body is 92.1% source-shape and is not yet live-validated.

REA confirms the hostile-station notice branch: station attack or
hostile faction standing triggers a fee calculated from standing or
player level, a random offset, and the hardcore multiplier. The fee
is formatted into game text and shown through `ChoiceWindow::set`;
the original sets window flags at `0x63` and `0x6b` and clears the
touch flag at `0x101`. Reconstructing this flow removes four helper
symbols: `Station_hasAttackedFriends_oi`, `Standing_isEnemy_oi`,
`Status_getStanding_oi`, and `ChoiceWindow_setFee_oiImpl`.
Focused `OnInitialize` source-shape rises 73.9% → **80.2%**; strict
fuzzy falls 31.8% → **20.6%**. The full ARM report
(`_work/modstation_init_hostile_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.74% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped.
No pair or exact-set losses occurred. Native and ARM64 static builds
pass; ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
This choice-window branch is not yet live-validated in a linked APK.

REA confirms the next two campaign branches. At mission 20's target
station, existing item 41 gets price zero and a ten-unit zero-price
copy of item 41 is added. At mission 27's target station, cargo 131 is
removed from the player ship. Typed calls now retain the actual
`Mission`, `Station`, `Item`, and `Ship` receivers, removing six
unresolved helpers: `Status_getCampaignMission_oi`,
`Station_getItems_oi`, `Item_getIndex_oi`, `Item_setPrice_oi`,
`Item_makeItemDescAmt_oi`, and `Ship_removeCargo_oi`.
Focused `OnInitialize` source-shape rises 80.2% → **81.0%**, strict
fuzzy changes 20.6% → **20.2%**. The full ARM report
(`_work/modstation_init_mission_items_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.74% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
without pair or exact-set losses. Native and ARM64 static builds pass;
ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
`Station::addItem` is 94.8% source-shape; this mission path has not
been live-validated in a linked APK. The item-85 and fuel-122 supply
branch is next.

REA confirms the station supply paths. At station 10 after the game
is won, item 164 is added in a 50-unit copy only if the player's
equipment/cargo lacks item 85 and the station lacks items 85 and 164.
The fuel path reads cargo 122 and adds a ten-unit item 122 when the
player has at most five and the station has none. Restoring typed
receivers removes `Ship_hasEquipment_oi`, `Ship_getCargo_oi`, and
`Item_getAmount_oi`; no new link errors appear. Focused
`OnInitialize` source-shape rises 81.0% → **81.7%** and strict fuzzy
changes 20.2% → **20.1%**. The full ARM report
(`_work/modstation_init_fuel_full_20261104.json`) still compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.74% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
without pair or exact-set losses. Windows native and ARM64 static
builds pass; ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
The existing ship cargo/equipment helpers are non-exact internally;
this supply path has not been live-validated in a linked APK.

REA confirms the gold-medal ship-list branch at station 10. When the
player has all gold medals and ship 8 is absent from the station's
ship list, the original constructs a temporary `Generator`, builds a
buy list for the station, calls `Station::setShips(list, false)`, and
destroys the generator. Typed receivers and ownership now follow this
flow, removing five unresolved helpers: `Achievements_gotAllGoldMedals_oi`,
`Generator_ctor_oi`, `Generator_dtor_oi`,
`Generator_getShipBuyList_oi`, and `Station_setShips_oi`.
Focused `OnInitialize` source-shape changes 81.7% → **81.5%** and
strict fuzzy 20.1% → **19.7%**. The full ARM report
(`_work/modstation_init_medals_full_20261104.json`) still compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.74% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
without pair or exact-set losses. Native and ARM64 static builds pass;
ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
`Generator::getShipBuyList` is 96.7% source-shape and
`Station::setShips` is 96.1%; this path is not yet live-validated.

REA confirms the station-100 ship replacement branch: ship 37 is
removed if present, then ships 38 and 40 are created from the global
ship table with price `-1` and added if absent. The reconstructed code
now passes the actual ship to `removeShip` and created objects to
`addShip`, removing `Station_hasShip_oi`, `Station_removeShip_oi`, and
`Station_addShip_oi`. Focused `OnInitialize` source-shape rises
81.5% → **81.9%**; strict fuzzy changes 19.7% → **19.6%**. The full
ARM report (`_work/modstation_init_ship_swap_full_20261104.json`)
still compares 4,323/4,524 originals in 4,770 pairs, with 96.75% /
86.74% means, 2,780 linked-exact, 1,108 byte-exact, 201 missing,
and zero skipped, without pair or exact-set losses. Native and ARM64
static builds pass; ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
`Station::addShip` is 91.0% source-shape and the new station path is
not yet live-validated in a linked APK.

REA confirms that freelance mission type 14 adds one copy of item 115
from `Globals::items` at its target station when the station lacks
that item. The branch now uses typed `Mission`, `Station`, and `Item`
receivers, removing `Mission_getType_oi`,
`Mission_getTargetStation_oi`, `Station_hasItem_oi`,
`Item_makeItemDesc_oi`, and `Station_addItem_oi`. Focused
`OnInitialize` source-shape rises 81.9% → **82.2%** and strict fuzzy
changes 19.6% → **19.5%**. The full ARM report
(`_work/modstation_init_freelance_full_20261104.json`) still compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.74% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
without pair or exact-set losses. Native and ARM64 static builds pass;
ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
`Station::addItem` is 94.8% source-shape; this path is not yet
live-validated. At this stage the reward-station branch still checked
the wrong credit threshold and omitted the original cargo amount.

The reward-station state-2 offer is now restored. REA shows station
108, status state 2, at least 30,000,001 credits, and at least 50
units of cargo 109. `ChoiceWindow::set` receives game text 477 with
buttons when eligible and text 476 otherwise; the original sets
window flags at `0x6c` and `0x63`. Typed calls replace the incorrect
100,000-credit/no-amount check, removing `Status_getCredits_oi`.
Focused `OnInitialize` source-shape rises 82.2% → **82.4%** and
strict fuzzy 19.5% → **43.8%**. The full ARM report
(`_work/modstation_init_reward_offer_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.75% means,
2,780 linked-exact, 1,108 byte-exact, 201 missing, and zero skipped,
without pair or exact-set losses. Native and ARM64 static builds pass;
ARMv7 compiles 209/209 units. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
The offer is not yet live-validated.

The remaining state-40 notices now use the original global arrays.
ELF symbols place `Globals::options` at `0x2181E8` and the 59-byte
`Globals::hints` at `0x21824C`. Thus `dword_21821D` bytes 0/2 map to
options indices `0x35`/`0x37`, `byte_218277` maps to hint `0x2b`,
and `word_218284` bytes 0/1 map to hints `0x38`/`0x39`. Text IDs
613, 110, 111, 3230, and 3231 are restored; the last text substitutes
the wanted count for `#N`. `Globals::enterSpaceLounge` replaces the
remaining null demo-flag placeholder. The pirate-base path sets
`alarmFlags.bytes[1]` as in Android ARM and no longer invents a
six-text ChoiceWindow. These replacements remove all five remaining
`OnInitialize` unresolved helper names: strict ARM64 link gaps fall
88 → **83**. Focused source-shape rises 89.9% → **91.7%** and strict
fuzzy 39.5% → **43.0%**. Windows native, ARM64 static, and ARMv7
209/209 builds pass. The wider pirate-base control flow and notices
still require live validation in a newly linked APK. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
The full ARM report (`_work/modstation_init_notices_full_20261104.json`)
compares 4,323/4,524 originals in 4,770 pairs, with 96.75% /
86.75% mean source-shape/strict fuzzy, 2,780 linked-exact,
1,108 byte-exact, 201 missing, and zero skipped. No comparison
pairs or exact-match sets were lost against the camera report.

The state-20 station camera entry is now recovered from Android ARM.
Before resetting the light, it reads the current camera matrix's
direction and subtracts 0.2 from X and 0.3 from Y. After the
mission-77 price adjustment, it chooses the race, reads coordinates
from the original phone table `0x1FDFB8`, resets the three
`EaseInOut` ranges, and sets a camera matrix with pitch, device
yaw, and roll `-0.03`. The cutscene receives the initial position,
angles, geometry matrix, and three random position offsets. A
`CutScene` union now models the station camera angle vector at
ARM32 offset `0x14` without changing the object size. Typed
`PaintCanvas` calls remove two more unresolved wrappers: the strict
ARM64 link falls 90 → **88**. Focused `OnInitialize` source-shape
rises 85.0% → **89.9%**, while strict fuzzy changes 44.8% →
**39.5%**. Windows native, ARM64 static, and ARMv7 209/209 builds
pass. The iOS coordinate behavior is unverified; Android ARM uses
the phone coordinates here even when it selects the iPad yaw table.
The scene is not yet live-validated in a newly linked APK. Original
REA evidence: `ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
The full ARM report (`_work/modstation_init_camera_final_full_20261104.json`)
compares 4,323/4,524 originals in 4,770 pairs, with 96.75% /
86.75% mean source-shape/strict fuzzy, 2,780 linked-exact,
1,108 byte-exact, 201 missing, and zero skipped. No comparison
pairs or exact-match sets were lost against the system-race report.

The remaining system-race lookup in the station camera path now uses
`Status::getSystem()->getRace()` when the station index is neither
101 nor 100, as in Android ARM. Two unresolved helpers disappear:
the strict ARM64 link falls 92 → **90**. Focused `OnInitialize`
source-shape remains **85.0%** and strict fuzzy **44.8%**; Windows
native, ARM64 static, and ARMv7 209/209 builds pass. At this
checkpoint the camera setup was still partial; the preceding camera
entry above reconstructs the direction, animation ranges, matrix,
and geometry updates. Original REA
evidence: `ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.
The full ARM report (`_work/modstation_init_system_race_full_20261104.json`)
compares 4,323/4,524 originals in 4,770 pairs, with 96.75% /
86.75% mean source-shape/strict fuzzy, 2,780 linked-exact,
1,108 byte-exact, 201 missing, and zero skipped. No comparison
pairs or exact-match sets were lost.

The mission-77 ship price branch is restored. The original Android
ARM code visits station 100's ship array and sets the price of every
ship with index 37 to zero. Typed `Station` and `Ship` calls replace
four unresolved helpers. Focused `OnInitialize` source-shape rises
84.9% → **85.0%**, with strict fuzzy unchanged at **44.8%**.
Windows native, ARM64 static, and ARMv7 209/209 builds pass; the
strict ARM64 link falls 96 → **92** unresolved helpers. This path
has not been live-validated in a newly linked APK. The full ARM report
(`_work/modstation_init_ship_price_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.75% mean
source-shape/strict fuzzy, 2,780 linked-exact, 1,108 byte-exact,
201 missing, and zero skipped. No comparison pairs or exact-match
sets were lost. Original REA
evidence: `ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.

The mission-1 starter ship path is recovered from Android ARM. The
original clones ship template 0 with price `-1`, installs it through
`Status::setShip`, sets race 8, creates items 90 and 81, marks both
unsaleable, installs them in slots 0 and 1, then calls
`CutScene::replacePlayerShip` with the new ship's index and variant 3.
The previous code dereferenced a null placeholder and used seven
unresolved helper names. Focused `OnInitialize` source-shape rises
84.5% → **84.9%**; strict fuzzy remains **44.8%**. Windows native,
ARM64 static, and ARMv7 209/209 builds pass; the strict ARM64 link
falls 103 → **96** unresolved helpers. This path is not yet
live-validated in a newly linked APK. The full ARM report
(`_work/modstation_init_starter_ship_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.75% mean
source-shape/strict fuzzy, 2,780 linked-exact, 1,108 byte-exact,
201 missing, and zero skipped. No comparison pairs or exact-match
sets were lost. Original REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.

The route warning in state 20 now follows the original Android ARM
branch: `Globals::layout` byte `0x2ec` is clear, station is not 101,
the ship has neither a jump drive nor 1 unit of cargo item 85, the
system has no routes, and the campaign mission is at least 17. It
shows game text 335 with the ChoiceWindow button. The previous
implementation dereferenced a null placeholder and called five
unresolved helpers; typed calls remove all five. The strict ARM64
link falls 108 → **103** unresolved helpers. Focused source-shape
rises 84.2% → **84.5%**, while strict fuzzy changes 45.0% →
**44.8%**. Windows native, ARM64 static, and ARMv7 209/209 builds
pass. The full ARM report
(`_work/modstation_init_no_routes_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.75% mean
source-shape/strict fuzzy, 2,780 linked-exact, 1,108 byte-exact,
201 missing, and zero skipped. No comparison pairs or exact-match
sets were lost. The warning remains unverified in a freshly linked APK.
Original REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.

The reward-station state-1 dialogue is also recovered. The original
Android `.data` at `0x2156A4` contains the five-word client portrait
`{4, 2, 0, 2, 2}`. REA shows the previous dialogue being destroyed,
then a mission with ID `-1`, text 1601, this portrait, race 4, and
target station 108 being created and marked won. State advances to 2,
then a `DialogueWindow` is constructed and its modal flag enabled.
The focused `OnInitialize` comparison rises 82.4% → **84.2%**
source-shape and 43.8% → **45.0%** strict fuzzy. Windows native,
ARM64 static, and ARMv7 209/209 builds pass. The strict ARM64 link
still has **108** unresolved helpers because the remaining uses of
its ChoiceWindow wrapper occur elsewhere. This branch remains
unverified in a freshly linked APK. The full ARM report
(`_work/modstation_init_reward_dialogue_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% / 86.75% mean
source-shape/strict fuzzy, 2,780 linked-exact, 1,108 byte-exact,
201 missing, and zero skipped. No comparison pairs or exact-match
sets were lost against the preceding report. REA evidence:
`ev_fde7c5f6ddbb9d335fd57806d522f3253b7b40199427cd9ebe82f6ebd61fe7ea`.

For the earlier cutscene-only change, Windows native and ARM64 static builds
pass; ARMv7 compiles 209/209 units. Focused `OnInitialize` remains
**72.2%** source-shape and **27.3%** strict fuzzy. The full ARM corpus
(`_work/modstation_init_cutscene_full_20261104.json`) compares
4,323/4,524 originals in 4,770 pairs, with 96.75% mean source-shape,
86.75% strict fuzzy, 2,780 linked-exact, 1,108 byte-exact, 201
missing library/ABI symbols, and zero skipped. Compared with the
preceding release-path report, there are no lost comparison pairs or
exact matches. This branch of `OnInitialize` has not been exercised
in a newly linked APK.

The installed Poco APK was built earlier and does not include these changes.
A new APK cannot be linked from current source until these calls are recovered
or otherwise correctly implemented. Success-returning stubs would mask missing
game behavior and are not a valid closure. The earlier
`ModStation::checkHints` null placeholders and missing wanted-name
substitutions have since been corrected as described above. Earlier REA evidence:
`ev_7e1aa9c7284245af140363313e649e4c93e2bd1306315edcf425f8785f24b1d3`.

## `ModStation::OnInitialize()` — 0 unresolved

Its remaining source-shape and semantic differences still need
verification; zero unresolved helper names is only a link property.

## `ModStation::OnUpdate()` — closed

The strict ARM64 link has no unresolved helper names from this function.
Focused source-shape and live behavior remain separate validation tasks.

## `ModStation::OnTouchEnd(int, int, void*)` — 35 unresolved

- `Achievements_resetNewMedals_ote(void*)`
- `Agent_getOffer_ote(Agent*)`
- `Agent_getSellItemIndex_ote(Agent*)`
- `Agent_setEvent_ote(Agent*, int)`
- `Agent_setOfferAccepted_ote(Agent*, int)`
- `ChoiceWindow_setFee_frag(void*, int, int)`
- `ChoiceWindow_setNotice_ote(int, int)`
- `Galaxy_getStation_ote(int)`
- `GameText_getText_ote(int)`
- `HangarWindow_dtor_ote(HangarWindow*)`
- `Item_makeItem_ote(int)`
- `Mission_getBonus_ote()`
- `Mission_getReward_ote()`
- `RecordHandler_saveOptions_ote(RecordHandler*)`
- `Ship_addCargo_ote(Item*)`
- `Ship_makeShip_ote(int)`
- `Ship_removeCargo_ote(int, int)`
- `SpaceLounge_refresh_ote()`
- `Station_addShip_ote(Ship*)`
- `Station_getAgents_ote(Station*)`
- `Station_getIndex_ote(Station*)`
- `Station_hasShip_ote(void*)`
- `Station_setAttackedFriends_ote(Station*, int)`
- `Station_setItems_ote(Station*, void*, int)`
- `Station_stationHasPirateBase_ote()`
- `Status_activateNewWanted_ote()`
- `Status_changeCredits_ote(int)`
- `Status_departStation_ote(Station*)`
- `Status_getCredits_ote()`
- `Status_getCurrentCampaignMission_ote()`
- `Status_getShip_ote()`
- `Status_getStation_ote()`
- `Status_removeMission_ote(int)`
- `Status_setMission_ote(int)`
- `Status_setStation_ote(Station*)`

## `ModStation::checkHints()` — closed

All 13 formerly unresolved helper names were removed by direct calls
and typed state access. See the recovery evidence and limits above.
