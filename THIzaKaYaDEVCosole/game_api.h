#pragma once
// ============================================================
// Game Function Wrappers - Direct RVA calls to IL2CPP methods
// All debug methods are static, IL2CPP signature: RetType Func(args..., MethodInfo*)
// ============================================================

#include "il2cpp_types.h"
#include "game_offsets.h"
#include <string>
#include <cstdio>

namespace Game {

    // --- Helper: call a void() static method ---
    inline void CallVoid(uintptr_t rva) {
        using Fn = void(*)(const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(nullptr);
    }

    // --- Helper: call a void(int) static method ---
    inline void CallVoidInt(uintptr_t rva, int value) {
        using Fn = void(*)(int, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(value, nullptr);
    }

    // --- Helper: call a void(int, int) static method ---
    inline void CallVoidIntInt(uintptr_t rva, int a, int b) {
        using Fn = void(*)(int, int, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(a, b, nullptr);
    }

    // --- Helper: call a void(bool) static method ---
    inline void CallVoidBool(uintptr_t rva, bool value) {
        using Fn = void(*)(bool, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(value, nullptr);
    }

    // --- Helper: call a void(float, int) static method ---
    inline void CallVoidFloatInt(uintptr_t rva, float f, int i) {
        using Fn = void(*)(float, int, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(f, i, nullptr);
    }

    // --- Helper: call a void(Il2CppString*) static method ---
    inline void CallVoidString(uintptr_t rva, const char* str) {
        using Fn = void(*)(Il2CppString*, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(IL2CPP::string_new(str), nullptr);
    }

    // --- Helper: get static bool ---
    inline bool GetBool(uintptr_t rva) {
        using Fn = bool(*)(const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        return fn(nullptr);
    }

    // --- Helper: set static bool ---
    inline void SetBool(uintptr_t rva, bool value) {
        using Fn = void(*)(bool, const MethodInfo*);
        auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
        fn(value, nullptr);
    }

    // ============================================================
    // DAY SCENE (白天)
    // ============================================================
    namespace Day {
        using namespace Offsets::DaySceneDebugImpl;
        // --- 注册/注销调试方法 (必须先调用) ---
        inline void RegisterMethod()              { CallVoid(RVA_RegisterMethod); }
        inline void UnRegisterMethod()            { CallVoid(RVA_UnRegisterMethod); }
        // --- 金钱/资源 ---
        inline void GetMoney(int amount)          { CallVoidInt(RVA_GetMoney, amount); }
        inline void GetMaxResources()             { CallVoid(RVA_GetMaxResources); }
        inline void GetTestResources()            { CallVoid(RVA_GetTestResources); }
        inline void GetRandomResources()          { CallVoid(RVA_GetRandomResources); }
        inline void GetSilverFrogCoin(int count)  { CallVoidInt(RVA_GetSilverFrogCoin, count); }
        inline void AddExp(int exp)               { CallVoidInt(RVA_AddExp, exp); }
        // --- 收集品/解锁 ---
        inline void GetAllClothes()               { CallVoid(RVA_GetAllClothes); }
        inline void GetAllRecords()               { CallVoid(RVA_GetAllRecords); }
        inline void GetAllBadges()                { CallVoid(RVA_GetAllBadges); }
        inline void GetAndUseAllDecorations()     { CallVoid(RVA_GetAndUseAllDecorations); }
        inline void GetAllPartners()              { CallVoid(RVA_GetAllPartners); }
        inline void GetAllFishingTrophy()         { CallVoid(RVA_GetAllFishingTrophy); }
        inline void UnlockSpecialGuestsAndMaps()  { CallVoid(RVA_UnlockSpecialGuestsAndMaps); }
        inline void UnlockAllMusicChapters()      { CallVoid(RVA_UnlockAllMusicChapters); }
        // --- 羁绊 ---
        inline void FullUpgradeAllKizunaLevel()   { CallVoid(RVA_FullUpgradeAllKizunaLevel); }
        // --- 时间 ---
        inline void NextDay()                     { CallVoid(RVA_NextDay); }
        inline void SetDays(int day)              { CallVoidInt(RVA_SetDays, day); }
        inline void AddDays(int day)              { CallVoidInt(RVA_AddDays, day); }
        inline void WarpHours(int hour)           { CallVoidInt(RVA_WarpHours, hour); }
        inline void WarpActions(int count)        { CallVoidInt(RVA_WarpActions, count); }
        // --- 清空 ---
        inline void ClearAllIngredients()         { CallVoid(RVA_ClearAllIngredients); }
        inline void ClearAllBeverages()           { CallVoid(RVA_ClearAllBeverages); }
        inline void ClearAllItems()               { CallVoid(RVA_ClearAllItems); }
        // --- 任务/事件 ---
        inline void StartAllMissions()            { CallVoid(RVA_StartAllMissions); }
        inline void FinishAllEventsAndMissions()  { CallVoid(RVA_FinishAllEventsAndMissions); }
        // --- 伙伴 ---
        inline void SetMultiPartnerModeActive(bool v) { CallVoidBool(RVA_SetMultiPartnerModeActive, v); }
        inline void SetMaxPartnerCount(int count) { CallVoidInt(RVA_SetMaxPartnerCount, count); }
        // --- 传送 ---
        inline void Transmit(const char* mapLabel){ CallVoidString(RVA_Transmit, mapLabel); }
        // --- 杂项 ---
        inline void MoveToRogueLike()             { CallVoid(RVA_MoveToRogueLike); }
        inline void HakureiMoneyBoxDonate(int m)  { CallVoidInt(RVA_HakureiMoneyBoxDonate, m); }
        inline void ResetCollectableCoolDown()    { CallVoid(RVA_ResetCollectableCoolDown); }
        inline void RefreshCollectable()          { CallVoid(RVA_RefreshCollectable); }
        inline void RefreshNPC()                  { CallVoid(RVA_RefreshNPC); }
        inline void InviteProfiteerTewi()         { CallVoid(RVA_InviteProfiteerTewi); }
        inline void GotoKourindou()               { CallVoid(RVA_GotoKourindou); }
        inline void AddToKourindoStaticMerchandise() { CallVoid(RVA_AddToKourindoStaticMerchandise); }
        inline void SetPlayerColliderActive(bool v)  { CallVoidBool(RVA_SetPlayerColliderActive, v); }
        inline void SetPlayerInputActive(bool v)     { CallVoidBool(RVA_SetPlayerInputActive, v); }
        inline void AddReimuSpell()               { CallVoid(RVA_AddReimuPossitiveSpellToWorkScene); }
    }

    // ============================================================
    // NIGHT SCENE (夜间营业)
    // ============================================================
    namespace Night {
        using namespace Offsets::NightSceneDebugImpl;
        // --- 注册/注销调试方法 (必须先调用) ---
        inline void RegisterMethod()              { CallVoid(RVA_RegisterMethod); }
        inline void UnRegisterMethod()            { CallVoid(RVA_UnRegisterMethod); }
        // --- 作弊Buff ---
        inline void RegisteredCheatedBuff()       { CallVoid(RVA_RegisteredCheatedBuff); }
        inline void RegisteredEternityFever()     { CallVoid(RVA_RegisteredEternityFever); }
        inline void RemoveAllTimedBuff()          { CallVoid(RVA_RemoveAllRegisteredTimedBuff); }
        inline void SetFreeCook(int seconds)      { CallVoidInt(RVA_SetFreeCook, seconds); }
        // --- 收入/连击 ---
        inline void SetWorkIncome(int val, int op){ CallVoidIntInt(RVA_SetWorkIncome, val, op); }
        inline void SetWorkCombo(int val, int op) { CallVoidIntInt(RVA_SetWorkCombo, val, op); }
        inline void ModifyWorkTime(int time)      { CallVoidInt(RVA_ModifyWorkTime, time); }
        // --- 补充 ---
        inline void SupplementaryBeverages()      { CallVoid(RVA_SupplementaryBeverages); }
        inline void SupplementaryIngredients()    { CallVoid(RVA_SupplementaryIngredients); }
        inline void SupplementaryFoods()          { CallVoid(RVA_SupplementaryFoods); }
        // --- 客人 ---
        inline void SpawnNormalGuest()            { CallVoid(RVA_SpawnNormalGuest); }
        inline void SpawnSpecialGuest()           { CallVoid(RVA_SpawnSpecialGuest); }
        inline void RepellGuestAndPay()           { CallVoid(RVA_RepellGuestAndPay); }
        // --- QTE Buff ---
        inline void TriggerRandomQTEBuff()        {
            using Fn = void(*)(bool, const MethodInfo*);
            reinterpret_cast<Fn>(IL2CPP::RVA(RVA_TriggerRandomQTEBuff))(true, nullptr);
        }
        inline void TriggerThrowServingQTE()      { CallVoid(RVA_TriggerThrowServingQTEBuff); }
        inline void TriggerPatientQTE()           { CallVoid(RVA_TriggerPatientQTEBuff); }
        inline void TriggerFinishEatingQTE()      { CallVoid(RVA_TriggerFinishEatingImmediatelyQTEBuff); }
        // --- 气氛 ---
        inline void IncreasePassionLevel()        { CallVoid(RVA_IncreasePassionLevel); }
        inline void DecreasePassionLevel()        { CallVoid(RVA_DecreasePassionLevel); }
    }

    // ============================================================
    // GLOBAL
    // ============================================================
    namespace Global {
        using namespace Offsets::GlobalDebugConsole;
        using namespace Offsets::GlobalDebugImpl;
        inline bool  GetEnableConsole()           { return GetBool(RVA_get_EnableConsole); }
        inline void  SetEnableConsole(bool v)     { SetBool(RVA_set_EnableConsole, v); }
        inline void  GotoMainScene()              { CallVoid(RVA_GotoMainScene); }
        inline void  GotoEmptyScene()             { CallVoid(RVA_GotoEmptyScene); }
        inline void  OutputArchiveAndLog()        { CallVoid(RVA_OutputArchiveAndLog); }
        inline void  OpenLogDirectory()           { CallVoid(RVA_OpenLogDirectory); }
    }

    // ============================================================
    // SCENE LOADING (场景加载控制 + 诊断)
    // ============================================================
    namespace SceneLoad {
        using namespace Offsets::UniversalGameManager;

        // 读取 TypeInfo -> staticFields (通用辅助)
        inline uintptr_t ReadStaticFields(uintptr_t typeInfoRVA) {
            auto typeInfoGlobal = IL2CPP::RVA(typeInfoRVA);
            auto typeInfo = *(uintptr_t*)typeInfoGlobal;
            if (!typeInfo) return 0;
            return *(uintptr_t*)(typeInfo + TypeInfo_StaticFields);
        }

        // MonoSingleton<UGM> 的 static fields (包含 Instance)
        inline uintptr_t GetSingletonStaticFields() {
            return ReadStaticFields(RVA_SingletonTypeInfo);
        }

        // UGM concrete 的 static fields (包含 isLoadingScene)
        inline uintptr_t GetConcreteStaticFields() {
            return ReadStaticFields(RVA_ConcreteTypeInfo);
        }

        // 获取 isLoadingScene 标志位的内存地址 (从 concrete TypeInfo)
        inline uint8_t* GetLoadingFlagPtr() {
            auto sf = GetConcreteStaticFields();
            if (!sf) return nullptr;
            return (uint8_t*)(sf + Static_IsLoadingScene);
        }

        // 重置标志位
        inline bool ResetLoadingFlag() {
            auto* flag = GetLoadingFlagPtr();
            if (flag) { *flag = 0; return true; }
            return false;
        }

        // 检查是否正在加载
        inline bool IsLoading() {
            auto* flag = GetLoadingFlagPtr();
            return flag ? (*flag != 0) : false;
        }

        // 诊断: 检查 LoadScene 所需的全部前置条件
        // SEH-safe helpers (no C++ objects)
        inline bool TryReadPtr(uintptr_t addr, uintptr_t* out) {
            __try {
                *out = *(uintptr_t*)addr;
                return true;
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }
        inline bool TryReadByte(uintptr_t addr, uint8_t* out) {
            __try {
                *out = *(uint8_t*)addr;
                return true;
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                return false;
            }
        }

        // 返回诊断结果代码和详细信息
        // code: 0=OK, 1=TypeInfo NULL, 2=StaticFields NULL, 3=Instance read fail,
        //        4=Instance NULL, 5=SceneArray read fail, 6=SceneArray NULL,
        //        7=ArrayLen read fail, 8=Index out of bounds
        struct DiagResult {
            int code;
            uintptr_t instance;
            uintptr_t singletonSF;   // MonoSingleton static fields
            uintptr_t concreteSF;    // UGM concrete static fields
            uintptr_t arrayLen;
            uint8_t loadingFlag;
        };

        inline DiagResult DiagnoseRaw(int sceneIndex) {
            DiagResult r = {};

            // Step 1: Read Instance from MonoSingleton<UGM> TypeInfo
            auto singletonGlobal = IL2CPP::RVA(RVA_SingletonTypeInfo);
            uintptr_t singletonTI = 0;
            if (!TryReadPtr(singletonGlobal, &singletonTI) || !singletonTI) { r.code = 1; return r; }
            if (!TryReadPtr(singletonTI + TypeInfo_StaticFields, &r.singletonSF) || !r.singletonSF) { r.code = 2; return r; }
            if (!TryReadPtr(r.singletonSF + Static_Instance, &r.instance)) { r.code = 3; return r; }
            if (!r.instance) { r.code = 4; return r; }

            // Step 2: Read isLoadingScene from concrete UGM TypeInfo
            auto concreteGlobal = IL2CPP::RVA(RVA_ConcreteTypeInfo);
            uintptr_t concreteTI = 0;
            if (TryReadPtr(concreteGlobal, &concreteTI) && concreteTI) {
                if (TryReadPtr(concreteTI + TypeInfo_StaticFields, &r.concreteSF) && r.concreteSF) {
                    TryReadByte(r.concreteSF + Static_IsLoadingScene, &r.loadingFlag);
                }
            }

            // Step 3: Check scene data array
            uintptr_t sceneArray = 0;
            if (!TryReadPtr(r.instance + Instance_SceneDataArray, &sceneArray)) { r.code = 5; return r; }
            if (!sceneArray) { r.code = 6; return r; }

            if (!TryReadPtr(sceneArray + 24, &r.arrayLen)) { r.code = 7; return r; }
            if ((uintptr_t)sceneIndex >= r.arrayLen) { r.code = 8; return r; }

            r.code = 0;
            return r;
        }

        // 返回人类可读的诊断字符串
        inline std::string Diagnose(int sceneIndex) {
            auto r = DiagnoseRaw(sceneIndex);
            char buf[512];
            switch (r.code) {
            case 0:
                sprintf_s(buf, "OK: Instance=0x%llX, ArrayLen=%llu, isLoading=%d",
                    (unsigned long long)r.instance, (unsigned long long)r.arrayLen, (int)r.loadingFlag);
                return buf;
            case 1: return "Singleton TypeInfo is NULL";
            case 2: return "Singleton StaticFields is NULL";
            case 3: return "Cannot read Instance pointer";
            case 4:
                sprintf_s(buf, "Instance is NULL (singletonSF=0x%llX, concreteSF=0x%llX, isLoading=%d)",
                    (unsigned long long)r.singletonSF, (unsigned long long)r.concreteSF, (int)r.loadingFlag);
                return buf;
            case 5:
                sprintf_s(buf, "Cannot read sceneArray (Inst=0x%llX)", (unsigned long long)r.instance);
                return buf;
            case 6:
                sprintf_s(buf, "SceneArray is NULL (Inst=0x%llX)", (unsigned long long)r.instance);
                return buf;
            case 7: return "Cannot read sceneArray length";
            case 8:
                sprintf_s(buf, "SceneIndex %d >= ArrayLen %llu (OUT OF BOUNDS)",
                    sceneIndex, (unsigned long long)r.arrayLen);
                return buf;
            default: return "Unknown error";
            }
        }
    }

    // ============================================================
    // MAIN MENU (主菜单)
    // ============================================================
    namespace Main {
        using namespace Offsets::MainSceneDebugImpl;
        inline void GotoTestWorkScene()           { /* needs many params, skip for now */ }
        inline void GotoTestResultScene()         { CallVoid(RVA_GotoTestResultScene); }
    }

    // ============================================================
    // STEAM ACHIEVEMENTS (Steam成就)
    // ============================================================
    namespace Achievement {
        using namespace Offsets::SteamUserStats;

        // Unlock a Steam achievement by name
        inline bool SetAchievement(const char* name) {
            using Fn = bool(*)(Il2CppString*, const MethodInfo*);
            auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(RVA_SetAchievement));
            return fn(IL2CPP::string_new(name), nullptr);
        }

        // Check if achievement is unlocked
        inline bool GetAchievement(const char* name, bool& achieved) {
            using Fn = bool(*)(Il2CppString*, bool*, const MethodInfo*);
            auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(RVA_GetAchievement));
            return fn(IL2CPP::string_new(name), &achieved, nullptr);
        }

        // Commit changes to Steam
        inline bool StoreStats() {
            using Fn = bool(*)(const MethodInfo*);
            auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(RVA_StoreStats));
            return fn(nullptr);
        }

        // Set stat value
        inline bool SetStat(const char* name, int value) {
            using Fn = bool(*)(Il2CppString*, int, const MethodInfo*);
            auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(RVA_SetStat));
            return fn(IL2CPP::string_new(name), value, nullptr);
        }

        // Get stat value
        inline bool GetStat(const char* name, int& value) {
            using Fn = bool(*)(Il2CppString*, int*, const MethodInfo*);
            auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(RVA_GetStat));
            return fn(IL2CPP::string_new(name), &value, nullptr);
        }

        // Game-level achievement check
        inline void CheckAllAchievement() {
            using namespace Offsets::DataBaseAchievement;
            CallVoid(RVA_CheckAllAchievement);
        }
    }

    // ============================================================
    // SCENE DETECTION (场景检测)
    // ============================================================
    enum class SceneType { Unknown, Day, Night, Main, Other };

    namespace Scene {
        using namespace Offsets::UnitySceneManager;

        // SEH wrapper - no C++ objects allowed in this function
        inline Il2CppString* GetCurrentSceneNameRaw() {
            using GetActiveScene_t = int(*)(const MethodInfo*);
            using SceneGetName_t = Il2CppString*(*)(int*, const MethodInfo*);

            auto getScene = reinterpret_cast<GetActiveScene_t>(IL2CPP::RVA(RVA_GetActiveScene));
            auto getName  = reinterpret_cast<SceneGetName_t>(IL2CPP::RVA(RVA_Scene_get_name));

            __try {
                int handle = getScene(nullptr);
                Il2CppString* nameStr = getName(&handle, nullptr);
                if (nameStr && nameStr->length > 0)
                    return nameStr;
            } __except(EXCEPTION_EXECUTE_HANDLER) {}
            return nullptr;
        }

        // Get current Unity scene name
        inline std::string GetCurrentSceneName() {
            Il2CppString* nameStr = GetCurrentSceneNameRaw();
            if (nameStr) {
                std::string result;
                for (int i = 0; i < nameStr->length && i < 128; i++)
                    result += (char)nameStr->chars[i];
                return result;
            }
            return "";
        }

        // Detect scene type from name
        inline SceneType DetectScene() {
            std::string name = GetCurrentSceneName();
            if (name.empty()) return SceneType::Unknown;
            // Match known scene names (case-insensitive substring)
            if (name.find("Day") != std::string::npos || name.find("day") != std::string::npos)
                return SceneType::Day;
            if (name.find("Work") != std::string::npos || name.find("work") != std::string::npos ||
                name.find("Night") != std::string::npos || name.find("night") != std::string::npos)
                return SceneType::Night;
            if (name.find("Main") != std::string::npos || name.find("main") != std::string::npos)
                return SceneType::Main;
            return SceneType::Other;
        }
    }

    inline bool initialized = false;

    // Dump first bytes at an RVA for validation
    inline void DumpRVA(FILE* f, const char* name, uintptr_t rva) {
        auto addr = IL2CPP::RVA(rva);
        auto* p = reinterpret_cast<const unsigned char*>(addr);
        fprintf(f, "  %-40s RVA=0x%06llX VA=0x%llX bytes=[",
            name, (unsigned long long)rva, (unsigned long long)addr);
        __try {
            for (int i = 0; i < 8; i++)
                fprintf(f, "%02X ", p[i]);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            fprintf(f, "UNREADABLE ");
        }
        fprintf(f, "]\n");
    }

    inline void DumpAllRVAs(const char* logPath) {
        FILE* f = fopen(logPath, "a");
        if (!f) return;
        fprintf(f, "\n=== RVA Validation Dump ===\n");
        fprintf(f, "GameAssembly base: 0x%llX\n\n", (unsigned long long)IL2CPP::baseAddress);

        fprintf(f, "--- DaySceneDebugImpl ---\n");
        using namespace Offsets::DaySceneDebugImpl;
        DumpRVA(f, "GetMoney",                  RVA_GetMoney);
        DumpRVA(f, "GetMaxResources",           RVA_GetMaxResources);
        DumpRVA(f, "GetRandomResources",        RVA_GetRandomResources);
        DumpRVA(f, "GetSilverFrogCoin",         RVA_GetSilverFrogCoin);
        DumpRVA(f, "GetAllClothes",             RVA_GetAllClothes);
        DumpRVA(f, "GetAllRecords",             RVA_GetAllRecords);
        DumpRVA(f, "GetAllBadges",              RVA_GetAllBadges);
        DumpRVA(f, "GetAndUseAllDecorations",   RVA_GetAndUseAllDecorations);
        DumpRVA(f, "GetAllPartners",            RVA_GetAllPartners);
        DumpRVA(f, "GetAllFishingTrophy",       RVA_GetAllFishingTrophy);
        DumpRVA(f, "UnlockSpecialGuestsAndMaps",RVA_UnlockSpecialGuestsAndMaps);
        DumpRVA(f, "UnlockAllMusicChapters",    RVA_UnlockAllMusicChapters);
        DumpRVA(f, "FullUpgradeAllKizunaLevel", RVA_FullUpgradeAllKizunaLevel);
        DumpRVA(f, "NextDay",                   RVA_NextDay);
        DumpRVA(f, "ClearAllIngredients",       RVA_ClearAllIngredients);
        DumpRVA(f, "ClearAllItems",             RVA_ClearAllItems);
        DumpRVA(f, "StartAllMissions",          RVA_StartAllMissions);
        DumpRVA(f, "FinishAllEventsAndMissions", RVA_FinishAllEventsAndMissions);
        DumpRVA(f, "RefreshCollectable",        RVA_RefreshCollectable);
        DumpRVA(f, "RefreshNPC",                RVA_RefreshNPC);
        DumpRVA(f, "AddReimuSpell",             RVA_AddReimuPossitiveSpellToWorkScene);

        fprintf(f, "\n--- NightSceneDebugImpl ---\n");
        using namespace Offsets::NightSceneDebugImpl;
        DumpRVA(f, "SetWorkIncome",             RVA_SetWorkIncome);
        DumpRVA(f, "SupplementaryBeverages",    RVA_SupplementaryBeverages);
        DumpRVA(f, "SupplementaryIngredients",  RVA_SupplementaryIngredients);
        DumpRVA(f, "SpawnNormalGuest",          RVA_SpawnNormalGuest);
        DumpRVA(f, "RegisteredCheatedBuff",     RVA_RegisteredCheatedBuff);

        fprintf(f, "\n=== End RVA Dump ===\n\n");
        fclose(f);
    }

    inline bool Init() {
        if (!IL2CPP::Initialize()) return false;
        auto* domain = IL2CPP::domain_get();
        if (!domain) return false;
        IL2CPP::thread_attach(domain);
        initialized = true;

        // Dump RVA validation to log file
        char path[MAX_PATH];
        GetModuleFileNameA(nullptr, path, MAX_PATH);
        std::string dir(path);
        auto pos = dir.find_last_of("\\/");
        if (pos != std::string::npos) dir = dir.substr(0, pos);
        std::string logPath = dir + "\\dev_console.log";
        DumpAllRVAs(logPath.c_str());

        return true;
    }
}
