#include <filesystem>
#include <thread>

#include <globals.h>
#include <Windows.h>

using namespace std;

#define FAILURE __COUNTER__
#define SUCCESS 0

void ShowMessage(const wchar_t* message)
{
	MessageBox(NULL, message, L"YGO Mod Starter", MB_ICONERROR | MB_OK);
}

int main(int, char* [])
{
	error_code filesystem_code;
	int return_code;

	// Cleanup of a previous startup
	filesystem::remove(ready_file, filesystem_code);

	// Check for the existence of the game's executable
	if (!filesystem::exists(EXECUTABLE, filesystem_code)) {
		ShowMessage(EXECUTABLE " does not exist in the current folder.");

		return FAILURE;
	}

	// Start the game in a suspended state
	STARTUPINFO sinformation;
	PROCESS_INFORMATION pinformation;

	ZeroMemory(&sinformation, sizeof(sinformation));
	ZeroMemory(&pinformation, sizeof(pinformation));

	sinformation.cb = sizeof(sinformation);

	if (!CreateProcess(EXECUTABLE, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &sinformation, &pinformation)) {
		ShowMessage(L"Failed to start " EXECUTABLE);

		return FAILURE;
	}

	// Prepare to inject the API
	HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
	LPVOID load_library;
	LPVOID load_library_argument;

	if (kernel32 == NULL)
	{
		ShowMessage(L"Failed to get a handle for kernel32.dll.");

		return_code = FAILURE;
		goto error;
	}

	load_library = GetProcAddress(kernel32, "LoadLibraryA");

	if (load_library == NULL) {
		ShowMessage(L"Failed to get the address of LoadLibraryA.");

		return_code = FAILURE;
		goto error;
	}

	load_library_argument = VirtualAllocEx(
		pinformation.hProcess, NULL,
		sizeof(plugin_manager_dll),
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
	);

	if (load_library_argument == NULL) {
		ShowMessage(L"Failed to allocate memory for the argument of LoadLibraryA.");

		return_code = FAILURE;
		goto error;
	}

	if (!WriteProcessMemory(pinformation.hProcess, load_library_argument, plugin_manager_dll, sizeof(plugin_manager_dll), NULL)) {
		ShowMessage(L"Failed to write the argument of LoadLibraryA.");

		return_code = FAILURE;
		goto error;
	}

	//ShowMessage(L"YuGiOh will now start in a modded state.");

	// Create a thread for the API's Plugin Manager
	if (!CreateRemoteThread(pinformation.hProcess, NULL, 0, static_cast<LPTHREAD_START_ROUTINE>(load_library), load_library_argument, NULL, NULL)) {
		ShowMessage(L"Failed to create a thread for the Plugin Manager.");

		return_code = FAILURE;
		goto error;
	}

	// Wait for the API to load
	for (unsigned char tries = 0; tries < max_plugins * max_tries; ++tries) {
		if (filesystem::exists(ready_file, filesystem_code)) {
			// Resume the game's process
			if (ResumeThread(pinformation.hThread) == -1) {
				ShowMessage(L"Failed to resume the game's main thread.");

				return_code = FAILURE;
				goto error;
			}

			//ShowMessage(L"Successfully injected the API into " EXECUTABLE ".");

			return_code = SUCCESS;
			goto success;
		}

		this_thread::sleep_for(chrono::milliseconds(try_time));
	}

	ShowMessage(L"API failed to load in the given time.");

	return_code = FAILURE;

error:
	TerminateProcess(pinformation.hProcess, 0);

success:
	CloseHandle(pinformation.hProcess);
	CloseHandle(pinformation.hThread);

	return return_code;
}
