module;

#include <unordered_map>

#include "../globals.h"
#include "../util/assembly.h"
#include "../util/error.h"

export module deck;

using namespace std;

import basictypes;
import campaign;
import card;
import log;
import stream;

using namespace card;

#pragma pack(push, 1)
export struct Deck { // Size: 0x84
	ID id; // 0x0
	CampaignSeries series; // 0x4
	ID character_id; // 0x8
	CardID signature_card; // 0xC
	short unknown2; //0xE
	String id_name; // 0x10
	GameString name; // 0x18
	GameString unknown3; // 0x38
	GameString unknown4; // 0x58
	int unknown5; // 0x78
	int unknown6; // 0x7C
	DLC dlc; // 0x80
	int unknown7; // 0x84
};
#pragma pack(pop)

// Called by the game to get a deck given an ID
Hook GetDeckAddressHook = Hook(0x1407FE420);

// SavedDecks = Save + (14280 = 0x37c8)
// SavedMiscData = Save + (4056 = 0xfd8)
// Unlocked Challenges = SavedMiscData + 0x38
// Unlocked Recipes = SavedMiscData + 0xB28

export class Decks {
public:
	static void Setup(void)
	{
		ApplyPatches();
		Logger::Log("Decks patches applied.");

		ApplyHooks();
		Logger::Log("Decks hooks applied.");

		ReadExtraDecks();
		Logger::Log("Extra decks loaded.");
	}

private:
	static unordered_map<ID, Deck> decks;

	static void ApplyHooks(void)
	{
		GetDeckAddressHook.Apply(&GetDeckAddress_);
	}

	static void ApplyPatches(void)
	{
	}

	static void ReadExtraDecks(void)
	{
		ByteStream extradecks(string(data_folder) + "extradecks.bin");

		if (!extradecks) {
			Logger::Show(Warning("Skipping extra decks because the file \"extradecks.bin\" is missing."));

			return;
		}

		size_t n_decks = extradecks.Read<size_t>();
		decks.reserve(n_decks);

		for (unsigned int index = 0; index < n_decks; ++index) {
			/*ID entry_id = */extradecks.Read<ID>();

			ID id = extradecks.Read<ID>() + 0x2BB;
			Deck& deck = (*decks.insert({ id, Deck{} }).first).second;
			deck.id = id;

			extradecks.Read(deck.series);
			extradecks.Read(deck.signature_card);
			extradecks.Read<short>();
			extradecks.Read(deck.character_id);

			extradecks.Read(deck.dlc);
			if (deck.dlc == -1) {
				deck.dlc = 1;
			}

			size_t id_name = extradecks.Read<size_t>();
			size_t name = extradecks.Read<size_t>();
			size_t unknown3 = extradecks.Read<size_t>();
			size_t unknown4 = extradecks.Read<size_t>();

			extradecks.Push();
			extradecks.SeekAndReadString(id_name, deck.id_name);
			extradecks.SeekAndReadString(name, deck.name.pointer, deck.name.length, deck.name.size);
			extradecks.SeekAndReadString(unknown3, deck.unknown3.pointer, deck.unknown3.length, deck.unknown3.size);
			extradecks.SeekAndReadString(unknown4, deck.unknown4.pointer, deck.unknown4.length, deck.unknown4.size);
			extradecks.Pop();
		}
	}

	__declspec(noinline) static Deck* GetDeckAddress(ID id)
	{
		if (decks.contains(id)) {
			return &decks[id];
		}
		else {
			return GetDeckAddressHook.CallOriginal(&GetDeckAddress)(id);
		}
	}

	// Hooks

	__declspec(noinline) static void GetDeckAddress_(void)
	{
		Call(GetDeckAddress);
	}
};

unordered_map<ID, Deck> Decks::decks;
