# ARM64 shared-library link gaps (2026-11-04)

A strict `--whole-archive --no-undefined` ARM64 link of the current `libgof2.a`
against the project libzip/crypto archives and Android system libraries reports
**158 unresolved symbols**, all referenced from `ModStation.cpp`. These are
reconstruction helper names, not 158 proven absent symbols in the original ARM
library. Many can ultimately map to existing C++ methods; each mapping needs
source and ABI confirmation before replacement. The raw linker evidence is
`D:/gof2-arm64-crypto/startup-link-after-fbo-20261104.log` (outside Git).

Before the FBO correction, there were 159 unresolved symbols. REA showed
`PaintCanvas::StartDraw2FBO` directly calls
`Engine::ActivateRender2TextureFBO`; replacing the unresolved
`paintcanvas_ext_start_fbo` wrapper removed that one link error.

The installed Poco APK was built earlier and does not include the latest
startup fixes. A new APK cannot be linked from this source until these
unresolved calls are recovered or otherwise correctly implemented.

Call-site groups: `OnInitialize` 64, `OnTouchEnd` 39, `OnUpdate` 31,
`checkHints` 13, and `OnRelease` 11. These counts are distinct by
caller in this link. Prioritize the release/initialization lifecycle
and the title-to-station transition, then the station interaction
paths. Do not replace these with success-returning stubs: that would
allow the APK to link while masking missing game behavior.

