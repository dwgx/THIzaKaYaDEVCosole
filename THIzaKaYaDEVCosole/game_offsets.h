#pragma once
// =============================================================================
// Touhou Mystia Izakaya - Debug/Cheat Method Offsets
// Extracted from: Touhou Mystia Izakaya_Data/dump.cs
// =============================================================================
// RVA  = Relative Virtual Address (offset from module base in memory)
// FileOffset = Raw file offset on disk (in the GameAssembly.dll)
// VA   = Full virtual address (with image base 0x180000000)
// =============================================================================

namespace Offsets {

// =============================================================================
// DaySceneDebugImpl (TypeDefIndex: 245) - "白天" (Daytime)
// =============================================================================
namespace DaySceneDebugImpl {

    // --- Core Registration ---
    constexpr uintptr_t RVA_RegisterMethod                  = 0x6E4B50;
    constexpr uintptr_t Off_RegisterMethod                  = 0x6E3550;
    constexpr uintptr_t RVA_UnRegisterMethod                = 0x6E68C0;
    constexpr uintptr_t Off_UnRegisterMethod                = 0x6E52C0;

    // --- Movement / Scene ---
    constexpr uintptr_t RVA_MoveCharacter                   = 0x6E4760;
    constexpr uintptr_t Off_MoveCharacter                   = 0x6E3160;
    constexpr uintptr_t RVA_ReturnCharacter                 = 0x6E4D50;
    constexpr uintptr_t Off_ReturnCharacter                 = 0x6E3750;
    constexpr uintptr_t RVA_Transmit                        = 0x6E6310;
    constexpr uintptr_t Off_Transmit                        = 0x6E4D10;

    // --- Kizuna (Bond) ---
    constexpr uintptr_t RVA_AddKizunaLevelExp               = 0x6E1A50;
    constexpr uintptr_t Off_AddKizunaLevelExp               = 0x6E0450;
    constexpr uintptr_t RVA_UpgradeKizunaLevel              = 0x6E6BB0;
    constexpr uintptr_t Off_UpgradeKizunaLevel              = 0x6E55B0;
    constexpr uintptr_t RVA_FullUpgradeAllKizunaLevel       = 0x6E1F80;
    constexpr uintptr_t Off_FullUpgradeAllKizunaLevel       = 0x6E0980;

    // --- Resources / Money ---
    constexpr uintptr_t RVA_GetMoney                        = 0x6E3680;
    constexpr uintptr_t Off_GetMoney                        = 0x6E2080;
    constexpr uintptr_t RVA_GetMaxResources                 = 0x6E3670;
    constexpr uintptr_t Off_GetMaxResources                 = 0x6E2070;
    constexpr uintptr_t RVA_GetTestResources                = 0x6E3EB0;
    constexpr uintptr_t Off_GetTestResources                = 0x6E28B0;
    constexpr uintptr_t RVA_GetRandomResources              = 0x6E36D0;
    constexpr uintptr_t Off_GetRandomResources              = 0x6E20D0;
    constexpr uintptr_t RVA_GetSilverFrogCoin               = 0x6E3E60;
    constexpr uintptr_t Off_GetSilverFrogCoin               = 0x6E2860;

    // --- Collectibles / Unlocks ---
    constexpr uintptr_t RVA_GetAllClothes                   = 0x6E22B0;
    constexpr uintptr_t Off_GetAllClothes                   = 0x6E0CB0;
    constexpr uintptr_t RVA_GetAllRecords                   = 0x6E3270;
    constexpr uintptr_t Off_GetAllRecords                   = 0x6E1C70;
    constexpr uintptr_t RVA_GetAllBadges                    = 0x6E2080;
    constexpr uintptr_t Off_GetAllBadges                    = 0x6E0A80;
    constexpr uintptr_t RVA_GetAndUseAllDecorations         = 0x6E3590;
    constexpr uintptr_t Off_GetAndUseAllDecorations         = 0x6E1F90;
    constexpr uintptr_t RVA_GetAllPartners                  = 0x6E3120;
    constexpr uintptr_t Off_GetAllPartners                  = 0x6E1B20;
    constexpr uintptr_t RVA_GetAllFishingTrophy             = 0x6E28D0;
    constexpr uintptr_t Off_GetAllFishingTrophy             = 0x6E12D0;
    constexpr uintptr_t RVA_UnlockSpecialGuestsAndMaps      = 0x6E6AD0;
    constexpr uintptr_t Off_UnlockSpecialGuestsAndMaps      = 0x6E54D0;
    constexpr uintptr_t RVA_UnlockAllMusicChapters          = 0x6E6950;
    constexpr uintptr_t Off_UnlockAllMusicChapters          = 0x6E5350;

