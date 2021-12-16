#pragma once

#define EXECUTABLE L"YuGiOh.exe"

// Parameters
constexpr unsigned char max_plugins = 100;

// Files and Folders
constexpr const char data_folder[] = "data/";
constexpr const char decks_folder[] = "data/decks";
constexpr const char log_file[] = "log.txt";
constexpr const char log_folder[] = "logs/";
constexpr const char plugins_folder[] = "plugins/";
constexpr const char plugin_manager_dll[] = "YGO Plugin Manager.dll";
constexpr const char save_folder[] = "save/";

// Inter-Process Communication
constexpr unsigned char max_tries = 50;
constexpr const char ready_file[] = "YGO API Loaded.IPC";
constexpr long long try_time = 100;
