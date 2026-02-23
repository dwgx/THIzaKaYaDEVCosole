#include "pch.h"
#include "console_ui.h"
#include "game_api.h"
#include "d3d11hook.h"

namespace ConsoleUI {

// --- IL2CPP thread attachment for render thread ---
static bool g_ThreadAttached = false;
static void EnsureThreadAttached() {
    if (!g_ThreadAttached && Game::initialized) {
        auto* domain = IL2CPP::domain_get();
        if (domain) {
            IL2CPP::thread_attach(domain);
            g_ThreadAttached = true;
        }
    }
}

// --- File log system ---
static std::string g_LogFilePath;
static void InitFileLog() {
    if (!g_LogFilePath.empty()) return;
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string dir(path);
    auto pos = dir.find_last_of("\\/");
    if (pos != std::string::npos) dir = dir.substr(0, pos);
    g_LogFilePath = dir + "\\dev_console.log";
    FILE* f = fopen(g_LogFilePath.c_str(), "w");
    if (f) {
        fprintf(f, "=== THIzaKaYa DEV Console Log ===\n");
        fprintf(f, "GameAssembly base: 0x%llX\n", (unsigned long long)IL2CPP::baseAddress);
        fprintf(f, "================================\n\n");
        fclose(f);
    }
}
static void FileLog(const char* fmt, ...) {
    InitFileLog();
    FILE* f = fopen(g_LogFilePath.c_str(), "a");
    if (!f) return;
    SYSTEMTIME st; GetLocalTime(&st);
    fprintf(f, "[%02d:%02d:%02d.%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    va_list args;
    va_start(args, fmt);
    vfprintf(f, fmt, args);
    va_end(args);
    fprintf(f, "\n");
    fclose(f);
}

// --- UI Log system ---
static std::vector<std::string> g_Log;
static std::mutex g_LogMutex;

static void Log(const char* fmt, ...) {
    char buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    FileLog("%s", buf);
    std::lock_guard lock(g_LogMutex);
    g_Log.push_back(buf);
    if (g_Log.size() > 200) g_Log.erase(g_Log.begin());
}

// SEH trampoline - isolate __try from C++ destructors
static thread_local std::function<void()>* g_SafeCallFn = nullptr;
static thread_local const char* g_SafeCallName = nullptr;
static void SafeCallTrampoline() { (*g_SafeCallFn)(); }

static DWORD g_LastExceptionCode = 0;
static PVOID g_LastExceptionAddr = nullptr;

static LONG WINAPI SafeCallFilter(EXCEPTION_POINTERS* ep) {
    g_LastExceptionCode = ep->ExceptionRecord->ExceptionCode;
    g_LastExceptionAddr = ep->ExceptionRecord->ExceptionAddress;
    return EXCEPTION_EXECUTE_HANDLER;
}

static bool SafeCallInner() {
    g_LastExceptionCode = 0;
    g_LastExceptionAddr = nullptr;
    __try { SafeCallTrampoline(); return true; }
    __except (SafeCallFilter(GetExceptionInformation())) { return false; }
}
// Per-button cooldown map (keyed by name pointer, which is a string literal)
static std::unordered_map<const char*, DWORD> g_ButtonCooldowns;
static constexpr DWORD COOLDOWN_MS = 100;

static void SafeCall(const char* name, std::function<void()> fn) {
    DWORD now = GetTickCount();
    auto& lastTick = g_ButtonCooldowns[name];
    if (now - lastTick < COOLDOWN_MS) return;
    lastTick = now;

    EnsureThreadAttached();
    g_SafeCallName = name;
    g_SafeCallFn = &fn;
    if (SafeCallInner()) {
        Log("[OK] %s", name);
    } else {
        Log("[CRASH] %s (code=0x%08X addr=0x%llX)",
            name, g_LastExceptionCode, (unsigned long long)g_LastExceptionAddr);
        FileLog("  -> GameAssembly base=0x%llX, crash offset=0x%llX",
            (unsigned long long)IL2CPP::baseAddress,
            (unsigned long long)((uintptr_t)g_LastExceptionAddr - IL2CPP::baseAddress));
    }
    g_SafeCallFn = nullptr;
    g_SafeCallName = nullptr;
}

// ============================================================
// SCENE AUTO-DETECTION
// ============================================================
static Game::SceneType g_CurrentScene = Game::SceneType::Unknown;
static Game::SceneType g_PrevScene = Game::SceneType::Unknown;
static std::string g_SceneName;
static bool g_DayRegistered = false;
static bool g_NightRegistered = false;
static DWORD g_LastScenePoll = 0;

static void PollScene() {
    DWORD now = GetTickCount();
    if (now - g_LastScenePoll < 500) return; // poll every 500ms
    g_LastScenePoll = now;

    EnsureThreadAttached();
    g_SceneName = Game::Scene::GetCurrentSceneName();
    g_CurrentScene = Game::Scene::DetectScene();

    // Scene changed - auto register
    if (g_CurrentScene != g_PrevScene) {
        FileLog("Scene changed: %s -> %s (%s)",
            g_PrevScene == Game::SceneType::Day ? "Day" :
            g_PrevScene == Game::SceneType::Night ? "Night" :
            g_PrevScene == Game::SceneType::Main ? "Main" : "?",
            g_CurrentScene == Game::SceneType::Day ? "Day" :
            g_CurrentScene == Game::SceneType::Night ? "Night" :
            g_CurrentScene == Game::SceneType::Main ? "Main" : "?",
            g_SceneName.c_str());

        // Reset registration flags on scene change
        g_DayRegistered = false;
        g_NightRegistered = false;

        // Auto-register for new scene
        if (g_CurrentScene == Game::SceneType::Day) {
            SafeCall("Auto::Day::RegisterMethod", [] { Game::Day::RegisterMethod(); });
            g_DayRegistered = true;
        } else if (g_CurrentScene == Game::SceneType::Night) {
            SafeCall("Auto::Night::RegisterMethod", [] { Game::Night::RegisterMethod(); });
            g_NightRegistered = true;
        }

        g_PrevScene = g_CurrentScene;
    }
}

// --- Scene status display helper ---
static const char* SceneLabel() {
    switch (g_CurrentScene) {
    case Game::SceneType::Day:   return "\xe7\x99\xbd\xe5\xa4\xa9 (Day)";
    case Game::SceneType::Night: return "\xe5\xa4\x9c\xe9\x97\xb4 (Night)";
    case Game::SceneType::Main:  return "\xe4\xb8\xbb\xe8\x8f\x9c\xe5\x8d\x95 (Main)";
    default: return "\xe6\x9c\xaa\xe7\x9f\xa5";
    }
}
static ImVec4 SceneColor() {
    switch (g_CurrentScene) {
    case Game::SceneType::Day:   return ImVec4(1,0.8f,0.2f,1);
    case Game::SceneType::Night: return ImVec4(0.4f,0.6f,1,1);
    case Game::SceneType::Main:  return ImVec4(0.2f,1,0.6f,1);
    default: return ImVec4(0.5f,0.5f,0.5f,1);
    }
}

// --- Input state ---
static int inputMoney = 99999;
static int inputExp = 10000;
static int inputDays = 1;
static int inputFrogCoin = 99;
static int inputHours = 1;
static int inputWorkTime = 300;
static int inputWorkIncome = 99999;
static int inputDonate = 1000;
static int inputPartnerCount = 3;
static int inputFreeCookSec = 600;

// --- Map data for teleport ---
struct MapEntry { const char* label; const char* displayName; };
static const MapEntry g_Maps[] = {
    // Core (核心地图)
    {"BeastForest",     "\xe5\x85\xbd\xe9\x81\x93"},
    {"HumanVillage",    "\xe4\xba\xba\xe9\x87\x8c"},
    {"HakureiShrine",   "\xe5\x8d\x9a\xe4\xb8\xbd\xe7\xa5\x9e\xe7\xa4\xbe"},
    {"ScarletMansion",  "\xe7\xba\xa2\xe9\xad\x94\xe9\xa6\x86"},
    {"BambooForest",    "\xe7\xab\xb9\xe6\x9e\x97"},
    {"Hakugyokurou",    "\xe7\x99\xbd\xe7\x8e\x89\xe6\xa5\xbc"},
    // DLC1
    {"YoukaiMountain",  "\xe5\xa6\x96\xe6\x80\xaa\xe4\xb9\x8b\xe5\xb1\xb1"},
    {"MagicForest",     "\xe9\xad\x94\xe6\xb3\x95\xe6\xa3\xae\xe6\x9e\x97"},
    // DLC2
    {"FormerHell",      "\xe6\x97\xa7\xe5\x9c\xb0\xe7\x8b\xb1"},
    // DLC3
    {"MyourenTemple",   "\xe5\x91\xbd\xe8\x8e\xb2\xe5\xaf\xba"},
    // DLC4
    {"GardenOfTheSun",  "\xe5\xa4\xaa\xe9\x98\xb3\xe8\x8a\xb1\xe7\x94\xb0"},
    {"FlandreHome",     "\xe7\xba\xa2\xe9\xad\x94\xe9\xa6\x86\xe5\x9c\xb0\xe4\xb8\x8b"},
};

// --- Steam achievement IDs (from AchievementProfile ScriptableObject) ---
struct AchievementEntry { const char* id; const char* displayName; };
static AchievementEntry g_Achievements[] = {
    // Core story
    {"Achievement_1_1stPlay",                   "\xe9\xa6\x96\xe6\xac\xa1\xe8\xbf\x9b\xe5\x85\xa5\xe6\xb8\xb8\xe6\x88\x8f"},
    {"Achievement_2_PlotYykDestroys",           "\xe5\xa4\x9c\xe9\x9b\x80\xe9\xa3\x9f\xe5\xa0\x82\xe8\xa2\xab\xe7\xa0\xb4\xe5\x9d\x8f"},
    {"Achievement_3_QuestBeastForestPayoff",    "\xe5\x85\xbd\xe9\x81\x93\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_4_QuestHumanVillagePayoff",   "\xe4\xba\xba\xe9\x87\x8c\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_5_QuestHakureiShrinePayoff",  "\xe5\x8d\x9a\xe4\xb8\xbd\xe7\xa5\x9e\xe7\xa4\xbe\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_6_QuestScarletMansionPayoff", "\xe7\xba\xa2\xe9\xad\x94\xe9\xa6\x86\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_7_QuestBambooForestPayoff",   "\xe7\xab\xb9\xe6\x9e\x97\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_8_ChallengeFinalYyk",         "\xe6\x9c\x80\xe7\xbb\x88\xe6\x8c\x91\xe6\x88\x98"},
    {"Achievement_9_5rateWithAddup",            "5\xe6\x98\x9f\xe8\xaf\x84\xe4\xbb\xb7(\xe5\x8a\xa0\xe6\x88\x90)"},
    {"Achievement_10_1stAwardSC",               "\xe9\xa6\x96\xe6\xac\xa1\xe5\xa5\x96\xe5\x8a\xb1\xe7\xa8\x80\xe5\xae\xa2"},
    {"Achievement_11_1stPunishSC_ID",           "\xe9\xa6\x96\xe6\xac\xa1\xe6\x83\xa9\xe7\xbd\x9a\xe7\xa8\x80\xe5\xae\xa2"},
    // Collectibles
    {"Achievement_12_1stCollectupBeastForest",  "\xe5\x85\xbd\xe9\x81\x93\xe9\x87\x87\xe9\x9b\x86\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_13_1stCollectupHumanVillage", "\xe4\xba\xba\xe9\x87\x8c\xe9\x87\x87\xe9\x9b\x86\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_14_1stCollectupHakureiShrine","\xe7\xa5\x9e\xe7\xa4\xbe\xe9\x87\x87\xe9\x9b\x86\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_15_1stCollectupScarletMansion","\xe7\xba\xa2\xe9\xad\x94\xe9\xa6\x86\xe9\x87\x87\xe9\x9b\x86\xe5\xae\x8c\xe6\x88\x90"},
    {"Achievement_16_1stCollectupBambooForest", "\xe7\xab\xb9\xe6\x9e\x97\xe9\x87\x87\xe9\x9b\x86\xe5\xae\x8c\xe6\x88\x90"},
    // Kizuna
    {"Achievement_44_KizunaLv2x5_ID",          "\xe7\xbe\x81\xe7\xbb\x8aLv2 x5"},
    {"Achievement_45_KizunaLv2x10_ID",         "\xe7\xbe\x81\xe7\xbb\x8aLv2 x10"},
    {"Achievement_46_KizunaLv2x15_ID",         "\xe7\xbe\x81\xe7\xbb\x8aLv2 x15"},
    {"Achievement_47_KizunaLv5x5_ID",          "\xe7\xbe\x81\xe7\xbb\x8aLv5 x5"},
    {"Achievement_48_KizunaLv5x10_ID",         "\xe7\xbe\x81\xe7\xbb\x8aLv5 x10"},
    {"Achievement_49_KizunaLv5x15_ID",         "\xe7\xbe\x81\xe7\xbb\x8aLv5 x15"},
    // Gameplay
    {"Achievement_70_PremadeSoldout",           "\xe9\xa2\x84\xe5\x88\xb6\xe6\x96\x99\xe7\x90\x86\xe5\x85\xa8\xe5\x94\xae\xe5\x87\xba"},
    {"Achievement_72_GetAllRecpToufu",          "\xe8\x8e\xb7\xe5\xbe\x97\xe5\x85\xa8\xe9\x83\xa8\xe8\xb1\x86\xe8\x85\x90\xe9\xa3\x9f\xe8\xb0\xb1"},
    {"Achievement_73_GetAllRecpGrill",          "\xe8\x8e\xb7\xe5\xbe\x97\xe5\x85\xa8\xe9\x83\xa8\xe7\x83\xa7\xe7\x83\xa4\xe9\xa3\x9f\xe8\xb0\xb1"},
    {"Achievement_79_SoldGreenTea10+",          "\xe5\x8d\x96\xe5\x87\xba" "10+\xe6\x9d\xaf\xe7\xbb\xbf\xe8\x8c\xb6"},
    {"Achievement_80_Sold5rateRiceBall30+",     "30+\xe4\xb8\xaa" "5\xe6\x98\x9f\xe9\xa5\xad\xe5\x9b\xa2"},
    {"Achievement_88_Waiting1+",               "\xe7\xad\x89\xe5\xbe\x85" "1+\xe5\x88\x86\xe9\x92\x9f"},
    {"Achievement_94_Profit500+",              "\xe5\x88\xa9\xe6\xb6\xa6" "500+"},
    {"Achievement_95_Profit5000+",             "\xe5\x88\xa9\xe6\xb6\xa6" "5000+"},
    {"Achievement_96_Profit10000+",            "\xe5\x88\xa9\xe6\xb6\xa6" "10000+"},
    // Boss battles
    {"Achievement_124_KaguyaWin",              "\xe8\xbe\x89\xe5\xa4\x9c\xe5\xa7\xac\xe8\x83\x9c\xe5\x88\xa9"},
    {"Achievement_125_MokouWin",               "\xe5\xa6\xb9\xe7\xba\xa2\xe8\x83\x9c\xe5\x88\xa9"},
    {"Achievement_130_ConcertEnd",             "\xe6\xbc\x94\xe5\x94\xb1\xe4\xbc\x9a\xe7\xbb\x93\xe6\x9d\x9f"},
    {"Achievement_131_GetAllKizuna",           "\xe5\x85\xa8\xe7\xbe\x81\xe7\xbb\x8a"},
    {"Achievement_138_SellCharacters",         "\xe5\x8d\x96\xe5\x87\xba\xe8\xa7\x92\xe8\x89\xb2"},
    // Meta
    {"Achievement_142_AllUnlocked",            "\xe5\x85\xa8\xe6\x88\x90\xe5\xb0\xb1\xe8\xa7\xa3\xe9\x94\x81"},
};
static const int g_AchievementCount = sizeof(g_Achievements) / sizeof(g_Achievements[0]);
static bool g_AchievementStatus[64] = {};
static bool g_AchievementQueried = false;

// ============================================================
// TAB RENDERERS
// ============================================================
static void RenderDayTab() {
    // 只在确认是夜间场景时禁用，Unknown/Other 不禁用（可能是检测不到）
    bool blocked = (g_CurrentScene == Game::SceneType::Night);
    if (g_CurrentScene == Game::SceneType::Day) {
        if (g_DayRegistered)
            ImGui::TextColored(ImVec4(0.2f,1,0.2f,1), "\xe2\x9c\x93 \xe7\x99\xbd\xe5\xa4\xa9\xe5\x9c\xba\xe6\x99\xaf\xef\xbc\x8c\xe8\xb0\x83\xe8\xaf\x95\xe5\xb7\xb2\xe6\xb3\xa8\xe5\x86\x8c");
    } else if (blocked) {
        ImGui::TextColored(ImVec4(1,0.4f,0.3f,1), "\xe2\x9a\xa0 \xe5\xbd\x93\xe5\x89\x8d\xe5\x9c\xa8\xe5\xa4\x9c\xe9\x97\xb4\xe5\x9c\xba\xe6\x99\xaf\xef\xbc\x8c\xe7\x99\xbd\xe5\xa4\xa9\xe5\x8a\x9f\xe8\x83\xbd\xe5\xb7\xb2\xe7\xa6\x81\xe7\x94\xa8");
    } else {
        ImGui::TextColored(ImVec4(1,0.8f,0.2f,1), "\xe2\x9a\xa0 \xe5\x9c\xba\xe6\x99\xaf\xe6\x9c\xaa\xe8\xaf\x86\xe5\x88\xab\xef\xbc\x8c\xe5\x8a\x9f\xe8\x83\xbd\xe5\x8f\xaf\xe8\x83\xbd\xe6\x97\xa0\xe6\x95\x88");
    }
    ImGui::Separator();
    if (blocked) ImGui::BeginDisabled();

    if (ImGui::CollapsingHeader("\xe9\x87\x91\xe9\x92\xb1 / \xe8\xb5\x84\xe6\xba\x90", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::InputInt("\xe9\x87\x91\xe9\xa2\x9d##money", &inputMoney, 1000, 10000);
        ImGui::SameLine();
        if (ImGui::Button("\xe8\x8e\xb7\xe5\xbe\x97\xe9\x87\x91\xe9\x92\xb1##getmoney"))
            SafeCall("GetMoney", [&]{ Game::Day::GetMoney(inputMoney); });
        if (ImGui::Button("\xe6\x9c\x80\xe5\xa4\xa7\xe8\xb5\x84\xe6\xba\x90"))
            SafeCall("GetMaxResources", []{ Game::Day::GetMaxResources(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe9\x9a\x8f\xe6\x9c\xba\xe8\xb5\x84\xe6\xba\x90"))
            SafeCall("GetRandomResources", []{ Game::Day::GetRandomResources(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb5\x8b\xe8\xaf\x95\xe8\xb5\x84\xe6\xba\x90"))
            SafeCall("GetTestResources", []{ Game::Day::GetTestResources(); });
        ImGui::InputInt("\xe9\x9d\x92\xe8\x9b\x99\xe5\xb8\x81##frog", &inputFrogCoin, 10, 100);
        ImGui::SameLine();
        if (ImGui::Button("\xe8\x8e\xb7\xe5\xbe\x97##getfrog"))
            SafeCall("GetSilverFrogCoin", [&]{ Game::Day::GetSilverFrogCoin(inputFrogCoin); });
        ImGui::InputInt("\xe7\xbb\x8f\xe9\xaa\x8c\xe5\x80\xbc##exp", &inputExp, 1000, 10000);
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb7\xbb\xe5\x8a\xa0\xe7\xbb\x8f\xe9\xaa\x8c##addexp"))
            SafeCall("AddExp", [&]{ Game::Day::AddExp(inputExp); });
    }

    if (ImGui::CollapsingHeader("\xe6\x94\xb6\xe9\x9b\x86\xe5\x93\x81 / \xe8\xa7\xa3\xe9\x94\x81")) {
        if (ImGui::Button("\xe5\x85\xa8\xe8\xa1\xa3\xe6\x9c\x8d"))   SafeCall("GetAllClothes", []{ Game::Day::GetAllClothes(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8" "CD\xe5\x94\xb1\xe7\x89\x87")) SafeCall("GetAllRecords", []{ Game::Day::GetAllRecords(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8\xe5\xbe\xbd\xe7\xab\xa0"))   SafeCall("GetAllBadges", []{ Game::Day::GetAllBadges(); });
        if (ImGui::Button("\xe5\x85\xa8\xe8\xa3\x85\xe9\xa5\xb0\xe5\x93\x81")) SafeCall("GetAndUseAllDecorations", []{ Game::Day::GetAndUseAllDecorations(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8\xe4\xbc\x99\xe4\xbc\xb4"))   SafeCall("GetAllPartners", []{ Game::Day::GetAllPartners(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8\xe9\x92\x93\xe9\xb1\xbc\xe6\x88\x98\xe5\x88\xa9\xe5\x93\x81")) SafeCall("GetAllFishingTrophy", []{ Game::Day::GetAllFishingTrophy(); });
        if (ImGui::Button("\xe8\xa7\xa3\xe9\x94\x81\xe7\xa8\x80\xe5\xae\xa2+\xe5\x9c\xb0\xe5\x9b\xbe")) SafeCall("UnlockAll", []{ Game::Day::UnlockSpecialGuestsAndMaps(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xa7\xa3\xe9\x94\x81\xe5\x85\xa8\xe9\x9f\xb3\xe6\xb8\xb8")) SafeCall("UnlockMusic", []{ Game::Day::UnlockAllMusicChapters(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8\xe7\xbe\x81\xe7\xbb\x8a\xe6\xbb\xa1\xe7\xba\xa7")) SafeCall("FullKizuna", []{ Game::Day::FullUpgradeAllKizunaLevel(); });
    }
    if (ImGui::CollapsingHeader("\xe6\x97\xb6\xe9\x97\xb4\xe6\x8e\xa7\xe5\x88\xb6")) {
        // LoadScene pre-flight diagnostic
        static std::string diagResult;
        if (ImGui::Button("\xe8\xaf\x8a\xe6\x96\xad LoadScene##diag")) {
            diagResult = Game::SceneLoad::Diagnose(5);
            Log("[DIAG] LoadScene(5): %s", diagResult.c_str());
        }
        if (!diagResult.empty()) {
            bool isOk = diagResult.find("OK:") == 0;
            ImGui::SameLine();
            ImGui::TextColored(isOk ? ImVec4(0.2f,1,0.2f,1) : ImVec4(1,0.3f,0.3f,1),
                "%s", diagResult.c_str());
        }

        if (Game::SceneLoad::IsLoading()) {
            ImGui::TextColored(ImVec4(1,0.6f,0.2f,1), "\xe2\x9a\xa0 \xe5\x9c\xba\xe6\x99\xaf\xe5\x8a\xa0\xe8\xbd\xbd\xe6\xa0\x87\xe5\xbf\x97\xe5\x8d\xa1\xe6\xad\xbb\xef\xbc\x8c\xe5\xb7\xb2\xe8\x87\xaa\xe5\x8a\xa8\xe9\x87\x8d\xe7\xbd\xae");
            Game::SceneLoad::ResetLoadingFlag();
        }
        if (ImGui::Button("\xe5\xbf\xab\xe8\xbf\x9b\xe5\x88\xb0\xe6\x98\x8e\xe5\xa4\xa9")) {
            auto diag = Game::SceneLoad::Diagnose(5);
            if (diag.find("OK:") == 0) {
                SafeCall("NextDay", []{ Game::SceneLoad::ResetLoadingFlag(); Game::Day::NextDay(); });
            } else {
                Log("[SKIP] NextDay: %s", diag.c_str());
            }
        }
        ImGui::InputInt("\xe5\xa4\xa9\xe6\x95\xb0##days", &inputDays, 1, 10);
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xae\xbe\xe7\xbd\xae##setdays")) {
            auto diag = Game::SceneLoad::Diagnose(5);
            if (diag.find("OK:") == 0)
                SafeCall("SetDays", [&]{ Game::SceneLoad::ResetLoadingFlag(); Game::Day::SetDays(inputDays); });
            else
                Log("[SKIP] SetDays: %s", diag.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("\xe5\xa2\x9e\xe5\x8a\xa0##adddays")) {
            auto diag = Game::SceneLoad::Diagnose(5);
            if (diag.find("OK:") == 0)
                SafeCall("AddDays", [&]{ Game::SceneLoad::ResetLoadingFlag(); Game::Day::AddDays(inputDays); });
            else
                Log("[SKIP] AddDays: %s", diag.c_str());
        }
        ImGui::InputInt("\xe5\xb0\x8f\xe6\x97\xb6##hours", &inputHours, 1, 4);
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb6\x88\xe8\x80\x97\xe6\x97\xb6\xe9\x97\xb4"))
            SafeCall("WarpHours", [&]{ Game::Day::WarpHours(inputHours); });
    }

    if (ImGui::CollapsingHeader("\xe6\xb8\x85\xe7\xa9\xba / \xe4\xbb\xbb\xe5\x8a\xa1")) {
        if (ImGui::Button("\xe6\xb8\x85\xe7\xa9\xba\xe9\xa3\x9f\xe6\x9d\x90")) SafeCall("ClearIng", []{ Game::Day::ClearAllIngredients(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb8\x85\xe7\xa9\xba\xe9\x85\x92\xe6\xb0\xb4")) SafeCall("ClearBev", []{ Game::Day::ClearAllBeverages(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb8\x85\xe7\xa9\xba\xe5\xb0\x8f\xe9\x81\x93\xe5\x85\xb7")) SafeCall("ClearItems", []{ Game::Day::ClearAllItems(); });
        if (ImGui::Button("\xe5\xbc\x80\xe5\xa7\x8b\xe6\x89\x80\xe6\x9c\x89\xe4\xbb\xbb\xe5\x8a\xa1")) SafeCall("StartAll", []{ Game::Day::StartAllMissions(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\xae\x8c\xe6\x88\x90\xe6\x89\x80\xe6\x9c\x89\xe4\xbb\xbb\xe5\x8a\xa1")) SafeCall("FinishAll", []{ Game::Day::FinishAllEventsAndMissions(); });
    }

    if (ImGui::CollapsingHeader("\xe6\x9d\x82\xe9\xa1\xb9")) {
        if (ImGui::Button("\xe5\x88\xb7\xe6\x96\xb0\xe9\x87\x87\xe9\x9b\x86\xe7\x82\xb9")) SafeCall("Refresh", []{ Game::Day::RefreshCollectable(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe9\x87\x8d\xe7\xbd\xae\xe5\x86\xb7\xe5\x8d\xb4")) SafeCall("ResetCD", []{ Game::Day::ResetCollectableCoolDown(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x88\xb7\xe6\x96\xb0NPC")) SafeCall("RefreshNPC", []{ Game::Day::RefreshNPC(); });
        if (ImGui::Button("\xe5\x8e\xbb\xe9\xa6\x99\xe9\x9c\x96\xe5\xa0\x82")) SafeCall("Kourindou", []{ Game::Day::GotoKourindou(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe9\xa6\x99\xe9\x9c\x96\xe5\xa0\x82\xe8\xa1\xa5\xe8\xb4\xa7")) SafeCall("Restock", []{ Game::Day::AddToKourindoStaticMerchandise(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe9\x82\x80\xe8\xaf\xb7\xe5\xa5\xb8\xe5\x95\x86\xe5\xb8\x9d")) SafeCall("Tewi", []{ Game::Day::InviteProfiteerTewi(); });
        if (ImGui::Button("\xe8\xbf\x9b\xe5\x85\xa5RogueLike")) SafeCall("RogueLike", []{ Game::Day::MoveToRogueLike(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe7\x81\xb5\xe6\xa2\xa6\xe7\xac\xa6\xe5\x8d\xa1")) SafeCall("ReimuSpell", []{ Game::Day::AddReimuSpell(); });
        ImGui::InputInt("\xe6\x8d\x90\xe6\xac\xbe##donate", &inputDonate, 100, 1000);
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x8d\x9a\xe4\xb8\xbd\xe9\x92\xb1\xe7\xae\xb1"))
            SafeCall("Donate", [&]{ Game::Day::HakureiMoneyBoxDonate(inputDonate); });
        ImGui::InputInt("\xe4\xbc\x99\xe4\xbc\xb4\xe6\x95\xb0##partner", &inputPartnerCount, 1, 1);
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xae\xbe\xe7\xbd\xae\xe4\xbc\x99\xe4\xbc\xb4\xe6\x95\xb0"))
            SafeCall("SetPartner", [&]{ Game::Day::SetMaxPartnerCount(inputPartnerCount); });
    }
    if (blocked) ImGui::EndDisabled();
}

static void RenderNightTab() {
    bool blocked = (g_CurrentScene == Game::SceneType::Day || g_CurrentScene == Game::SceneType::Main);
    if (g_CurrentScene == Game::SceneType::Night) {
        if (g_NightRegistered)
            ImGui::TextColored(ImVec4(0.2f,1,0.2f,1), "\xe2\x9c\x93 \xe5\xa4\x9c\xe9\x97\xb4\xe5\x9c\xba\xe6\x99\xaf\xef\xbc\x8c\xe8\xb0\x83\xe8\xaf\x95\xe5\xb7\xb2\xe6\xb3\xa8\xe5\x86\x8c");
    } else if (blocked) {
        ImGui::TextColored(ImVec4(1,0.4f,0.3f,1), "\xe2\x9a\xa0 \xe5\xbd\x93\xe5\x89\x8d\xe4\xb8\x8d\xe5\x9c\xa8\xe5\xa4\x9c\xe9\x97\xb4\xe5\x9c\xba\xe6\x99\xaf\xef\xbc\x8c\xe6\x8c\x89\xe9\x92\xae\xe5\xb7\xb2\xe7\xa6\x81\xe7\x94\xa8");
    } else {
        ImGui::TextColored(ImVec4(1,0.8f,0.2f,1), "\xe2\x9a\xa0 \xe5\x9c\xba\xe6\x99\xaf\xe6\x9c\xaa\xe8\xaf\x86\xe5\x88\xab\xef\xbc\x8c\xe5\x8a\x9f\xe8\x83\xbd\xe5\x8f\xaf\xe8\x83\xbd\xe6\x97\xa0\xe6\x95\x88");
    }
    ImGui::Separator();
    if (blocked) ImGui::BeginDisabled();

    if (ImGui::CollapsingHeader("\xe4\xbd\x9c\xe5\xbc\x8a" "Buff", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("\xe6\xb0\xb8\xe4\xb9\x85\xe7\xb2\x89\xe8\xaf\x84\xe4\xbd\x9c\xe5\xbc\x8a"))
            SafeCall("CheatedBuff", []{ Game::Night::RegisteredCheatedBuff(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe6\xb0\xb8\xe7\xbb\xad\xe7\x83\xad\xe7\x81\xab\xe6\x9c\x9d\xe5\xa4\xa9"))
            SafeCall("EternityFever", []{ Game::Night::RegisteredEternityFever(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe7\xa7\xbb\xe9\x99\xa4\xe6\x89\x80\xe6\x9c\x89" "Buff"))
            SafeCall("RemoveBuff", []{ Game::Night::RemoveAllTimedBuff(); });
        ImGui::InputInt("\xe5\x85\x8d\xe8\xb4\xb9\xe7\x83\xb9\xe9\xa5\xaa(\xe7\xa7\x92)##freecook", &inputFreeCookSec, 60, 300);
        ImGui::SameLine();
        if (ImGui::Button("\xe5\xbc\x80\xe5\x90\xaf##freecook"))
            SafeCall("FreeCook", [&]{ Game::Night::SetFreeCook(inputFreeCookSec); });
    }

    if (ImGui::CollapsingHeader("\xe8\xa1\xa5\xe5\x85\x85\xe7\x89\xa9\xe8\xb5\x84")) {
        if (ImGui::Button("\xe8\xa1\xa5\xe5\x85\x85\xe9\x85\x92\xe6\xb0\xb4"))
            SafeCall("SupBev", []{ Game::Night::SupplementaryBeverages(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xa1\xa5\xe5\x85\x85\xe9\xa3\x9f\xe6\x9d\x90"))
            SafeCall("SupIng", []{ Game::Night::SupplementaryIngredients(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xa1\xa5\xe5\x85\x85\xe6\x96\x99\xe7\x90\x86"))
            SafeCall("SupFood", []{ Game::Night::SupplementaryFoods(); });
    }

    if (ImGui::CollapsingHeader("\xe6\x94\xb6\xe5\x85\xa5 / \xe6\x97\xb6\xe9\x97\xb4")) {
        ImGui::InputInt("\xe6\x94\xb6\xe5\x85\xa5##income", &inputWorkIncome, 1000, 10000);
        ImGui::SameLine();
        if (ImGui::Button("\xe8\xae\xbe\xe7\xbd\xae\xe6\x94\xb6\xe5\x85\xa5"))
            SafeCall("SetIncome", [&]{ Game::Night::SetWorkIncome(inputWorkIncome, 0); });
        ImGui::InputInt("\xe8\x90\xa5\xe4\xb8\x9a\xe6\x97\xb6\xe9\x97\xb4(\xe7\xa7\x92)##worktime", &inputWorkTime, 30, 120);
        ImGui::SameLine();
        if (ImGui::Button("\xe4\xbf\xae\xe6\x94\xb9\xe6\x97\xb6\xe9\x97\xb4"))
            SafeCall("ModTime", [&]{ Game::Night::ModifyWorkTime(inputWorkTime); });
    }

    if (ImGui::CollapsingHeader("\xe5\xae\xa2\xe4\xba\xba\xe7\xae\xa1\xe7\x90\x86")) {
        if (ImGui::Button("\xe7\x94\x9f\xe6\x88\x90\xe6\x99\xae\xe5\xae\xa2"))
            SafeCall("SpawnNormal", []{ Game::Night::SpawnNormalGuest(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe7\x94\x9f\xe6\x88\x90\xe7\xa8\x80\xe5\xae\xa2"))
            SafeCall("SpawnSpecial", []{ Game::Night::SpawnSpecialGuest(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe5\x85\xa8\xe9\x83\xa8\xe4\xbb\x98\xe6\xac\xbe\xe7\xa6\xbb\xe5\xbc\x80"))
            SafeCall("RepellPay", []{ Game::Night::RepellGuestAndPay(); });
    }

    if (ImGui::CollapsingHeader("QTE Buff")) {
        if (ImGui::Button("\xe9\x9a\x8f\xe6\x9c\xbaQTE"))
            SafeCall("RandomQTE", []{ Game::Night::TriggerRandomQTEBuff(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe6\x8a\x95\xe6\x8e\xb7\xe4\xb8\x8a\xe8\x8f\x9c"))
            SafeCall("ThrowQTE", []{ Game::Night::TriggerThrowServingQTE(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe8\x80\x90\xe5\xbf\x83\xe4\xb8\x8d\xe5\x87\x8f"))
            SafeCall("PatientQTE", []{ Game::Night::TriggerPatientQTE(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe7\xab\x8b\xe5\x8d\xb3\xe5\xae\x8c\xe9\xa3\x9f"))
            SafeCall("FinishEat", []{ Game::Night::TriggerFinishEatingQTE(); });
    }

    if (ImGui::CollapsingHeader("\xe6\xb0\x94\xe6\xb0\x9b")) {
        if (ImGui::Button("\xe6\x8f\x90\xe5\x8d\x87\xe6\xb0\x94\xe6\xb0\x9b"))
            SafeCall("IncPassion", []{ Game::Night::IncreasePassionLevel(); });
        ImGui::SameLine();
        if (ImGui::Button("\xe9\x99\x8d\xe4\xbd\x8e\xe6\xb0\x94\xe6\xb0\x9b"))
            SafeCall("DecPassion", []{ Game::Night::DecreasePassionLevel(); });
    }
    if (blocked) ImGui::EndDisabled();
}

static void RenderGlobalTab() {
    ImGui::TextColored(ImVec4(0.2f,1,0.6f,1), "\xe5\x85\xa8\xe5\xb1\x80 (Global)");
    ImGui::Separator();
    static bool enableConsole = false;
    if (ImGui::Checkbox("\xe5\x90\xaf\xe7\x94\xa8\xe6\xb8\xb8\xe6\x88\x8f\xe5\x86\x85\xe7\xbd\xae\xe8\xb0\x83\xe8\xaf\x95\xe6\x8e\xa7\xe5\x88\xb6\xe5\x8f\xb0", &enableConsole))
        SafeCall("EnableConsole", [&]{ Game::Global::SetEnableConsole(enableConsole); });
    ImGui::Spacing();
    if (ImGui::Button("\xe5\x89\x8d\xe5\xbe\x80\xe4\xb8\xbb\xe8\x8f\x9c\xe5\x8d\x95")) {
        auto diag = Game::SceneLoad::Diagnose(1);
        if (diag.find("OK:") == 0)
            SafeCall("GotoMain", []{ Game::SceneLoad::ResetLoadingFlag(); Game::Global::GotoMainScene(); });
        else
            Log("[SKIP] GotoMain: %s", diag.c_str());
    }
    ImGui::SameLine();
    if (ImGui::Button("\xe5\xaf\xbc\xe5\x87\xba\xe5\xad\x98\xe6\xa1\xa3\xe5\x92\x8c\xe6\x97\xa5\xe5\xbf\x97"))
        SafeCall("ExportLog", []{ Game::Global::OutputArchiveAndLog(); });
    ImGui::SameLine();
    if (ImGui::Button("\xe6\x89\x93\xe5\xbc\x80\xe6\x97\xa5\xe5\xbf\x97\xe7\x9b\xae\xe5\xbd\x95"))
        SafeCall("OpenLog", []{ Game::Global::OpenLogDirectory(); });
}

static void RenderMapsTab() {
    // 只在确认是夜间/主菜单时禁用
    bool blocked = (g_CurrentScene == Game::SceneType::Night || g_CurrentScene == Game::SceneType::Main);
    if (!blocked && g_CurrentScene != Game::SceneType::Day) {
        ImGui::TextColored(ImVec4(1,0.8f,0.2f,1), "\xe2\x9a\xa0 \xe5\x9c\xba\xe6\x99\xaf\xe6\x9c\xaa\xe8\xaf\x86\xe5\x88\xab\xef\xbc\x8c\xe4\xbc\xa0\xe9\x80\x81\xe5\x8f\xaf\xe8\x83\xbd\xe6\x97\xa0\xe6\x95\x88");
    } else if (blocked) {
        ImGui::TextColored(ImVec4(1,0.4f,0.3f,1), "\xe2\x9a\xa0 \xe4\xbc\xa0\xe9\x80\x81\xe4\xbb\x85\xe5\x9c\xa8\xe7\x99\xbd\xe5\xa4\xa9\xe5\x9c\xba\xe6\x99\xaf\xe6\x9c\x89\xe6\x95\x88");
    }
    ImGui::Separator();
    if (blocked) ImGui::BeginDisabled();
    const char* sections[] = { "\xe6\xa0\xb8\xe5\xbf\x83\xe5\x9c\xb0\xe5\x9b\xbe", "DLC1", "DLC2", "DLC3", "DLC4" };
    int sectionStart[] = { 0, 6, 8, 9, 10 };
    int sectionEnd[]   = { 6, 8, 9, 10, 12 };
    int mapCount = sizeof(g_Maps) / sizeof(g_Maps[0]);
    for (int s = 0; s < 5; s++) {
        if (sectionEnd[s] > mapCount) sectionEnd[s] = mapCount;
        if (sectionStart[s] >= mapCount) continue;
        if (ImGui::TreeNodeEx(sections[s], ImGuiTreeNodeFlags_DefaultOpen)) {
            for (int i = sectionStart[s]; i < sectionEnd[s]; i++) {
                if (ImGui::Button(g_Maps[i].displayName, ImVec2(140, 0)))
                    SafeCall("Transmit", [i]{ Game::Day::Transmit(g_Maps[i].label); });
                if ((i - sectionStart[s]) % 3 != 2 && i + 1 < sectionEnd[s])
                    ImGui::SameLine();
            }
            ImGui::TreePop();
        }
    }
    if (blocked) ImGui::EndDisabled();
}

static void RenderAchievementTab() {
    ImGui::TextColored(ImVec4(1,0.85f,0.3f,1), "\xe6\x88\x90\xe5\xb0\xb1\xe7\xae\xa1\xe7\x90\x86 (Achievements)");
    ImGui::Separator();

    // Full list of all 142 achievement IDs for bulk unlock
    static const char* g_AllAchievementIds[] = {
        "Achievement_1_1stPlay", "Achievement_2_PlotYykDestroys",
        "Achievement_3_QuestBeastForestPayoff", "Achievement_4_QuestHumanVillagePayoff",
        "Achievement_5_QuestHakureiShrinePayoff", "Achievement_6_QuestScarletMansionPayoff",
        "Achievement_7_QuestBambooForestPayoff", "Achievement_8_ChallengeFinalYyk",
        "Achievement_9_5rateWithAddup", "Achievement_10_1stAwardSC",
        "Achievement_11_1stPunishSC_ID", "Achievement_12_1stCollectupBeastForest",
        "Achievement_13_1stCollectupHumanVillage", "Achievement_14_1stCollectupHakureiShrine",
        "Achievement_15_1stCollectupScarletMansion", "Achievement_16_1stCollectupBambooForest",
        "Achievement_17_KizunaLv2Rumia", "Achievement_18_KizunaLv4Rumia",
        "Achievement_19_KizunaLv5Rumia", "Achievement_20_KizunaLv2Wriggle",
        "Achievement_21_KizunaLv4Wriggle", "Achievement_22_KizunaLv5Wriggle",
        "Achievement_23_KizunaLv2Chen", "Achievement_24_KizunaLv4Chen",
        "Achievement_25_KizunaLv5Chen", "Achievement_26_KizunaLv2Keine",
        "Achievement_27_KizunaLv4Keine", "Achievement_28_KizunaLv5Keine",
        "Achievement_29_KizunaLv2Kasen", "Achievement_30_KizunaLv4Kasen",
        "Achievement_31_KizunaLv5Kasen", "Achievement_32_KizunaLv2Akyuu",
        "Achievement_33_KizunaLv4Akyuu", "Achievement_34_KizunaLv5Akyuu",
        "Achievement_35_KizunaLv2Reimu", "Achievement_36_KizunaLv4Reimu",
        "Achievement_37_KizunaLv5Reimu", "Achievement_38_KizunaLv2Suika",
        "Achievement_39_KizunaLv4Suika", "Achievement_40_KizunaLv5Suika",
        "Achievement_41_KizunaLv2Tenshi", "Achievement_42_KizunaLv4Tenshi",
        "Achievement_43_KizunaLv5Tenshi", "Achievement_44_KizunaLv2x5_ID",
        "Achievement_45_KizunaLv2x10_ID", "Achievement_46_KizunaLv2x15_ID",
        "Achievement_47_KizunaLv5x5_ID", "Achievement_48_KizunaLv5x10_ID",
        "Achievement_49_KizunaLv5x15_ID",
        "Achievement_50_AwardSCRumia_ID", "Achievement_51_PunishSCRumia_ID",
        "Achievement_52_AwardSCWriggle_ID", "Achievement_53_PunishSCWriggle_ID",
        "Achievement_54_AwardSCChen_ID", "Achievement_55_PunishSCChen_ID",
        "Achievement_56_AwardSCKeine_ID", "Achievement_57_PunishSCKeine_ID",
        "Achievement_58_AwardSCKasen_ID", "Achievement_59_PunishSCKasen_ID",
        "Achievement_60_AwardSCAkyuu_ID", "Achievement_61_PunishSCAkyuu_ID",
        "Achievement_62_AwardSCMarisa_ID", "Achievement_63_PunishSCMarisa_ID",
        "Achievement_64_AwardSCReimu_ID", "Achievement_65_PunishSCReimu_ID",
        "Achievement_66_AwardSCSuika_ID", "Achievement_67_PunishSCSuika_ID",
        "Achievement_68_AwardSCTenshi_ID", "Achievement_69_PunishSCTenshi_ID",
        "Achievement_70_PremadeSoldout", "Achievement_71_1stDarkMatter_ID",
        "Achievement_72_GetAllRecpToufu", "Achievement_73_GetAllRecpGrill",
        "Achievement_74_GetUdumbara_ID", "Achievement_75_GetSparrowWine_ID",
        "Achievement_76_GetMashroom_ID", "Achievement_77_1stGetNews",
        "Achievement_78_5QuestsOfNews_ID",
        "Achievement_79_SoldGreenTea10+", "Achievement_80_Sold5rateRiceBall30+",
        "Achievement_81_FailedTagCulturedx10_ID", "Achievement_82_Expel100+_ID",
        "Achievement_83_AwardSC20+_ID", "Achievement_84_AwardSC50+_ID",
        "Achievement_85_AwardSC100+_ID", "Achievement_86_PunishSC20+_ID",
        "Achievement_87_PunishSC50+_ID",
        "Achievement_88_Waiting1+", "Achievement_89_Waiting10+", "Achievement_90_Waiting20+",
        "Achievement_91_1stTips_ID", "Achievement_92_Tips1000+", "Achievement_93_Tips5000+",
        "Achievement_94_Profit500+", "Achievement_95_Profit5000+", "Achievement_96_Profit10000+",
        "Achievement_97_AwardSCMeirin_ID", "Achievement_98_PunishSCMeirin_ID",
        "Achievement_99_KizunaLv2Meirin", "Achievement_100_KizunaLv4Meirin",
        "Achievement_101_KizunaLv5Meirin", "Achievement_102_KizunaLv2Patchouli",
        "Achievement_103_KizunaLv4Patchouli", "Achievement_104_KizunaLv5Patchouli",
        "Achievement_105_KizunaLv2Cirno", "Achievement_106_KizunaLv4Cirno",
        "Achievement_107_KizunaLv5Cirno", "Achievement_108_KizunaLv2Tewi",
        "Achievement_109_KizunaLv4Tewi", "Achievement_110_KizunaLv5Tewi",
        "Achievement_111_KizunaLv2Kaguya", "Achievement_112_KizunaLv4Kaguya",
        "Achievement_113_KizunaLv5Kaguya",
        "Achievement_114_AwardSCPatchouli_ID", "Achievement_115_PunishSCPatchouli_ID",
        "Achievement_116_AwardSCCirno_ID", "Achievement_117_PunishSCCirno_ID",
        "Achievement_118_AwardSCTewi_ID", "Achievement_119_PunishSCTewi_ID",
        "Achievement_120_AwardSCKaguya_ID", "Achievement_121_PunishSCKaguya_ID",
        "Achievement_122_AwardSCMokou_ID", "Achievement_123_PunishSCMokou_ID",
        "Achievement_124_KaguyaWin", "Achievement_125_MokouWin",
        "Achievement_127_ChallengeLV1", "Achievement_128_ChallengeLV2",
        "Achievement_129_ChallengeBossFail", "Achievement_130_ConcertEnd",
        "Achievement_131_GetAllKizuna",
        "Achievement_132_ExchangeWineLV1_ID", "Achievement_133_ExchangeWineLV2_ID",
        "Achievement_134_ExchangeWineLV3_ID",
        "Achievement_135_StrongBuyAndSellLV1_ID", "Achievement_136_StrongBuyAndSellLV2_ID",
        "Achievement_137_StrongBuyAndSellLV3_ID",
        "Achievement_138_SellCharacters",
        "Achievement_139_KizunaLv2Mokou", "Achievement_140_KizunaLv4Mokou",
        "Achievement_141_KizunaLv5Mokou",
        "Achievement_142_AllUnlocked",
    };
    static const int g_AllAchievementCount = sizeof(g_AllAchievementIds) / sizeof(g_AllAchievementIds[0]);

    if (ImGui::Button("\xe4\xb8\x80\xe9\x94\xae\xe8\xa7\xa3\xe9\x94\x81\xe5\x85\xa8\xe9\x83\xa8\xe6\x88\x90\xe5\xb0\xb1 (142\xe4\xb8\xaa)")) {
        SafeCall("UnlockAllAchievements", []{
            int ok = 0, fail = 0;
            for (int i = 0; i < g_AllAchievementCount; i++) {
                if (Game::Achievement::SetAchievement(g_AllAchievementIds[i]))
                    ok++;
                else
                    fail++;
            }
            Game::Achievement::StoreStats();
            FileLog("  UnlockAll: %d ok, %d fail", ok, fail);
        });
        g_AchievementQueried = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("\xe5\x88\xb7\xe6\x96\xb0\xe7\x8a\xb6\xe6\x80\x81"))
        g_AchievementQueried = false;
    ImGui::SameLine();
    if (ImGui::Button("\xe6\xa3\x80\xe6\x9f\xa5\xe6\xb8\xb8\xe6\x88\x8f\xe6\x88\x90\xe5\xb0\xb1"))
        SafeCall("CheckAllAchievement", []{ Game::Achievement::CheckAllAchievement(); });

    if (!g_AchievementQueried) {
        SafeCall("QueryAchievements", []{
            for (int i = 0; i < g_AchievementCount; i++) {
                bool achieved = false;
                Game::Achievement::GetAchievement(g_Achievements[i].id, achieved);
                g_AchievementStatus[i] = achieved;
            }
        });
        g_AchievementQueried = true;
    }

    ImGui::Spacing();
    ImGui::BeginChild("AchList", ImVec2(0, 0), true);
    float unlockBtnX = ImGui::GetContentRegionAvail().x - 60;
    for (int i = 0; i < g_AchievementCount; i++) {
        ImGui::PushID(i);
        if (g_AchievementStatus[i])
            ImGui::TextColored(ImVec4(0.2f,1,0.2f,1), "[OK]");
        else
            ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1), "[  ]");
        ImGui::SameLine();
        ImGui::Text("%s", g_Achievements[i].displayName);
        ImGui::SameLine(unlockBtnX);
        if (!g_AchievementStatus[i]) {
            if (ImGui::SmallButton("\xe8\xa7\xa3\xe9\x94\x81")) {
                int idx = i;
                SafeCall("UnlockAch", [idx]{
                    Game::Achievement::SetAchievement(g_Achievements[idx].id);
                    Game::Achievement::StoreStats();
                });
                g_AchievementStatus[i] = true;
            }
        } else {
            ImGui::TextColored(ImVec4(0.3f,0.8f,0.3f,1), "\xe5\xb7\xb2\xe8\xa7\xa3\xe9\x94\x81");
        }
        ImGui::PopID();
    }
    ImGui::EndChild();
}

static void RenderLogTab() {
    ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1), "\xe6\x97\xa5\xe5\xbf\x97 (Log)");
    ImGui::Separator();
    if (ImGui::Button("\xe6\xb8\x85\xe7\xa9\xba\xe6\x97\xa5\xe5\xbf\x97")) {
        std::lock_guard lock(g_LogMutex);
        g_Log.clear();
    }
    ImGui::BeginChild("LogScroll", ImVec2(0, 0), true);
    {
        std::lock_guard lock(g_LogMutex);
        for (auto& line : g_Log) {
            if (line.find("[OK]") != std::string::npos)
                ImGui::TextColored(ImVec4(0.2f,1,0.2f,1), "%s", line.c_str());
            else if (line.find("[CRASH]") != std::string::npos)
                ImGui::TextColored(ImVec4(1,0.2f,0.2f,1), "%s", line.c_str());
            else if (line.find("[SKIP]") != std::string::npos)
                ImGui::TextColored(ImVec4(1,0.7f,0.2f,1), "%s", line.c_str());
            else if (line.find("[DIAG]") != std::string::npos)
                ImGui::TextColored(ImVec4(0.4f,0.8f,1,1), "%s", line.c_str());
            else
                ImGui::TextUnformatted(line.c_str());
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}

// ============================================================
// MAIN RENDER
// ============================================================
void Render() {
    if (!D3D11Hook::showMenu) return;

    ImGui::SetNextWindowSize(ImVec2(720, 560), ImGuiCond_FirstUseEver);
    ImGui::Begin("\xe4\xb8\x9c\xe6\x96\xb9\xe5\xa4\x9c\xe9\x9b\x80\xe9\xa3\x9f\xe5\xa0\x82 DEV Console  [DEL \xe5\xbc\x80/\xe5\x85\xb3]",
                 &D3D11Hook::showMenu, ImGuiWindowFlags_NoCollapse);

    if (!Game::initialized) {
        ImGui::TextColored(ImVec4(1,0.3f,0.3f,1), "GameAssembly.dll \xe6\x9c\xaa\xe5\x8a\xa0\xe8\xbd\xbd\xe6\x88\x96IL2CPP\xe5\x88\x9d\xe5\xa7\x8b\xe5\x8c\x96\xe5\xa4\xb1\xe8\xb4\xa5!");
        if (ImGui::Button("\xe9\x87\x8d\xe8\xaf\x95\xe5\x88\x9d\xe5\xa7\x8b\xe5\x8c\x96"))
            Game::Init();
        ImGui::End();
        return;
    }

    // Poll scene and auto-register
    PollScene();

    // Scene status bar
    ImGui::TextColored(SceneColor(), "\xe5\xbd\x93\xe5\x89\x8d\xe5\x9c\xba\xe6\x99\xaf: %s", SceneLabel());
    if (!g_SceneName.empty()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1), "(%s)", g_SceneName.c_str());
    }
    // 手动注册按钮（场景检测失败时可手动触发）
    if (g_CurrentScene != Game::SceneType::Day && g_CurrentScene != Game::SceneType::Night) {
        ImGui::SameLine();
        if (ImGui::SmallButton("\xe6\xb3\xa8\xe5\x86\x8c\xe7\x99\xbd\xe5\xa4\xa9")) {
            SafeCall("Manual::Day::Register", [] { Game::Day::RegisterMethod(); });
            g_DayRegistered = true;
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("\xe6\xb3\xa8\xe5\x86\x8c\xe5\xa4\x9c\xe9\x97\xb4")) {
            SafeCall("Manual::Night::Register", [] { Game::Night::RegisterMethod(); });
            g_NightRegistered = true;
        }
    }
    ImGui::Separator();

    if (ImGui::BeginTabBar("MainTabs")) {
        // Show Day tab first if in Day scene, Night tab first if in Night scene
        if (g_CurrentScene == Game::SceneType::Night) {
            if (ImGui::BeginTabItem("\xe5\xa4\x9c\xe9\x97\xb4"))   { RenderNightTab();       ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("\xe7\x99\xbd\xe5\xa4\xa9"))   { RenderDayTab();         ImGui::EndTabItem(); }
        } else {
            if (ImGui::BeginTabItem("\xe7\x99\xbd\xe5\xa4\xa9"))   { RenderDayTab();         ImGui::EndTabItem(); }
            if (ImGui::BeginTabItem("\xe5\xa4\x9c\xe9\x97\xb4"))   { RenderNightTab();       ImGui::EndTabItem(); }
        }
        if (ImGui::BeginTabItem("\xe5\x9c\xb0\xe5\x9b\xbe"))   { RenderMapsTab();        ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("\xe6\x88\x90\xe5\xb0\xb1"))   { RenderAchievementTab(); ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("\xe5\x85\xa8\xe5\xb1\x80"))   { RenderGlobalTab();      ImGui::EndTabItem(); }
        if (ImGui::BeginTabItem("\xe6\x97\xa5\xe5\xbf\x97"))   { RenderLogTab();         ImGui::EndTabItem(); }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

} // namespace ConsoleUI
