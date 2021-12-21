module;

#include <Windows.h>

// Standard Libraries
#include <filesystem>
#include <fstream>
#include <thread>
#include <unordered_map>

// API Libraries
#include <globals.h>
#include <util/error.h>

export module manager;

using namespace std;
using namespace filesystem;

import arena;
import card;
import campaign;
import character;
import deck;
import log;
import plugin;
import save;

export class PluginManager {
public:
	__declspec(dllexport) static void Handle(const Error& error) noexcept
	{
		Logger::Show(error);
	}

	__declspec(dllexport) static bool Register(Plugin* plugin)
	{
		// Add it to the list of managed plugins
		plugins.insert({ plugin->Name(), unique_ptr<Plugin>(plugin) });

		// Return false to mean a successful execution
		return false;
	}

	static bool Setup(void)
	{
		// Setup logging
		Logger::Setup();

		// Global try-catch block to handle all initialization exceptions
		try {
			// Load persistent data (save/configuration)
			ExtraSave::Setup();

			// Initialize the API
			Arenas::Setup();
			card::Cards::Setup();
			Campaign::Setup();
			Characters::Setup();
			Decks::Setup();

			// List and load all mods in the plugins folder
			for (directory_entry const& file : recursive_directory_iterator(plugins_folder))
			{
				path path = file.path();

				if (is_regular_file(file) && path.extension() == ".dll") {
					string name = path.stem().string();

					if (plugins.contains(name)) {
						Handle(MinorError("Skipping duplicated plugin \"{}\".", name.c_str()));
					}
					else {
						// Start loading the plugin
						size_t n_plugins = plugins.size() + 1;
						LoadLibrary(path.wstring().c_str());

						// Wait for the plugin to load
						for (unsigned char tries = 0; tries < max_tries; ++tries) {
							if (plugins.size() == n_plugins) {
								Logger::Log("Successfully loaded plugin \"{}\".", name.c_str());

								goto success;
							}

							this_thread::sleep_for(chrono::milliseconds(try_time));
						}

						Logger::Log("Plugin \"{}\" failed to load in the given time.", name.c_str());

					success:
						// Check for plugin overflow
						if (plugins.size() >= max_plugins) {
							Logger::Log("Maximum number of {} plugins reached.", max_plugins);

							break;
						}
					}
				}
			}
		}
		catch (const Error& error) {
			Handle(error);
		}
		catch (const char* message) {
			Handle(Exception(message));
		}
		catch (...) {
			Handle(Exception("No suitable handler for exception."));
		}

		// Signal the API is ready
		ofstream ready(ready_file);
		ready.close();

		// Return false to mean a successful execution
		return false;
	}

private:
	static unordered_map<string, unique_ptr<Plugin>> plugins;
};

unordered_map<string, unique_ptr<Plugin>> PluginManager::plugins;