    // --- Time / Day Progression ---
    constexpr uintptr_t RVA_NextDay                         = 0x6E47A0;
    constexpr uintptr_t Off_NextDay                         = 0x6E31A0;
    constexpr uintptr_t RVA_SetDays                         = 0x6E4E00;
    constexpr uintptr_t Off_SetDays                         = 0x6E3800;
    constexpr uintptr_t RVA_AddDays                         = 0x6E1960;
    constexpr uintptr_t Off_AddDays                         = 0x6E0360;
    constexpr uintptr_t RVA_WarpHours                       = 0x6E6C10;
    constexpr uintptr_t Off_WarpHours                       = 0x6E5610;
    constexpr uintptr_t RVA_WarpActions                     = 0x6E6C00;
    constexpr uintptr_t Off_WarpActions                     = 0x6E5600;

    // --- Experience ---
    constexpr uintptr_t RVA_AddExp                          = 0x6E1A00;
    constexpr uintptr_t Off_AddExp                          = 0x6E0400;

    // --- Clear Inventory ---
    constexpr uintptr_t RVA_ClearAllIngredients             = 0x6E1CB0;
    constexpr uintptr_t Off_ClearAllIngredients             = 0x6E06B0;
    constexpr uintptr_t RVA_ClearAllBeverages               = 0x6E1CA0;
    constexpr uintptr_t Off_ClearAllBeverages               = 0x6E06A0;
    constexpr uintptr_t RVA_ClearAllItems                   = 0x6E1CC0;
    constexpr uintptr_t Off_ClearAllItems                   = 0x6E06C0;

    // --- Missions / Events ---
    constexpr uintptr_t RVA_StartMission                    = 0x6E6260;
    constexpr uintptr_t Off_StartMission                    = 0x6E4C60;
    constexpr uintptr_t RVA_StartAllMissions                = 0x6E50A0;
    constexpr uintptr_t Off_StartAllMissions                = 0x6E3AA0;
    constexpr uintptr_t RVA_FinishNode                      = 0x6E1F30;
    constexpr uintptr_t Off_FinishNode                      = 0x6E0930;
    constexpr uintptr_t RVA_FinishAllEventsAndMissions      = 0x6E1CD0;
    constexpr uintptr_t Off_FinishAllEventsAndMissions      = 0x6E06D0;
    constexpr uintptr_t RVA_ScheduleEvent                   = 0x6E4D60;
    constexpr uintptr_t Off_ScheduleEvent                   = 0x6E3760;
    constexpr uintptr_t RVA_ScheduleNews                    = 0x6E4DB0;
    constexpr uintptr_t Off_ScheduleNews                    = 0x6E37B0;

    // --- Partner Mode ---
    constexpr uintptr_t RVA_SetMultiPartnerModeActive       = 0x6E4F40;
    constexpr uintptr_t Off_SetMultiPartnerModeActive       = 0x6E3940;
    constexpr uintptr_t RVA_SetMaxPartnerCount              = 0x6E4EB0;
    constexpr uintptr_t Off_SetMaxPartnerCount              = 0x6E38B0;

    // --- RogueLike ---
    constexpr uintptr_t RVA_MoveToRogueLike                 = 0x6E4790;
    constexpr uintptr_t Off_MoveToRogueLike                 = 0x6E3190;

