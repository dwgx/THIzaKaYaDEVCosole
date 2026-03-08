## Compliance Notice / 合规声明

- This repository is provided only for education, reverse engineering research, debugging, and interoperability study.
- Do not use any code or ideas here for unauthorized access, cheating in online services, privacy invasion, data theft, malware delivery, or service disruption.
- You must comply with applicable laws, platform Terms of Service, and software/game EULA before any use.
- If any content infringes your rights, open an issue or contact the maintainer for removal.
- Full statement: [DISCLAIMER.md](./DISCLAIMER.md)

---
# 东方夜雀食堂 DEV Console

Unity IL2CPP 调试控制台，通过 DLL 注入 + D3D11 Present Hook + Dear ImGui 实现游戏内覆盖层 UI。

适用于 **东方夜雀食堂 (Touhou Mystia Izakaya)** Steam 版本。

## 功能

- **白天场景调试** — 金钱/资源修改、全收集品解锁、羁绊满级、时间快进、任务完成
- **夜间场景调试** — 作弊 Buff、补充物资、客人生成/驱逐、QTE 触发、气氛控制
- **地图传送** — 核心地图 + DLC1~4 全地图一键传送
- **Steam 成就** — 142 个成就一键解锁 / 单独解锁
- **全局控制** — 场景切换、存档导出、内置调试控制台开关
- **场景自动检测** — 自动识别白天/夜间/主菜单，自动注册调试方法
- **崩溃保护** — SEH 包裹所有游戏函数调用，崩溃不影响游戏运行
- **日志系统** — UI 内实时日志 + 文件日志 (`dev_console.log`)

## 截图

按 `DEL` 键打开/关闭控制台。

## 使用方法

### 1. 下载

从 [Releases](../../releases) 下载最新的 `THIzaKaYaDEVCosole.dll`。

### 2. 注入

使用任意 DLL 注入器将 DLL 注入到游戏进程 `Touhou Mystia Izakaya.exe`：

推荐注入器：
- [Process Hacker](https://processhacker.sourceforge.io/)
- [Xenos Injector](https://github.com/DarthTon/Xenos)
- 或任何支持 x64 DLL 注入的工具

### 3. 操作

| 按键 | 功能 |
|------|------|
| `DEL` | 开关控制台 |
| `END` | 卸载 DLL |

## 架构

```
dllmain.cpp          DLL 入口，创建初始化线程
├── game_api.h       游戏函数封装 (CallVoid/CallVoidInt/SafeCall)
│   ├── il2cpp_types.h   IL2CPP 运行时类型定义
│   └── game_offsets.h   RVA 偏移量 (从 dump.cs 提取)
├── d3d11hook.cpp    Hook D3D11 Present，初始化 ImGui
└── console_ui.cpp   ImGui UI 界面，按钮触发游戏函数
```

## 核心写法

### IL2CPP 函数调用

游戏使用 Unity IL2CPP，所有 C# 方法编译为 native 函数。调试类方法都是 `static`，IL2CPP 签名为：

```cpp
// IL2CPP 静态方法签名: RetType Func(args..., MethodInfo*)
// MethodInfo* 传 nullptr 即可
using Fn = void(*)(const MethodInfo*);
auto fn = reinterpret_cast<Fn>(IL2CPP::RVA(rva));
fn(nullptr);
```

### RVA 偏移获取

使用 [Il2CppDumper](https://github.com/Perfare/Il2CppDumper) 从游戏文件提取 `dump.cs`，其中包含所有类和方法的 RVA：

```cs
// dump.cs 中的方法定义示例
// RVA: 0x6E4B50 Offset: 0x6E3550 VA: 0x1806E4B50
public static void RegisterMethod() { }
```

### D3D11 Present Hook

```cpp
// 1. 创建临时 D3D11 设备获取 Present 虚函数地址
void** vtable = *reinterpret_cast<void***>(pSwapChain);
void* presentAddr = vtable[8]; // IDXGISwapChain::Present = index 8

// 2. 用 MinHook 替换
MH_CreateHook(presentAddr, &HookedPresent, (void**)&g_OriginalPresent);

// 3. 在 HookedPresent 中渲染 ImGui
HRESULT WINAPI HookedPresent(IDXGISwapChain* pSwapChain, UINT sync, UINT flags) {
    // ... ImGui NewFrame / Render / EndFrame ...
    return g_OriginalPresent(pSwapChain, sync, flags);
}
```

### SEH 崩溃保护

```cpp
// 所有游戏函数调用都通过 SafeCall 包裹
// __try/__except 不能与 C++ 对象混用 (MSVC C2712)
// 所以用 trampoline 隔离
static bool SafeCallInner() {
    __try { SafeCallTrampoline(); return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}
```

### MonoSingleton 双 TypeInfo

IL2CPP 泛型单例 `MonoSingleton<T>` 的 Instance 和派生类的静态字段在不同的 TypeInfo 中：

```cpp
// MonoSingleton<UGM> TypeInfo → staticFields → Instance (offset 0)
constexpr uintptr_t RVA_SingletonTypeInfo = 0x4258E80;
// UGM concrete TypeInfo → staticFields → isLoadingScene (offset 129)
constexpr uintptr_t RVA_ConcreteTypeInfo  = 0x422F890;
```

## 构建

### 环境要求

- Visual Studio 2022 (v143 工具集, C++20)
- Windows SDK 10.0+

### 依赖 (已包含在 vendor/)

- [MinHook](https://github.com/TsudaKageworthy/minhook) — API Hook
- [Dear ImGui](https://github.com/ocornut/imgui) — UI 框架 (含 DX11 + Win32 backend)

### 编译

1. 打开 `THIzaKaYaDEVCosole.slnx`
2. 选择 `Release | x64`
3. 生成 → 输出 `x64/Release/THIzaKaYaDEVCosole.dll`

## 游戏更新后

游戏更新会改变 RVA 偏移。需要：

1. 用 Il2CppDumper 重新 dump
2. 在新的 `dump.cs` 中找到对应方法的新 RVA
3. 更新 `game_offsets.h`

## 免责声明

本工具仅供学习和研究用途。使用本工具修改游戏可能违反游戏服务条款，请自行承担风险。

