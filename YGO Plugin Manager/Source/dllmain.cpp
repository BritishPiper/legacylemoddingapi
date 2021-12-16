#include <Windows.h>

#define FAILURE FALSE
#define SUCCESS TRUE

import manager;

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    // Setup the Plugin Manager
    if (reason == DLL_PROCESS_ATTACH && PluginManager::Setup()) {
        return FAILURE;
    }

    return SUCCESS;
}