    // --- Misc ---
    constexpr uintptr_t RVA_SetEntityActive                 = 0x6E4EA0;
    constexpr uintptr_t Off_SetEntityActive                 = 0x6E38A0;
    constexpr uintptr_t RVA_SetPlayerColliderActive         = 0x6E4FD0;
    constexpr uintptr_t Off_SetPlayerColliderActive         = 0x6E39D0;
    constexpr uintptr_t RVA_SetPlayerInputActive            = 0x6E5030;
    constexpr uintptr_t Off_SetPlayerInputActive            = 0x6E3A30;
    constexpr uintptr_t RVA_AddReimuPossitiveSpellToWorkScene = 0x6E1AB0;
    constexpr uintptr_t Off_AddReimuPossitiveSpellToWorkScene = 0x6E04B0;
    constexpr uintptr_t RVA_HakureiMoneyBoxDonate           = 0x6E4650;
    constexpr uintptr_t Off_HakureiMoneyBoxDonate           = 0x6E3050;
    constexpr uintptr_t RVA_ResetCollectableCoolDown        = 0x6E4BE0;
    constexpr uintptr_t Off_ResetCollectableCoolDown        = 0x6E35E0;
    constexpr uintptr_t RVA_RefreshCollectable              = 0x6E4A90;
    constexpr uintptr_t Off_RefreshCollectable              = 0x6E3490;
    constexpr uintptr_t RVA_RefreshNPC                      = 0x6E4AF0;
    constexpr uintptr_t Off_RefreshNPC                      = 0x6E34F0;
    constexpr uintptr_t RVA_InviteProfiteerTewi             = 0x6E46A0;
    constexpr uintptr_t Off_InviteProfiteerTewi             = 0x6E30A0;
    constexpr uintptr_t RVA_GotoKourindou                   = 0x6E3F40;
    constexpr uintptr_t Off_GotoKourindou                   = 0x6E2940;
    constexpr uintptr_t RVA_AddToKourindoStaticMerchandise  = 0x6E1AF0;
    constexpr uintptr_t Off_AddToKourindoStaticMerchandise  = 0x6E04F0;
    constexpr uintptr_t RVA_OpenCreatorsBoxPanel            = 0x6E47F0;
    constexpr uintptr_t Off_OpenCreatorsBoxPanel            = 0x6E31F0;

    // --- Staff Scenes ---
    constexpr uintptr_t RVA_GotoStaffSceneCore              = 0x6E4140;
    constexpr uintptr_t Off_GotoStaffSceneCore              = 0x6E2B40;
    constexpr uintptr_t RVA_GotoStaffSceneTrue              = 0x6E4640;
    constexpr uintptr_t Off_GotoStaffSceneTrue              = 0x6E3040;
    constexpr uintptr_t RVA_GotoStaffSceneDLC1              = 0x6E4150;
    constexpr uintptr_t Off_GotoStaffSceneDLC1              = 0x6E2B50;
    constexpr uintptr_t RVA_GotoStaffSceneDLC2              = 0x6E4160;
    constexpr uintptr_t Off_GotoStaffSceneDLC2              = 0x6E2B60;
    constexpr uintptr_t RVA_GotoStaffSceneDLC3              = 0x6E4170;
    constexpr uintptr_t Off_GotoStaffSceneDLC3              = 0x6E2B70;
    constexpr uintptr_t RVA_GotoStaffSceneDLC4              = 0x6E4180;
    constexpr uintptr_t Off_GotoStaffSceneDLC4              = 0x6E2B80;
    constexpr uintptr_t RVA_GotoStaffSceneDLC5              = 0x6E4190;
    constexpr uintptr_t Off_GotoStaffSceneDLC5              = 0x6E2B90;

