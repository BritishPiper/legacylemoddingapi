#include <Windows.h>
#include <type_traits>

#define FAILURE FALSE
#define SUCCESS TRUE

import example;
import manager;

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    // Register int the Plugin Manager
    if (reason == DLL_PROCESS_ATTACH && PluginManager::Register(new Example)) {
        return FAILURE;
    }

    return SUCCESS;
}
