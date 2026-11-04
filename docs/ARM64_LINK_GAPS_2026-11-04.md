# ARM64 shared-library link gaps (2026-11-04)

A strict `--whole-archive --no-undefined` ARM64 link of the current `libgof2.a`
against the project libzip/crypto archives and Android system libraries reports
**145 unresolved symbols**, all referenced from `ModStation.cpp`. These are
reconstruction helper names, not 145 proven absent symbols in the original ARM
library. Many can map to existing C++ methods; each mapping needs source and ABI
confirmation before replacement. The raw linker evidence is
`D:/gof2-arm64-crypto/modstation-init-cutscene-link-20261104.log` (outside Git).

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
159 → 158 → 147 → **145**.

For this initialization change, Windows native and ARM64 static builds
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
game behavior and are not a valid closure. For example, `ModStation::checkHints`
currently dereferences placeholder pointers initialized to null and omits the
original's wanted-name text substitutions. REA original evidence:
`ev_7e1aa9c7284245af140363313e649e4c93e2bd1306315edcf425f8785f24b1d3`.

## `ModStation::OnInitialize()` — 62 unresolved

- `Achievements_gotAllGoldMedals_oi()`
- `ChoiceWindow_set1_oiImpl(void*, int)`
- `ChoiceWindow_set6_oiImpl(void*, int, int, int, int, int, int)`
- `ChoiceWindow_setFee_oiImpl(void*, int, int)`
- `ChoiceWindow_setNotice_oi(void*, int)`
- `ChoiceWindow_set_oi(void*, int, int)`
- `CutScene_replacePlayerShip_oi(int, int)`
- `GameText_getText_oi(int)`
- `GameText_getText_oiImpl(int)`
- `Generator_ctor_oi(Generator*)`
- `Generator_dtor_oi(Generator*)`
- `Generator_getShipBuyList_oi(Station*)`
- `Item_getAmount_oi()`
- `Item_getIndex_oi(Item*)`
- `Item_makeItemDescAmt_oi(int, int)`
- `Item_makeItemDesc_oi(int)`
- `Item_makeItem_oi()`
- `Item_setPrice_oi(int)`
- `Item_setUnsaleable_oi(int)`
- `Mission_getTargetStation_oi()`
- `Mission_getType_oi()`
- `ModStation_oi_cameraHandle()`
- `ModStation_oi_setCameraLocal(unsigned int, AbyssEngine::AEMath::Matrix const&)`
- `Ship_getCargo_oi(int)`
- `Ship_getIndex_oi()`
- `Ship_hasCargo_oi(int, int)`
- `Ship_hasEquipment_oi(int, int)`
- `Ship_hasJumpDrive_oi()`
- `Ship_makeShip_oi(int)`
- `Ship_removeCargo_oi(int)`
- `Ship_setEquipment_oi(Item*, int)`
- `Ship_setPrice_oi(int)`
- `Ship_setRace_oi(int)`
- `SolarSystem_getRace_oi()`
- `SolarSystem_getRoutes_oi()`
- `Standing_isEnemy_oi(int)`
- `Station_addItem_oi(Item*)`
- `Station_addShip_oi(Ship*)`
- `Station_getIndex_oi(Station*)`
- `Station_getItems_oi()`
- `Station_getShips_oi()`
- `Station_hasAttackedFriends_oi()`
- `Station_hasItem_oi(int)`
- `Station_hasShip_oi(void*)`
- `Station_removeShip_oi(Ship*)`
- `Station_setShips_oi(void*, int)`
- `Station_stationHasPirateBase_oi()`
- `Status_activateNewWanted_oi()`
- `Status_dlc1Won_oi()`
- `Status_gameWon_oi()`
- `Status_getCampaignMission_oi()`
- `Status_getCredits_oi()`
- `Status_getCurrentCampaignMission_oi()`
- `Status_getFreelanceMission_oi()`
- `Status_getShip_oi()`
- `Status_getStanding_oi()`
- `Status_getStation_oi()`
- `Status_getSystem_oi()`
- `Status_inAlienOrbit_oi()`
- `Status_inBlackMarketSystem_oi()`
- `Status_inSupernovaSystem_oi()`
- `Status_setShip_oi(Ship*)`

