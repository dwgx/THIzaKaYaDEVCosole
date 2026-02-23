#include "pch.h"
#include "il2cpp_types.h"
#include "game_api.h"
#include "d3d11hook.h"
#include "console_ui.h"

static HMODULE g_hModule = nullptr;

static DWORD WINAPI MainThread(LPVOID lpParam) {
    // Wait for game to fully load
    while (!GetModuleHandleA("GameAssembly.dll")) {
        Sleep(500);
    }
    Sleep(2000); // Extra wait for IL2CPP init

    // Initialize IL2CPP API
    if (!Game::Init()) {
        MessageBoxA(nullptr,
            "Failed to initialize IL2CPP API.\n"
            "Make sure GameAssembly.dll is loaded.",
            "THIzaKaYa DEV Console", MB_ICONERROR);
        FreeLibraryAndExitThread(g_hModule, 1);
        return 1;
    }

    // Initialize D3D11 hook + ImGui
    if (!D3D11Hook::Initialize()) {
        MessageBoxA(nullptr,
            "Failed to hook D3D11 Present.\n"
            "The game might not be using D3D11.",
            "THIzaKaYa DEV Console", MB_ICONERROR);
        FreeLibraryAndExitThread(g_hModule, 1);
        return 1;
    }

    // Set the render callback
    D3D11Hook::SetRenderCallback(ConsoleUI::Render);

    // Wait for unload key (END)
    while (!(GetAsyncKeyState(VK_END) & 1)) {
        Sleep(100);
    }

    // Cleanup
    D3D11Hook::Shutdown();
    Sleep(200);
    FreeLibraryAndExitThread(g_hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        CloseHandle(CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr));
    }
    return TRUE;
}
