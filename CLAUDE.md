# THIzaKaYa DEV Console

## 项目概述
东方夜雀食堂 (Touhou Mystia Izakaya) 的 Unity IL2CPP 调试控制台 DLL 注入工具。
通过 D3D11 Present Hook + ImGui 实现游戏内覆盖层 UI。

## 架构
- **dllmain.cpp** - DLL 入口，创建初始化线程
- **d3d11hook.cpp** - Hook D3D11 Present，渲染 ImGui
- **console_ui.cpp** - ImGui UI 界面，按钮触发游戏函数
- **game_api.h** - 游戏函数封装 (CallVoid/CallVoidInt 等)
- **game_offsets.h** - IL2CPP RVA 偏移量 (从 dump.cs 提取)
- **il2cpp_types.h** - IL2CPP 运行时类型定义

## 关键路径
- 游戏目录: `D:\Steam\steamapps\common\Touhou Mystia Izakaya\`
- dump.cs: `D:\Steam\steamapps\common\Touhou Mystia Izakaya\Touhou Mystia Izakaya_Data\dump.cs`
- Il2CppDumper: `D:\Debugger\Il2CppDumper-win-v6.7.46\`

## 调用流程
1. DLL 注入 → dllmain 创建 MainThread
2. MainThread: 等待 GameAssembly.dll → Game::Init() (IL2CPP 初始化 + thread_attach)
3. MainThread: D3D11Hook::Initialize() (Hook Present)
4. Present Hook 线程: HookedPresent → ConsoleUI::Render → 用户点击按钮 → SafeCall → 游戏函数

## 已修复问题
### 2026-02-23: 所有函数 CRASH
- **原因**: IL2CPP::thread_attach 在 DLL MainThread 调用，但游戏函数在 Present Hook 线程执行
- **修复**: 在 SafeCall 中添加 EnsureThreadAttached()，确保 Present 线程 attach 到 IL2CPP domain

### 2026-02-23: 部分函数仍然 CRASH (C# 异常)
- **异常代码**: 0xE06D7363 (C++ exception = IL2CPP 翻译的 C# 异常)
- **原因**: 场景上下文不对 — 夜间方法在白天调用、场景切换方法在不支持的状态调用
- **崩溃函数**: CheatedBuff/EternityFever(需夜间), NextDay/SetDays(场景切换), GotoMain(场景切换)
- **修复**: 添加场景提示文字，添加 100ms 防连点冷却

### 2026-02-23: 添加 DLC 解锁功能
- **已移除**: DLC 解锁需要实际 bundle 文件，仅 patch 所有权检查会导致 AssetBundle 加载错误
- **结论**: 没有 Steam 下载的 DLC 资源文件，无法通过内存补丁实现 DLC 解锁

### 2026-02-23: 添加 RegisterMethod 支持
- **发现**: DaySceneDebugImpl 和 NightSceneDebugImpl 的 RegisterMethod() 必须先调用
- **影响**: Transmit(传送)、NextDay(快进) 等方法可能依赖 RegisterMethod 初始化的内部状态
- **修复**: 现已改为场景自动检测 + 自动注册

### 2026-02-23: 场景自动检测系统
- **原理**: 通过 UnityEngine.SceneManagement.SceneManager.GetActiveScene().name 获取当前场景名
- **RVA**: GetActiveScene=0x2CFE560, Scene.get_name=0x2CF5E70
- **功能**: 每 500ms 轮询场景，场景切换时自动调用 RegisterMethod()
- **UI**: 窗口顶部显示当前场景状态，夜间场景自动切换到夜间 tab

### 2026-02-23: NextDay/SetDays/AddDays 崩溃修复
- **根因**: NextDay 内部调用 `UniversalGameManager.LoadScene(5)`
- **机制**: LoadScene 有 `isLoadingScene` 静态标志位 (TypeInfo+184→staticFields+129)
- **问题**: 从 Present Hook 线程调用后，异步回调无法正常触发，标志位永远卡在 true
- **异常**: 后续调用检测到标志位为 true，抛出 `InvalidOperationException` (0xE06D7363)
- **修复**: 调用 LoadScene 前重置标志位 (`Game::SceneLoad::ResetLoadingFlag()`)
- **RVA**: UniversalGameManager_TypeInfo 全局指针 = 0x422F890

### 2026-02-23: 地图传送标签修正
- **问题**: 原始地图标签 (BeastPath, ScarletDevil 等) 是猜测的，二进制中不存在
- **修复**: 通过 global-metadata.dat 搜索确认正确标签
- **正确标签**: BeastForest, HumanVillage, HakureiShrine, ScarletMansion, BambooForest, Hakugyokurou, YoukaiMountain, MagicForest, FormerHell, MyourenTemple, GardenOfTheSun, FlandreHome

### 2026-02-23: 成就查询 SEH 保护
- **问题**: 成就 tab 的 GetAchievement 查询循环没有 SEH 保护，崩溃会带崩整个 UI
- **修复**: 用 SafeCall 包裹查询循环

### 2026-02-23: 成就 ID 修正
- **问题**: 原始 Steam 成就 ID (ACH_xxx) 是猜测的，全部错误
- **正确格式**: `Achievement_N_Name` (如 `Achievement_1_1stPlay`)
- **来源**: AchievementProfile ScriptableObject (XOR 0x53 加密的 Addressable bundle)
- **总数**: 142 个成就 (Achievement_1 到 Achievement_142，跳过 126)
- **系统**: 游戏内部 DataBaseAchievement 和 Steam SteamUserStats 使用相同的 ID 字符串

### 2026-02-23: NextDay 崩溃深入分析
- **现状**: 重置 isLoadingScene 标志位后仍然崩溃
- **IDA 分析**: LoadScene 内部有多个异常抛出点:
  1. isLoadingScene=true → InvalidOperationException (已通过标志位重置修复)
  2. UniversalGameManager.Instance==null → NullReferenceException
  3. Instance.sceneDataArray==null → NullReferenceException
  4. sceneIndex >= arrayLength → IndexOutOfRangeException
- **CheatedBuff 同理**: 需要 EventManager 单例存在，否则 NullReferenceException
- **修复**: 添加 LoadScene 前置诊断 (SceneLoad::Diagnose)，检查所有前置条件
- **关键偏移**: Instance 在 staticFields+0, sceneDataArray 在 Instance+280

## 日志系统
- 文件日志: `{游戏exe目录}\dev_console.log`
- 记录所有 SafeCall 结果，崩溃时记录异常代码和地址
- RVA 验证 dump 在 Init 时自动生成

## 调试类信息 (dump.cs)
- `DaySceneDebugImpl` (TypeDefIndex: 245) - public static class，白天场景调试
- `NightSceneDebugImpl` (TypeDefIndex: 264) - public static class，夜间场景调试
- `GlobalDebugConsole` (TypeDefIndex: 252) - MonoSingletonPersistant，全局调试控制台
- `GlobalDebugImpl` (TypeDefIndex: 253) - 全局调试实现

## 工具
- Visual Studio 2022 (v145 toolset, C++20)
- MinHook - API Hook 库
- Dear ImGui - UI 框架
- idapromcp - IDA Pro MCP 插件用于二进制分析

## 注意事项
- 所有 Debug 方法都是 static，调用时不需要 this 指针
- IL2CPP 静态方法签名: `RetType Func(args..., MethodInfo*)`，MethodInfo 传 nullptr
- 白天方法只能在白天场景调用，夜间方法只能在夜间场景调用
- 游戏更新后需要重新 dump 并更新 game_offsets.h 中的 RVA
