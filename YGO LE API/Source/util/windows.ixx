module;

#include <Windows.h>
#include <Psapi.h>

#include <vector>

#include "error.h"

export module windows;

using namespace std;

import basictypes; // Byte

export auto GetModuleBase(const wchar_t* module)
{
    static Byte* base = nullptr;
    static DWORD size = 0;

    if (base == nullptr) {
        HANDLE process = GetCurrentProcess();
        DWORD bytes_needed;

        if (!EnumProcessModulesEx(process, nullptr, 0, &bytes_needed, LIST_MODULES_ALL)) {
            throw MajorError("Initial EnumProcessModulesEx failed.");
        }

        size_t n_modules = bytes_needed / sizeof(HMODULE);
        vector<HMODULE> hmodules(n_modules);

        if (!EnumProcessModulesEx(process, hmodules.data(), bytes_needed, &bytes_needed, LIST_MODULES_ALL)) {
            throw MajorError("Final EnumProcessModulesEx failed.");
        }

        for (HMODULE hmodule : hmodules) {
            MODULEINFO information;

            if (!GetModuleInformation(process, hmodule, &information, sizeof(information))) {
                throw MajorError("GetModuleInformation failed.");
            }

            wchar_t path[MAX_PATH];

            if (!GetModuleFileNameEx(process, hmodule, path, sizeof(path))) {
                throw MajorError("GetModuleFileNameEx failed.");
            }

            if (wcsstr(path, module) != nullptr) {
                base = static_cast<Byte*>(information.lpBaseOfDll);
                size = information.SizeOfImage;

                break;
            }
        }

        if (base == nullptr) {
            throw MajorError("Failed to find the main module.");
        }
    }

    struct result { decltype(base) const base; decltype(size) size; };
    return result{ base, size };
}