    // --- Challenges ---
    constexpr uintptr_t RVA_StartHakugyokurouChallengeLv1   = 0x6E54A0;
    constexpr uintptr_t Off_StartHakugyokurouChallengeLv1   = 0x6E3EA0;
    constexpr uintptr_t RVA_StartHakugyokurouChallengeLv2   = 0x6E5500;
    constexpr uintptr_t Off_StartHakugyokurouChallengeLv2   = 0x6E3F00;
    constexpr uintptr_t RVA_StartHakugyokurouChallengeFinal = 0x6E5440;
    constexpr uintptr_t Off_StartHakugyokurouChallengeFinal = 0x6E3E40;
    constexpr uintptr_t RVA_StartWeirdCookingChallenge      = 0x6E62B0;
    constexpr uintptr_t Off_StartWeirdCookingChallenge      = 0x6E4CB0;
    constexpr uintptr_t RVA_StartHakureiFestivalMusicChallenge   = 0x6E5C20;
    constexpr uintptr_t Off_StartHakureiFestivalMusicChallenge   = 0x6E4620;
    constexpr uintptr_t RVA_StartHakureiFestivalCookingChallenge = 0x6E5560;
    constexpr uintptr_t Off_StartHakureiFestivalCookingChallenge = 0x6E3F60;

    // --- Infinite Selector Panels ---
    constexpr uintptr_t RVA_OpenInfiniteSelectorPanel1      = 0x6E4A00;
    constexpr uintptr_t Off_OpenInfiniteSelectorPanel1      = 0x6E3400;
    constexpr uintptr_t RVA_OpenInfiniteSelectorPanel2      = 0x6E4A30;
    constexpr uintptr_t Off_OpenInfiniteSelectorPanel2      = 0x6E3430;
    constexpr uintptr_t RVA_OpenInfiniteSelectorPanel3      = 0x6E4A60;
    constexpr uintptr_t Off_OpenInfiniteSelectorPanel3      = 0x6E3460;

} // namespace DaySceneDebugImpl

// =============================================================================
// GlobalDebugConsole (TypeDefIndex: 252) - EnableConsole property
// =============================================================================
namespace GlobalDebugConsole {

    constexpr uintptr_t RVA_get_EnableConsole               = 0x6E9BC0;
    constexpr uintptr_t Off_get_EnableConsole               = 0x6E85C0;
    constexpr uintptr_t RVA_set_EnableConsole               = 0x6E9C10;
    constexpr uintptr_t Off_set_EnableConsole               = 0x6E8610;

    constexpr uintptr_t RVA_Start                           = 0x6E9760;
    constexpr uintptr_t Off_Start                           = 0x6E8160;
    constexpr uintptr_t RVA_Update                          = 0x6E99A0;
    constexpr uintptr_t Off_Update                          = 0x6E83A0;
    constexpr uintptr_t RVA_OnGUI                           = 0x6E8300;
    constexpr uintptr_t Off_OnGUI                           = 0x6E6D00;
    constexpr uintptr_t RVA_ShowDebugConsole                = 0x6E8B20;
    constexpr uintptr_t Off_ShowDebugConsole                = 0x6E7520;

} // namespace GlobalDebugConsole

// =============================================================================
// GlobalDebugImpl (TypeDefIndex: 253) - "全局" (Global)
// =============================================================================
namespace GlobalDebugImpl {

    constexpr uintptr_t RVA_RegisterMethod                  = 0x6E9FF0;
    constexpr uintptr_t Off_RegisterMethod                  = 0x6E89F0;
    constexpr uintptr_t RVA_OutputArchiveAndLog             = 0x6E9DE0;
    constexpr uintptr_t Off_OutputArchiveAndLog             = 0x6E87E0;
    constexpr uintptr_t RVA_OpenLogDirectory                = 0x6E9D10;
    constexpr uintptr_t Off_OpenLogDirectory                = 0x6E8710;
    constexpr uintptr_t RVA_GotoEmptyScene                  = 0x6E9C70;
    constexpr uintptr_t Off_GotoEmptyScene                  = 0x6E8670;
    constexpr uintptr_t RVA_GotoMainScene                   = 0x6E9CC0;
    constexpr uintptr_t Off_GotoMainScene                   = 0x6E86C0;

} // namespace GlobalDebugImpl

// =============================================================================
// MainSceneDebugImpl (TypeDefIndex: 262) - "主菜单" (Main Menu)
// =============================================================================
namespace MainSceneDebugImpl {