## `ModStation::OnUpdate()` — 31 unresolved

- `AEGeometry_rotate_ou(void*, float, float, float)`
- `ChoiceWindow_setNotice_ou(int, int)`
- `ChoiceWindow_update_ou(int)`
- `CutScene_process_ou(int)`
- `EaseInOut_advance_ou(void*, int)`
- `Engine_setHangarLightIntensity_ou(float)`
- `GameText_getText_ou(int)`
- `HangarWindow_update_ou(int)`
- `MenuTouchWindow_update_ou(void*, int)`
- `Mission_ctor_ou(Mission*, int, int, int)`
- `Mission_getStatusValue_ou()`
- `Mission_getType_ou()`
- `Mission_setCampaignMission_ou(void*)`
- `Mission_setStatusValue_ou(int)`
- `ModStation_ou_cameraHandle()`
- `ModStation_ou_setCameraLocal(unsigned int, AbyssEngine::AEMath::Matrix const&)`
- `SolarSystem_getIndex_ou()`
- `SpaceLounge_introFinished_ou()`
- `SpaceLounge_setHangarUpdate_ou(void*)`
- `SpaceLounge_update_ou(int)`
- `StarMap_update_ou(int, int)`
- `Station_getIndex_ou(Station*)`
- `StatusWindow_update_ou(int)`
- `Status_getCampaignMission_ou()`
- `Status_getCurrentCampaignMission_ou()`
- `Status_getStation_ou()`
- `Status_getSystem_ou()`
- `Status_missionCompleted_ou(int, int, long long)`
- `Status_missionFailed_ou(int, int, int)`
- `Status_removeMission_ou(int)`
- `Status_setCurrentCampaignMission_ou(int)`

## `ModStation::OnTouchEnd(int, int, void*)` — 39 unresolved

- `Achievements_resetNewMedals_ote(void*)`
- `Agent_getOffer_ote(Agent*)`
- `Agent_getSellItemIndex_ote(Agent*)`
- `Agent_setEvent_ote(Agent*, int)`
- `Agent_setOfferAccepted_ote(Agent*, int)`
- `ChoiceWindow_setFee_frag(void*, int, int)`
- `ChoiceWindow_setNotice_ote(int, int)`
- `CutScene_replacePlayerShip_ote(int, int)`
- `Galaxy_getStation_ote(int)`
- `GameText_getText_ote(int)`
- `HangarWindow_dtor_ote(HangarWindow*)`
- `Item_makeItem_ote(int)`
- `Mission_getBonus_ote()`
- `Mission_getReward_ote()`
- `RecordHandler_saveOptions_ote(RecordHandler*)`
- `Ship_addCargo_ote(Item*)`
- `Ship_getIndex_ote()`
- `Ship_getRace_ote(Ship*)`
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
- `Status_nextCampaignMission_ote(int)`
- `Status_removeMission_ote(int)`
- `Status_setMission_ote(int)`
- `Status_setStation_ote(Station*)`

## `ModStation::checkHints()` — 13 unresolved

- `Achievements_gotAllGoldMedals_ch()`
- `Achievements_gotAllMedals_ch(Achievements*)`
- `Achievements_gotAllSupernovaMedals_ch(Achievements*)`
- `ChoiceWindow_set1_ch(void*, int)`
- `ChoiceWindow_set6_ch(void*, int, int, int, int, int)`
- `DialogueWindow_initWingman_ch(void*, int)`
- `FModSound_playWingmanRecruit_ch()`
- `GameText_getText_ch(int)`
- `GameText_text_ch(int)`
- `Status_getCurrentCampaignMission_ch()`
- `Status_hardCoreMode_ch()`
- `Status_isBlueprintUnlocked_ch(void*, int)`
- `Status_unlockBluePrint_ch(void*, int)`
