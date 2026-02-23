#pragma once
// ============================================================
// D3D11 Present Hook + Dear ImGui Overlay
// ============================================================

#include <d3d11.h>
#include <dxgi.h>

namespace D3D11Hook {
    bool Initialize();
    void Shutdown();

    // Set the callback that draws the ImGui UI each frame
    using RenderCallback = void(*)();
    void SetRenderCallback(RenderCallback cb);

    inline bool showMenu = true;
    inline HWND gameWindow = nullptr;
}
