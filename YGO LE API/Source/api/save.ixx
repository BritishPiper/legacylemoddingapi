module;

#include <fstream>
#include <sstream>

#include "../globals.h"
#include "../util/assembly.h"
#include "../util/error.h"
#include "../util/json.hpp"

export module save;

using namespace json;
using namespace std;

import log;

// Called whenever the game wants to pass the save file from memory to disk
Hook SaveDataToDiskHook = Hook(0x1408DA240);

// Game bugs a lot if Steam fails to save (so we return the save is always a success)
Patch SteamSaveBugPatch = Patch(0x1408DA2F2, "\x8B\xD1\x90");

// 0x140871490 - GetSaveFile

// SavedMiscData = Save + (4056 = 0xfd8)
// Unlocked Shop Packs = SavedMiscData + 0xB80
// Unlocked Battle Packs =  SavedMiscData + 0xB84
// Unknown =   SavedMiscData + 0xB88
// Completed Tutorials =   SavedMiscData + 0xB90
// Unlocked Content (Card Shop, Battlepacks and Challenges) = SavedMiscData + 0xB94

export class ExtraSave {
public:
	static void Setup(void)
	{
		Load();

		SaveDataToDiskHook.Apply(&Save_);
		SteamSaveBugPatch.Apply();
	}

	static void Load(void)
	{
		ifstream input(string(save_folder) + "extra.json");

		if (input) {
			stringstream buffer;
			buffer << input.rdbuf();

			try {
				data = JSON::Load(buffer.str());
			}
			catch (...) {
				throw CriticalError("Extra save file corrupted (Invalid JSON).");
			}

			if (!data.hasKey("campaign")) {
				Logger::Show(Warning("Extra save file corrupted (missing \"campaign\" entry)."));

				Campaign() = {};
			}

			if (!data.hasKey("character")) {
				Logger::Show(Warning("Extra save file corrupted (missing \"character\" entry)."));

				Character() = {};
			}

			if (!data.hasKey("deck")) {
				Logger::Show(Warning("Extra save file corrupted (missing \"deck\" entry)."));

				Deck() = {};
			}

			Logger::Log("\"extra.json\" loaded.");
		}
		else {
			// First Save File Being Generated
			Campaign() = {};
			Character() = {};
			Deck() = {};

			Logger::Log("Creating new save file \"extra.json\".");

			Save();
		}
	}

	static void Save(void)
	{
		ofstream output(string(save_folder) + "extra.json");

		if (output) {
			output << data;

			Logger::Log("Saved \"extra.json\" successfully.");
		}
		else {
			Logger::Log("Failed to open file \"extra.json\" for saving.");
		}
	}

	static JSON& Campaign(void)
	{
		return data["campaign"];
	}

	static JSON& Character(void)
	{
		return data["character"];
	}

	static JSON& Deck(void)
	{
		return data["deck"];
	}

private:
	static JSON data;

	// Hooks

	__declspec(noinline) static void Save_(void)
	{
		SaveDataToDiskHook.CallOriginal(&Save)();

		Call(Save);
	}
};

JSON ExtraSave::data;
