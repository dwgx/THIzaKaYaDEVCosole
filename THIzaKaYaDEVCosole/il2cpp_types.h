#pragma once
// ============================================================
// IL2CPP Type Definitions for Unity IL2CPP Runtime
// ============================================================

#include <cstdint>

struct Il2CppObject {
    void* klass;
    void* monitor;
};

struct Il2CppString : Il2CppObject {
    int32_t length;
    wchar_t chars[1];
};

struct Il2CppArray : Il2CppObject {
    void* bounds;
    uintptr_t max_length;
};

struct MethodInfo;
struct Il2CppDomain;
struct Il2CppThread;
struct Il2CppClass;
struct Il2CppImage;
struct Il2CppAssembly;

// IL2CPP API function typedefs
using il2cpp_domain_get_t          = Il2CppDomain* (*)();
using il2cpp_thread_attach_t       = Il2CppThread* (*)(Il2CppDomain*);
using il2cpp_thread_detach_t       = void (*)(Il2CppThread*);
using il2cpp_string_new_t          = Il2CppString* (*)(const char*);
using il2cpp_string_new_utf16_t    = Il2CppString* (*)(const wchar_t*, int32_t);
using il2cpp_runtime_invoke_t      = Il2CppObject* (*)(const MethodInfo*, void*, void**, Il2CppObject**);
using il2cpp_class_from_name_t     = Il2CppClass* (*)(const Il2CppImage*, const char*, const char*);
using il2cpp_domain_get_assemblies_t = const Il2CppAssembly** (*)(const Il2CppDomain*, size_t*);
using il2cpp_assembly_get_image_t  = const Il2CppImage* (*)(const Il2CppAssembly*);
using il2cpp_image_get_name_t      = const char* (*)(const Il2CppImage*);

// Global IL2CPP API pointers
namespace IL2CPP {
    inline HMODULE hGameAssembly = nullptr;
    inline uintptr_t baseAddress = 0;

    inline il2cpp_domain_get_t          domain_get = nullptr;
    inline il2cpp_thread_attach_t       thread_attach = nullptr;
    inline il2cpp_thread_detach_t       thread_detach = nullptr;
    inline il2cpp_string_new_t          string_new = nullptr;
    inline il2cpp_string_new_utf16_t    string_new_utf16 = nullptr;
    inline il2cpp_runtime_invoke_t      runtime_invoke = nullptr;

    inline bool Initialize() {
        hGameAssembly = GetModuleHandleA("GameAssembly.dll");
        if (!hGameAssembly) return false;
        baseAddress = reinterpret_cast<uintptr_t>(hGameAssembly);

        auto resolve = [](const char* name) -> void* {
            return GetProcAddress(hGameAssembly, name);
        };

        domain_get       = (il2cpp_domain_get_t)resolve("il2cpp_domain_get");
        thread_attach    = (il2cpp_thread_attach_t)resolve("il2cpp_thread_attach");
        thread_detach    = (il2cpp_thread_detach_t)resolve("il2cpp_thread_detach");
        string_new       = (il2cpp_string_new_t)resolve("il2cpp_string_new");
        string_new_utf16 = (il2cpp_string_new_utf16_t)resolve("il2cpp_string_new_utf16");
        runtime_invoke   = (il2cpp_runtime_invoke_t)resolve("il2cpp_runtime_invoke");

        return domain_get && thread_attach && string_new;
    }

    inline uintptr_t RVA(uintptr_t offset) {
        return baseAddress + offset;
    }
}