| Unresolved symbol | First recorded call site |
| --- | --- |
| `AEGeometry_rotate_ou(void*, float, float, float)` | `ModStation.cpp:1280` |
| `Achievements_gotAllGoldMedals_ch()` | `ModStation.cpp:1432` |
| `Achievements_gotAllGoldMedals_oi()` | `ModStation.cpp:3211` |
| `Achievements_gotAllMedals_ch(Achievements*)` | `ModStation.cpp:1404` |
| `Achievements_gotAllSupernovaMedals_ch(Achievements*)` | `ModStation.cpp:1433` |
| `Achievements_resetNewMedals_ote(void*)` | `ModStation.cpp:2176` |
| `Agent_getOffer_ote(Agent*)` | `ModStation.cpp:2228` |
| `Agent_getSellItemIndex_ote(Agent*)` | `ModStation.cpp:2228` |
| `Agent_setEvent_ote(Agent*, int)` | `ModStation.cpp:2229` |
| `Agent_setOfferAccepted_ote(Agent*, int)` | `ModStation.cpp:2230` |
| `Array_RM_dtor(void*)` | `ModStation.cpp:1587` |
| `ChoiceWindow_dtor(void*)` | `ModStation.cpp:1568` |
| `ChoiceWindow_set1_ch(void*, int)` | `ModStation.cpp:1438` |
| `ChoiceWindow_set1_oiImpl(void*, int)` | `ModStation.cpp:3270` |
| `ChoiceWindow_set6_ch(void*, int, int, int, int, int)` | `ModStation.cpp:1374` |
| `ChoiceWindow_set6_oiImpl(void*, int, int, int, int, int, int)` | `ModStation.cpp:3101` |
| `ChoiceWindow_setFee_frag(void*, int, int)` | `ModStation.cpp:2312` |
| `ChoiceWindow_setFee_oiImpl(void*, int, int)` | `ModStation.cpp:3139` |
| `ChoiceWindow_setNotice_oi(void*, int)` | `ModStation.cpp:3277` |
| `ChoiceWindow_setNotice_ote(int, int)` | `ModStation.cpp:0` |
| `ChoiceWindow_setNotice_ou(int, int)` | `ModStation.cpp:1143` |
| `ChoiceWindow_set_oi(void*, int, int)` | `ModStation.cpp:3046` |
| `ChoiceWindow_update_ou(int)` | `ModStation.cpp:1125` |
| `CutScene_ctor_oi(CutScene*, int)` | `ModStation.cpp:2925` |
| `CutScene_dtor(void*)` | `ModStation.cpp:1576` |
| `CutScene_initialize_oi(CutScene*)` | `ModStation.cpp:2927` |
| `CutScene_process_ou(int)` | `ModStation.cpp:1049` |
| `CutScene_replacePlayerShip_oi(int, int)` | `ModStation.cpp:2959` |
| `CutScene_replacePlayerShip_ote(int, int)` | `ModStation.cpp:1952` |
| `DialogueWindow_dtor(void*)` | `ModStation.cpp:1564` |
| `DialogueWindow_initWingman_ch(void*, int)` | `ModStation.cpp:1450` |
| `EaseInOut_advance_ou(void*, int)` | `ModStation.cpp:1055` |
| `Engine_setHangarLightIntensity_ou(float)` | `ModStation.cpp:1274` |
| `FModSound_playWingmanRecruit_ch()` | `ModStation.cpp:1452` |
| `Galaxy_getStation_ote(int)` | `ModStation.cpp:2238` |
| `GameText_getText_ch(int)` | `ModStation.cpp:1375` |
| `GameText_getText_oi(int)` | `ModStation.cpp:3046` |
| `GameText_getText_oiImpl(int)` | `ModStation.cpp:3102` |
| `GameText_getText_ote(int)` | `ModStation.cpp:0` |
| `GameText_getText_ou(int)` | `ModStation.cpp:1143` |
| `GameText_text_ch(int)` | `ModStation.cpp:1438` |
| `Generator_ctor_oi(Generator*)` | `ModStation.cpp:3229` |
| `Generator_dtor_oi(Generator*)` | `ModStation.cpp:3234` |
| `Generator_getShipBuyList_oi(Station*)` | `ModStation.cpp:3232` |
| `HangarWindow_dtor(void*)` | `ModStation.cpp:1549` |
| `HangarWindow_dtor_ote(HangarWindow*)` | `ModStation.cpp:2087` |
| `HangarWindow_update_ou(int)` | `ModStation.cpp:1115` |
| `Item_getAmount_oi()` | `ModStation.cpp:3201` |
| `Item_getIndex_oi(Item*)` | `ModStation.cpp:3160` |
| `Item_makeItemDescAmt_oi(int, int)` | `ModStation.cpp:3165` |
| `Item_makeItemDesc_oi(int)` | `ModStation.cpp:3188` |
| `Item_makeItem_oi()` | `ModStation.cpp:2953` |
| `Item_makeItem_ote(int)` | `ModStation.cpp:2057` |
| `Item_setPrice_oi(int)` | `ModStation.cpp:3161` |
| `Item_setUnsaleable_oi(int)` | `ModStation.cpp:2954` |
| `MenuTouchWindow_update_ou(void*, int)` | `ModStation.cpp:1127` |
| `Mission_ctor_ou(Mission*, int, int, int)` | `ModStation.cpp:1196` |
| `Mission_getBonus_ote()` | `ModStation.cpp:1955` |
| `Mission_getReward_ote()` | `ModStation.cpp:1954` |
| `Mission_getStatusValue_ou()` | `ModStation.cpp:1160` |
| `Mission_getTargetStation_oi()` | `ModStation.cpp:3155` |
| `Mission_getType_oi()` | `ModStation.cpp:3286` |
| `Mission_getType_ou()` | `ModStation.cpp:1248` |
| `Mission_setCampaignMission_ou(void*)` | `ModStation.cpp:1197` |
| `Mission_setStatusValue_ou(int)` | `ModStation.cpp:1169` |
| `MissionsWindow_dtor(void*)` | `ModStation.cpp:1560` |
| `ModStation_oi_cameraHandle()` | `ModStation.cpp:3014` |
| `ModStation_oi_setCameraLocal(unsigned int, AbyssEngine::AEMath::Matrix const&)` | `ModStation.cpp:3014` |
| `ModStation_ou_cameraHandle()` | `ModStation.cpp:1060` |
| `ModStation_ou_setCameraLocal(unsigned int, AbyssEngine::AEMath::Matrix const&)` | `ModStation.cpp:1060` |
| `Radio_dtor(void*)` | `ModStation.cpp:1583` |
| `RecordHandler_saveOptions_ote(RecordHandler*)` | `ModStation.cpp:0` |
| `ScrollTouchBox_dtor(void*)` | `ModStation.cpp:1625` |
| `Ship_addCargo_ote(Item*)` | `ModStation.cpp:2058` |
| `Ship_getCargo_oi(int)` | `ModStation.cpp:3200` |
| `Ship_getIndex_oi()` | `ModStation.cpp:2952` |
| `Ship_getIndex_ote()` | `ModStation.cpp:1949` |
| `Ship_getRace_ote(Ship*)` | `ModStation.cpp:1951` |
| `Ship_hasCargo_oi(int, int)` | `ModStation.cpp:3041` |
| `Ship_hasEquipment_oi(int, int)` | `ModStation.cpp:3182` |
| `Ship_hasJumpDrive_oi()` | `ModStation.cpp:3040` |
| `Ship_makeShip_oi(int)` | `ModStation.cpp:2950` |
| `Ship_makeShip_ote(int)` | `ModStation.cpp:2002` |
| `Ship_removeCargo_oi(int)` | `ModStation.cpp:3170` |
| `Ship_removeCargo_ote(int, int)` | `ModStation.cpp:0` |
| `Ship_setEquipment_oi(Item*, int)` | `ModStation.cpp:2955` |
| `Ship_setPrice_oi(int)` | `ModStation.cpp:3028` |
| `Ship_setRace_oi(int)` | `ModStation.cpp:2952` |
| `SolarSystem_getIndex_ou()` | `ModStation.cpp:1158` |
| `SolarSystem_getRace_oi()` | `ModStation.cpp:3131` |
| `SolarSystem_getRoutes_oi()` | `ModStation.cpp:3043` |
| `SpaceLounge_introFinished_ou()` | `ModStation.cpp:1151` |
| `SpaceLounge_refresh_ote()` | `ModStation.cpp:1962` |
| `SpaceLounge_setHangarUpdate_ou(void*)` | `ModStation.cpp:1253` |
| `SpaceLounge_update_ou(int)` | `ModStation.cpp:1119` |
| `Standing_isEnemy_oi(int)` | `ModStation.cpp:3132` |
| `StarMap_dtor(void*)` | `ModStation.cpp:1553` |
| `StarMap_update_ou(int, int)` | `ModStation.cpp:1113` |
| `Station_addItem_oi(Item*)` | `ModStation.cpp:3166` |
| `Station_addShip_oi(Ship*)` | `ModStation.cpp:3255` |
| `Station_addShip_ote(Ship*)` | `ModStation.cpp:2003` |
| `Station_getAgents_ote(Station*)` | `ModStation.cpp:2223` |
| `Station_getIndex_oi(Station*)` | `ModStation.cpp:3063` |
| `Station_getIndex_ote(Station*)` | `ModStation.cpp:2167` |
| `Station_getIndex_ou(Station*)` | `ModStation.cpp:1162` |
| `Station_getItems_oi()` | `ModStation.cpp:3157` |
| `Station_getShips_oi()` | `ModStation.cpp:3024` |
| `Station_hasAttackedFriends_oi()` | `ModStation.cpp:3126` |
| `Station_hasItem_oi(int)` | `ModStation.cpp:3186` |
| `Station_hasShip_oi(void*)` | `ModStation.cpp:3247` |
| `Station_hasShip_ote(void*)` | `ModStation.cpp:2049` |
| `Station_removeShip_oi(Ship*)` | `ModStation.cpp:3251` |
| `Station_setAttackedFriends_ote(Station*, int)` | `ModStation.cpp:2263` |
| `Station_setItems_ote(Station*, void*, int)` | `ModStation.cpp:0` |
| `Station_setShips_oi(void*, int)` | `ModStation.cpp:3233` |
| `Station_stationHasPirateBase_oi()` | `ModStation.cpp:3098` |
| `Station_stationHasPirateBase_ote()` | `ModStation.cpp:2171` |
| `StatusWindow_dtor(void*)` | `ModStation.cpp:1572` |
| `StatusWindow_update_ou(int)` | `ModStation.cpp:1121` |
| `Status_activateNewWanted_oi()` | `ModStation.cpp:3319` |
| `Status_activateNewWanted_ote()` | `ModStation.cpp:2108` |
| `Status_changeCredits_ote(int)` | `ModStation.cpp:2259` |
| `Status_departStation_ote(Station*)` | `ModStation.cpp:2175` |
| `Status_dlc1Won_oi()` | `ModStation.cpp:3240` |
| `Status_gameWon_oi()` | `ModStation.cpp:3180` |
| `Status_getCampaignMission_oi()` | `ModStation.cpp:3152` |
| `Status_getCampaignMission_ou()` | `ModStation.cpp:1159` |
| `Status_getCredits_oi()` | `ModStation.cpp:3137` |
| `Status_getCredits_ote()` | `ModStation.cpp:2257` |
| `Status_getCurrentCampaignMission_ch()` | `ModStation.cpp:1373` |
| `Status_getCurrentCampaignMission_oi()` | `ModStation.cpp:3064` |
| `Status_getCurrentCampaignMission_ote()` | `ModStation.cpp:2177` |
| `Status_getCurrentCampaignMission_ou()` | `ModStation.cpp:1154` |
| `Status_getFreelanceMission_oi()` | `ModStation.cpp:3286` |
| `Status_getShip_oi()` | `ModStation.cpp:3039` |
| `Status_getShip_ote()` | `ModStation.cpp:0` |
| `Status_getStanding_oi()` | `ModStation.cpp:3129` |
| `Status_getStation_oi()` | `ModStation.cpp:3062` |
| `Status_getStation_ote()` | `ModStation.cpp:2166` |
| `Status_getStation_ou()` | `ModStation.cpp:1161` |
| `Status_getSystem_oi()` | `ModStation.cpp:3130` |
| `Status_getSystem_ou()` | `ModStation.cpp:1157` |
| `Status_hardCoreMode_ch()` | `ModStation.cpp:1434` |
| `Status_inAlienOrbit_oi()` | `ModStation.cpp:3061` |
| `Status_inBlackMarketSystem_oi()` | `ModStation.cpp:3114` |
| `Status_inSupernovaSystem_oi()` | `ModStation.cpp:3084` |
| `Status_isBlueprintUnlocked_ch(void*, int)` | `ModStation.cpp:1419` |
| `Status_missionCompleted_ou(int, int, long long)` | `ModStation.cpp:1152` |
| `Status_missionFailed_ou(int, int, int)` | `ModStation.cpp:1232` |
| `Status_nextCampaignMission_ote(int)` | `ModStation.cpp:1940` |
| `Status_removeMission_ote(int)` | `ModStation.cpp:1959` |
| `Status_removeMission_ou(int)` | `ModStation.cpp:1237` |
| `Status_setCurrentCampaignMission_ou(int)` | `ModStation.cpp:1206` |
| `Status_setMission_ote(int)` | `ModStation.cpp:1971` |
| `Status_setShip_oi(Ship*)` | `ModStation.cpp:2951` |
| `Status_setStation_ote(Station*)` | `ModStation.cpp:0` |
| `Status_unlockBluePrint_ch(void*, int)` | `ModStation.cpp:1423` |
| `ms_op_delete(void*)` | `ModStation.cpp:1549` |