    constexpr uintptr_t RVA_RegisterMethod                  = 0x6F1160;
    constexpr uintptr_t Off_RegisterMethod                  = 0x6EFB60;
    constexpr uintptr_t RVA_UnRegisterMethod                = 0x6F1270;
    constexpr uintptr_t Off_UnRegisterMethod                = 0x6EFC70;
    constexpr uintptr_t RVA_SetNewGameMode                  = 0x6F1200;
    constexpr uintptr_t Off_SetNewGameMode                  = 0x6EFC00;
    constexpr uintptr_t RVA_SetGameLanguage                 = 0x6F11F0;
    constexpr uintptr_t Off_SetGameLanguage                 = 0x6EFBF0;
    constexpr uintptr_t RVA_GotoTestWorkScene               = 0x6F0B40;
    constexpr uintptr_t Off_GotoTestWorkScene               = 0x6EF540;
    constexpr uintptr_t RVA_GotoTestResultScene             = 0x6F0970;
    constexpr uintptr_t Off_GotoTestResultScene             = 0x6EF370;
    constexpr uintptr_t RVA_GotoStaffScene                  = 0x6F0720;
    constexpr uintptr_t Off_GotoStaffScene                  = 0x6EF120;

} // namespace MainSceneDebugImpl

// =============================================================================
// NightSceneDebugImpl (TypeDefIndex: 264) - "夜间" (Nighttime)
// =============================================================================
namespace NightSceneDebugImpl {

    // --- Core Registration ---
    constexpr uintptr_t RVA_RegisterMethod                  = 0x6F2D90;
    constexpr uintptr_t Off_RegisterMethod                  = 0x6F1790;
    constexpr uintptr_t RVA_UnRegisterMethod                = 0x6F43B0;
    constexpr uintptr_t Off_UnRegisterMethod                = 0x6F2DB0;

    // --- Income / Combo ---
    constexpr uintptr_t RVA_SetWorkIncome                   = 0x6F3650;
    constexpr uintptr_t Off_SetWorkIncome                   = 0x6F2050;
    constexpr uintptr_t RVA_SetWorkCombo                    = 0x6F3590;
    constexpr uintptr_t Off_SetWorkCombo                    = 0x6F1F90;
    constexpr uintptr_t RVA_SetWorkExp                      = 0x6F35F0;
    constexpr uintptr_t Off_SetWorkExp                      = 0x6F1FF0;
    constexpr uintptr_t RVA_SetComboValue                   = 0x6F3280;
    constexpr uintptr_t Off_SetComboValue                   = 0x6F1C80;
    constexpr uintptr_t RVA_ResetCombo                      = 0x6F31C0;
    constexpr uintptr_t Off_ResetCombo                      = 0x6F1BC0;
    constexpr uintptr_t RVA_AddWorkTip                      = 0x6F14D0;
    constexpr uintptr_t Off_AddWorkTip                      = 0x6EFED0;

    // --- Time ---
    constexpr uintptr_t RVA_ModifyWorkTime                  = 0x6F2A90;
    constexpr uintptr_t Off_ModifyWorkTime                  = 0x6F1490;

    // --- Supplementary (Restock) ---
    constexpr uintptr_t RVA_SupplementaryBeverages          = 0x6F3B50;
    constexpr uintptr_t Off_SupplementaryBeverages          = 0x6F2550;
    constexpr uintptr_t RVA_SupplementaryIngredients        = 0x6F3F10;
    constexpr uintptr_t Off_SupplementaryIngredients        = 0x6F2910;
    constexpr uintptr_t RVA_SupplementaryFoods              = 0x6F3D30;
    constexpr uintptr_t Off_SupplementaryFoods              = 0x6F2730;

