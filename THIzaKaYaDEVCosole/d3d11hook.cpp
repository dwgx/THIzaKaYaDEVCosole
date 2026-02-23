#include "pch.h"
#include "d3d11hook.h"
#include "MinHook.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace D3D11Hook {

// --- State ---
static ID3D11Device*            g_Device = nullptr;
static ID3D11DeviceContext*     g_Context = nullptr;
static ID3D11RenderTargetView*  g_RTV = nullptr;
static IDXGISwapChain*          g_SwapChain = nullptr;
static bool                     g_ImGuiInitialized = false;
static RenderCallback           g_RenderCB = nullptr;
static WNDPROC                  g_OriginalWndProc = nullptr;

// --- Present hook types ---
using PresentFn = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
static PresentFn g_OriginalPresent = nullptr;

// --- WndProc hook ---
static LRESULT WINAPI HookedWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_DELETE) {
        showMenu = !showMenu;
        return 0;
    }
    if (showMenu) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return 1;
        // Block game input when menu is open
        switch (msg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_RBUTTONDOWN: case WM_RBUTTONUP:
        case WM_MBUTTONDOWN: case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:  case WM_MOUSEMOVE:
        case WM_KEYDOWN:     case WM_KEYUP:
        case WM_CHAR:        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
            return 0;
        }
    }
    return CallWindowProcA(g_OriginalWndProc, hWnd, msg, wParam, lParam);
}

// --- Present Hook ---
static HRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!g_ImGuiInitialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device))) {
            g_Device->GetImmediateContext(&g_Context);
            g_SwapChain = pSwapChain;

            DXGI_SWAP_CHAIN_DESC desc;
            pSwapChain->GetDesc(&desc);
            gameWindow = desc.OutputWindow;

            // Create render target view
            ID3D11Texture2D* backBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
            if (backBuffer) {
                g_Device->CreateRenderTargetView(backBuffer, nullptr, &g_RTV);
                backBuffer->Release();
            }

            // Init ImGui
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.IniFilename = nullptr; // Don't save layout
            io.MouseDrawCursor = true; // Draw software cursor when overlay is active

            // Style
            ImGui::StyleColorsDark();
            auto& style = ImGui::GetStyle();
            style.WindowRounding = 6.0f;
            style.FrameRounding = 4.0f;
            style.Alpha = 0.95f;

            // Load Chinese font from Windows
            const char* fontPath = "C:\\Windows\\Fonts\\msyh.ttc";
            if (GetFileAttributesA(fontPath) != INVALID_FILE_ATTRIBUTES) {
                io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
                // Merge symbol glyphs (✓ ⚠ etc.) into the same font
                static const ImWchar symbolRanges[] = {
                    0x2600, 0x26FF, // Misc Symbols (⚠ U+26A0)
                    0x2700, 0x27BF, // Dingbats (✓ U+2713)
                    0
                };
                ImFontConfig mergeCfg;
                mergeCfg.MergeMode = true;
                io.Fonts->AddFontFromFileTTF(fontPath, 16.0f, &mergeCfg, symbolRanges);
            }

            ImGui_ImplWin32_Init(gameWindow);
            ImGui_ImplDX11_Init(g_Device, g_Context);

            g_OriginalWndProc = (WNDPROC)SetWindowLongPtrA(gameWindow, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
            g_ImGuiInitialized = true;
        }
    }

    if (g_ImGuiInitialized && showMenu) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (g_RenderCB) g_RenderCB();

        ImGui::EndFrame();
        ImGui::Render();
        g_Context->OMSetRenderTargets(1, &g_RTV, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return g_OriginalPresent(pSwapChain, SyncInterval, Flags);
}

// --- Find Present address via dummy device ---
static void* FindPresentAddress() {
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), CS_CLASSDC, DefWindowProcA, 0, 0,
                       GetModuleHandleA(nullptr), nullptr, nullptr, nullptr, nullptr,
                       "DummyDX11", nullptr };
    RegisterClassExA(&wc);
    HWND hWnd = CreateWindowExA(0, wc.lpszClassName, "", WS_OVERLAPPEDWINDOW,
                                0, 0, 100, 100, nullptr, nullptr, wc.hInstance, nullptr);

    D3D_FEATURE_LEVEL featureLevel;
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0,
        D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pContext);

    void* presentAddr = nullptr;
    if (SUCCEEDED(hr)) {
        void** vtable = *reinterpret_cast<void***>(pSwapChain);
        presentAddr = vtable[8]; // IDXGISwapChain::Present is index 8
        pSwapChain->Release();
        pDevice->Release();
        pContext->Release();
    }

    DestroyWindow(hWnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);
    return presentAddr;
}

// --- Public API ---
static RenderCallback s_cb = nullptr;
void SetRenderCallback(RenderCallback cb) { s_cb = cb; g_RenderCB = cb; }

bool Initialize() {
    void* presentAddr = FindPresentAddress();
    if (!presentAddr) return false;

    if (MH_Initialize() != MH_OK) return false;
    if (MH_CreateHook(presentAddr, &HookedPresent, (void**)&g_OriginalPresent) != MH_OK) return false;
    if (MH_EnableHook(presentAddr) != MH_OK) return false;

    return true;
}

void Shutdown() {
    if (g_ImGuiInitialized) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        g_ImGuiInitialized = false;
    }
    if (g_RTV) { g_RTV->Release(); g_RTV = nullptr; }
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    if (g_OriginalWndProc && gameWindow) {
        SetWindowLongPtrA(gameWindow, GWLP_WNDPROC, (LONG_PTR)g_OriginalWndProc);
    }
}

} // namespace D3D11Hook