    // --- QTE Buffs ---
    constexpr uintptr_t RVA_TriggerRandomQTEBuff            = 0x6F4300;
    constexpr uintptr_t Off_TriggerRandomQTEBuff            = 0x6F2D00;
    constexpr uintptr_t RVA_TriggerThrowServingQTEBuff      = 0x6F4360;
    constexpr uintptr_t Off_TriggerThrowServingQTEBuff      = 0x6F2D60;
    constexpr uintptr_t RVA_TriggerPatientQTEBuff           = 0x6F4230;
    constexpr uintptr_t Off_TriggerPatientQTEBuff           = 0x6F2C30;
    constexpr uintptr_t RVA_TriggerFinishEatingImmediatelyQTEBuff = 0x6F4160;
    constexpr uintptr_t Off_TriggerFinishEatingImmediatelyQTEBuff = 0x6F2B60;

    // --- Cheat Buffs ---
    constexpr uintptr_t RVA_RegisteredCheatedBuff           = 0x6F2E20;
    constexpr uintptr_t Off_RegisteredCheatedBuff           = 0x6F1820;
    constexpr uintptr_t RVA_RegisteredEternityFever         = 0x6F2EB0;
    constexpr uintptr_t Off_RegisteredEternityFever         = 0x6F18B0;
    constexpr uintptr_t RVA_RemoveAllRegisteredTimedBuff    = 0x6F2F60;
    constexpr uintptr_t Off_RemoveAllRegisteredTimedBuff    = 0x6F1960;

    // --- Cooking Buffs ---
    constexpr uintptr_t RVA_SetFreeCook                     = 0x6F3350;
    constexpr uintptr_t Off_SetFreeCook                     = 0x6F1D50;
    constexpr uintptr_t RVA_SetFreeCookRate                 = 0x6F32E0;
    constexpr uintptr_t Off_SetFreeCookRate                 = 0x6F1CE0;

    // --- Guest Spawning ---
    constexpr uintptr_t RVA_SpawnNormalGuest                = 0x6F3780;
    constexpr uintptr_t Off_SpawnNormalGuest                = 0x6F2180;
    constexpr uintptr_t RVA_SpawnNormalGuest_ById           = 0x6F36B0;
    constexpr uintptr_t Off_SpawnNormalGuest_ById           = 0x6F20B0;
    constexpr uintptr_t RVA_SpawnSpecialGuest               = 0x6F38F0;
    constexpr uintptr_t Off_SpawnSpecialGuest               = 0x6F22F0;
    constexpr uintptr_t RVA_SpawnSpecialGuest_ById          = 0x6F39C0;
    constexpr uintptr_t Off_SpawnSpecialGuest_ById          = 0x6F23C0;

    // --- Guest Management ---
    constexpr uintptr_t RVA_RepellGuestAndPay               = 0x6F3090;
    constexpr uintptr_t Off_RepellGuestAndPay               = 0x6F1A90;
    constexpr uintptr_t RVA_RepellGuestAndPay_ByDesk        = 0x6F3110;
    constexpr uintptr_t Off_RepellGuestAndPay_ByDesk        = 0x6F1B10;
    constexpr uintptr_t RVA_RepellGuestAndPayByMoodValue    = 0x6F3010;
    constexpr uintptr_t Off_RepellGuestAndPayByMoodValue    = 0x6F1A10;
    constexpr uintptr_t RVA_TriggerPositiveBuff             = 0x6F4280;
    constexpr uintptr_t Off_TriggerPositiveBuff             = 0x6F2C80;
    constexpr uintptr_t RVA_TriggerNegativeBuff             = 0x6F41B0;
    constexpr uintptr_t Off_TriggerNegativeBuff             = 0x6F2BB0;
    constexpr uintptr_t RVA_PatientDeplete                  = 0x6F2C40;
    constexpr uintptr_t Off_PatientDeplete                  = 0x6F1640;
    constexpr uintptr_t RVA_ClearSpecialGuestAlbum          = 0x6F1650;
    constexpr uintptr_t Off_ClearSpecialGuestAlbum          = 0x6F0050;
    constexpr uintptr_t RVA_UnlockInfo                      = 0x6F44B0;
    constexpr uintptr_t Off_UnlockInfo                      = 0x6F2EB0;

    // --- Passion (Atmosphere) ---
    constexpr uintptr_t RVA_SetPassionLevelOverrideActive   = 0x6F34A0;
    constexpr uintptr_t Off_SetPassionLevelOverrideActive   = 0x6F1EA0;
    constexpr uintptr_t RVA_IncreasePassionLevel            = 0x6F2660;
    constexpr uintptr_t Off_IncreasePassionLevel            = 0x6F1060;
    constexpr uintptr_t RVA_DecreasePassionLevel            = 0x6F1830;
    constexpr uintptr_t Off_DecreasePassionLevel            = 0x6F0230;
    constexpr uintptr_t RVA_SetPassionValue                 = 0x6F3530;
    constexpr uintptr_t Off_SetPassionValue                 = 0x6F1F30;
    constexpr uintptr_t RVA_AddPassionPerTime               = 0x6F1430;
    constexpr uintptr_t Off_AddPassionPerTime               = 0x6EFE30;

    // --- Misc Buffs ---
    constexpr uintptr_t RVA_Stun                            = 0x6F3AE0;
    constexpr uintptr_t Off_Stun                            = 0x6F24E0;
    constexpr uintptr_t RVA_ChangeMoveSpeedByBuff           = 0x6F1550;
    constexpr uintptr_t Off_ChangeMoveSpeedByBuff           = 0x6EFF50;
    constexpr uintptr_t RVA_LockDailyRecipes                = 0x6F26E0;
    constexpr uintptr_t Off_LockDailyRecipes                = 0x6F10E0;
    constexpr uintptr_t RVA_CookTimeAndOrderRateEditByTag   = 0x6F1770;
    constexpr uintptr_t Off_CookTimeAndOrderRateEditByTag   = 0x6F0170;
    constexpr uintptr_t RVA_CookTimeAndOrderRateEditByCookerType = 0x6F1690;
    constexpr uintptr_t Off_CookTimeAndOrderRateEditByCookerType = 0x6F0090;
    constexpr uintptr_t RVA_ForcedOverrideFoodProbably      = 0x6F1AB0;
    constexpr uintptr_t Off_ForcedOverrideFoodProbably      = 0x6F04B0;
    constexpr uintptr_t RVA_SetPartnerExtraWage             = 0x6F3440;
    constexpr uintptr_t Off_SetPartnerExtraWage             = 0x6F1E40;
    constexpr uintptr_t RVA_RemovePartnerExtraWage          = 0x6F2FB0;
    constexpr uintptr_t Off_RemovePartnerExtraWage          = 0x6F19B0;
    constexpr uintptr_t RVA_SetGuestNotLeaveWhenIzakayaClose = 0x6F33C0;
    constexpr uintptr_t Off_SetGuestNotLeaveWhenIzakayaClose = 0x6F1DC0;
    constexpr uintptr_t RVA_GuestMoodExtraAddEdit           = 0x6F24B0;
    constexpr uintptr_t Off_GuestMoodExtraAddEdit           = 0x6F0EB0;
    constexpr uintptr_t RVA_OrderCycleRateEdit              = 0x6F2B50;
    constexpr uintptr_t Off_OrderCycleRateEdit              = 0x6F1550;
    constexpr uintptr_t RVA_EatTimeEdit                     = 0x6F18B0;
    constexpr uintptr_t Off_EatTimeEdit                     = 0x6F02B0;
    constexpr uintptr_t RVA_GuestPatientEdit_Youmu          = 0x6F2540;
    constexpr uintptr_t Off_GuestPatientEdit_Youmu          = 0x6F0F40;
    constexpr uintptr_t RVA_SpawnRateEdit                   = 0x6F3860;
    constexpr uintptr_t Off_SpawnRateEdit                   = 0x6F2260;

} // namespace NightSceneDebugImpl

// =============================================================================
// Steam Achievement (Steamworks) - Direct Steam API calls
// =============================================================================
namespace SteamUserStats {
    constexpr uintptr_t RVA_GetAchievement    = 0x9CD5F0; // bool GetAchievement(string, out bool)
    constexpr uintptr_t RVA_SetAchievement    = 0x9CDA00; // bool SetAchievement(string)
    constexpr uintptr_t RVA_StoreStats        = 0x9CDE00; // bool StoreStats()
    constexpr uintptr_t RVA_GetStat           = 0x9CD800; // bool GetStat(string, out int)
    constexpr uintptr_t RVA_SetStat           = 0x9CDC00; // bool SetStat(string, int)
} // namespace SteamUserStats

// =============================================================================
// DataBaseAchievement - Game achievement system
// =============================================================================
namespace DataBaseAchievement {
    constexpr uintptr_t RVA_CheckAllAchievement       = 0x65B250;
    constexpr uintptr_t RVA_FinishAchievement          = 0x65B3F0; // void(string achievementId)
    constexpr uintptr_t RVA_FinishAndCheckAchievement  = 0x65B4B0; // void(string achievementId)
    constexpr uintptr_t RVA_HasAchievementFinished     = 0x65B780; // bool(string achievementId)
    constexpr uintptr_t RVA_Initialize                 = 0x65B800;
    constexpr uintptr_t RVA_OnEnterGame                = 0x65C7D0;
} // namespace DataBaseAchievement

// =============================================================================
// Unity SceneManagement - Scene detection
// =============================================================================
namespace UnitySceneManager {
    constexpr uintptr_t RVA_GetActiveScene  = 0x2CFE560; // static Scene GetActiveScene()
    constexpr uintptr_t RVA_Scene_get_name  = 0x2CF5E70; // string Scene.get_name()
} // namespace UnitySceneManager

// =============================================================================
// UniversalGameManager - Scene loading control
// =============================================================================
namespace UniversalGameManager {
    // IL2CPP has TWO separate TypeInfo pointers for this class hierarchy:
    //
    // 1) MonoSingleton<UGM> TypeInfo (generic base) — holds the singleton Instance
    //    *(base + RVA_SingletonTypeInfo) -> TypeInfo
    //    *(TypeInfo + 184) -> staticFields
    //    *(staticFields + 0) -> Instance pointer
    //
    // 2) Concrete UGM TypeInfo (derived class) — holds isLoadingScene flag
    //    *(base + RVA_ConcreteTypeInfo) -> TypeInfo
    //    *(TypeInfo + 184) -> staticFields
    //    *(staticFields + 129) -> isLoadingScene (bool)
    //
    constexpr uintptr_t RVA_SingletonTypeInfo  = 0x4258E80;  // MonoSingleton<UGM> — Instance at sf+0
    constexpr uintptr_t RVA_ConcreteTypeInfo   = 0x422F890;  // UGM concrete — isLoadingScene at sf+129
    constexpr uintptr_t TypeInfo_StaticFields   = 184;        // offset to static fields ptr in TypeInfo
    constexpr uintptr_t Static_Instance         = 0;          // in Singleton's static fields
    constexpr uintptr_t Static_IsLoadingScene   = 129;        // in Concrete's static fields
    // Instance field offsets
    constexpr uintptr_t Instance_SceneDataArray = 0x118;      // scene data array
    // LoadScene RVA
    constexpr uintptr_t RVA_LoadScene           = 0x814170;
} // namespace UniversalGameManager

// =============================================================================
// EventManager - Night scene event system (MonoSingleton)
// TypeDefIndex: 1653, namespace: NightScene.EventUtility
// CheatedBuff etc. depend on this singleton existing
// =============================================================================
namespace EventManager {
    // NOTE: TypeInfo RVA not yet verified via IDA
    // Night debug functions internally call MonoSingleton<EventManager>.get_Instance()
    // If the singleton is null (wrong scene), they throw NullReferenceException
} // namespace EventManager

} // namespace Offsets
