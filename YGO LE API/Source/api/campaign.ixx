module;

#include <string>
#include <unordered_map>

#include "../globals.h"
#include "../util/assembly.h"
#include "../util/error.h"

export module campaign;

using namespace std;

import basictypes;
import log;
import save;
import stream;

#pragma pack(push, 1)
export struct CampaignCharacter { // Size: 0x10
	ID id;
	ID deck_id;
	String alternate_skin;
};

export enum class CampaignSeries : ID {
	YuGiOh = 0,
	YuGiOhGX = 1,
	YuGiOh5D = 2,
	YuGiOhZEXAL = 3,
	YuGiOhARCV = 4,
	YuGiOhVRAINS = 5
};

export struct CampaignDuel { // Size: 0xB0
	ID id; // 0x0
	int unknown1; // 0x4
	CampaignCharacter left_character; // 0x8
	CampaignCharacter right_character; // 0x18
	ID arena_id; // 0x28
	int unknown2; // 0x2C
	ID reward_pack_id; // 0x30
	int unknown3; // 0x34
	String id_name; // 0x38
	GameString name; // 0x40
	int unknown9; // 0x60
	int unknown10; // 0x64
	int unknown11; // 0x68
	int unknown12; // 0x6C
	int unknown13; // 0x70
	int unknown14; // 0x74
	int unknown15; // 0x78
	int unknown16; // 0x7C
	int unknown17; // 0x80
	int unknown18; // 0x84
	int unknown19; // 0x88
	int unknown20; // 0x8C
	int unknown21; // 0x90
	int unknown22; // 0x94
	int unknown23; // 0x98
	int unknown24; // 0x9C
	CampaignSeries series; // 0xA0
	ID number_in_series; // 0xA4
	int unknown26; // 0xA8
	int unknown27; // 0xAC

	~CampaignDuel(void)
	{
		if (left_character.alternate_skin) {
			delete[] left_character.alternate_skin;
		}

		if (right_character.alternate_skin) {
			delete[] right_character.alternate_skin;
		}

		if (id_name) {
			delete[] id_name;
		}

		if (name.pointer) {
			delete[] name.pointer;
		}
	}
};
#pragma pack(pop)

// Called when the game is loading to create a table containing all campaign duels per series
Patch GenerateDuelsPerSeriesTablePatch1 = Patch(0x14074A40B, "\x2D");
Patch GenerateDuelsPerSeriesTablePatch2 = Patch(0x14074A437, "\xEB");
Patch GenerateDuelsPerSeriesTablePatch3 = Patch(0x14074A4BB, "\x24");
Patch GenerateDuelsPerSeriesTablePatch4 = Patch(0x14074A4DE, "\xEB");

// Called by the game to get a duel given an ID
Hook GetDuelAddressHook = Hook(0x1407FF6B0);

// Called by the game to get the characters of a duel
Patch GetDuelCharacterPatch = Patch(0x1407FF4A4, "\x90\x90\x90\x90\x41\x50\x48\x89\xD1\xE8\xFE\x01\x00\x00\x48\x83\xF8\x00\x41\x58\x48\x89\xC1\x90\x90\x90");

// Called when starting a campaign duel to get the characters' decks
Patch GetCampaignDuelDeckPatch1 = Patch(0x1407FF453, "\x90\x90\x48\x98\x48\x89\xC1\xE8\x51\x02\x00\x00\x48\x89\xC1\x90\x90\x90\x90\x90\x90");
Patch GetCampaignDuelDeckPatch2 = Patch(0x1407FF673, "\x90\x90\x48\x98\x48\x89\xC1\xE8\x31\x00\x00\x00\x48\x89\xC1\x90\x90\x90\x90\x90\x90");

// SavedCampaignData = Save + (7024 = 0x1b70)
// Unlocked Campaign Duels = SavedCampaignData + 0x8
// Each duel size is 6 * 0x4 = 0x18, except the first duel (representing the series), which is 6 * 0x4 + 2 = 0x1A

// Reads save data on campaign duel selection screen
Hook SelectDuelHook1 = Hook(0x140861513); // Get whether to show the duel name or a lock in a given entry
Hook SelectDuelHook2 = Hook(0x140860523); // Get whether the duel is unlocked when clicking on the screen to start a duel
Hook SelectDuelHook3 = Hook(0x140861B32); // Don't know what this does, but it constantly accesses the unlocked campaign duels save data
Hook SelectDuelHook4 = Hook(0x140861725); // Get whether to show the duel character in shadowed/black or normal form
Hook SelectDuelHook5 = Hook(0x1408605A1); // Pressing X to change into reverse/non-reverse duel

Patch SelectDuelPatch1 = Patch(0x140860523, "\x90\x90\x90\x90\x90\x90\x90\x83\x38\x00"); // Allows SelectDuelHook2
Patch SelectDuelPatch2 = Patch(0x140861B32, "\x83\x78\x04\x00\x8B\x10\x90\x90\x90"); // Allows SelectDuelHook3

// Writes to the save file once a campaign duel ends
Hook UnlockNextDuelHook = Hook(0x1408804C1);
Hook UnlockReverseUpdateNonReverseDuelHook = Hook(0x1408802D2);

Patch UnlockNextDuelPatch = Patch(0x1408804C1, "\x90\x90\x90\xEB\x15\x90\x90"); // Allows UnlockNextDuelHook

export class Campaign {
public:
	static void Setup(void)
	{
		ApplyPatches();
		Logger::Log("Campaign patches applied.");

		ApplyHooks();
		Logger::Log("Campaign hooks applied.");

		ReadExtraDuels();
		Logger::Log("Extra campaign duels loaded.");
	}

private:
	static unordered_map<ID, CampaignDuel> duels;

	static ID previous_get_duel_id;

	static void ApplyPatches(void)
	{
		GenerateDuelsPerSeriesTablePatch1.Apply();
		GenerateDuelsPerSeriesTablePatch2.Apply();
		GenerateDuelsPerSeriesTablePatch3.Apply();
		GenerateDuelsPerSeriesTablePatch4.Apply();
		GetDuelCharacterPatch.Apply();
		GetCampaignDuelDeckPatch1.Apply();
		GetCampaignDuelDeckPatch2.Apply();
		UnlockNextDuelPatch.Apply();
		SelectDuelPatch1.Apply();
		SelectDuelPatch2.Apply();
	}

	static void ApplyHooks(void)
	{
		GetDuelAddressHook.Apply(&GetDuelAddress_);
		SelectDuelHook1.Apply(&SelectDuel1_);
		SelectDuelHook2.Apply(&SelectDuel2_);
		SelectDuelHook3.Apply(&SelectDuel3_);
		SelectDuelHook4.Apply(&SelectDuel4_);
		SelectDuelHook5.Apply(&SelectDuel5_);
		UnlockNextDuelHook.Apply(&GetDuelUnlockedAddress_);
		UnlockReverseUpdateNonReverseDuelHook.Apply(&UnlockReverseUpdateNonReverseDuel_);
	}

	static void ReadExtraDuels(void)
	{
		ByteStream extraduels(string(data_folder) + "extraduels.bin");

		if (!extraduels) {
			Logger::Show(Warning("Skipping extra duels because the file \"extraduels.bin\" is missing."));

			return;
		}

		size_t n_duels = extraduels.Read<size_t>();
		duels.reserve(n_duels);

		for (unsigned int index = 0; index < n_duels; ++index) {
			ID id = extraduels.Read<ID>() + 0xE1;
			CampaignDuel& duel = (*duels.insert({ id, CampaignDuel{} }).first).second;
			duel.id = id;

			extraduels.Read(duel.series);
			extraduels.Read(duel.number_in_series);
			extraduels.Read(duel.left_character.id);
			extraduels.Read(duel.right_character.id);
			extraduels.Read(duel.left_character.deck_id);
			extraduels.Read(duel.right_character.deck_id);
			extraduels.Read(duel.arena_id);
			extraduels.Read(duel.reward_pack_id);

			if (duel.reward_pack_id == 0xFFFFFFFF) duel.reward_pack_id = 0;
			if (extraduels.Read<int>() == -1) duel.unknown2 = 1;
			extraduels.Read(duel.unknown26);

			size_t id_name = extraduels.Read<size_t>();
			size_t left_character_alt = extraduels.Read<size_t>();
			size_t right_character_alt = extraduels.Read<size_t>();
			size_t name = extraduels.Read<size_t>();
			/*size_t description = */extraduels.Read<size_t>();
			/*size_t tip = */extraduels.Read<size_t>();

			extraduels.Push();
			extraduels.SeekAndReadString(id_name, duel.id_name);
			extraduels.SeekAndReadString(left_character_alt, duel.left_character.alternate_skin);
			extraduels.SeekAndReadString(right_character_alt, duel.right_character.alternate_skin);
			extraduels.SeekAndReadString(name, duel.name.pointer, duel.name.length, duel.name.size);
			//extraduels.SeekAndReadString(description,);
			//extraduels.SeekAndReadString(tip,);
			extraduels.Pop();
		}
	}

	// Hooks

	__declspec(noinline) static auto* GetDuelUnlockedAddress(ID id)
	{
		auto& campaign = ExtraSave::Campaign();
		string string_id = to_string(id);

		if (!campaign["unlocked_duels"].hasKey(string_id)) {
			campaign["unlocked_duels"][string_id] = 0;
		}

		return &campaign["unlocked_duels"][string_id].Internal.Int;
	}

	__declspec(noinline) static CampaignDuel* GetDuelAddress(ID id)
	{
		previous_get_duel_id = id;

		if (duels.contains(id)) {
			return &duels[id];
		}
		else {
			return GetDuelAddressHook.CallOriginal(&GetDuelAddress)(id);
		}
	}

	__declspec(noinline) static void GetDuelAddress_(void)
	{
		Call(GetDuelAddress);
	}

	__declspec(noinline) static Return* GetDuelUnlockedAddress__(ID id, ID number_in_series, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = UnlockNextDuelHook.Original;

		if (id >= 0xE2) {
			ret->rcx = (Register)GetDuelUnlockedAddress(id);
		}
		else {
			ret->rcx = 3 * number_in_series;
			ret->rip += 0x3 + 0x2;
		}

		return ret;
	}

	__declspec(noinline) static void GetDuelUnlockedAddress_(void)
	{
		MovRcxRdi();
		MovRdxRbx();

		CallEx(GetDuelUnlockedAddress__);
	}

	__declspec(noinline) static Return* SelectDuel1__(Register rcx, ID id, Register rax, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ret->rcx = rcx;
		ret->rdx = id;
		ret->r9 = r9;
		ret->rax = rax;
		ret->rip = SelectDuelHook1.Original;

		if (id >= 0xE2) {
			ret->r8 = (Register)GetDuelUnlockedAddress(id);
			ret->rip += 0x4 + 0x3;
		}

		return ret;
	}

	__declspec(noinline) static void SelectDuel1_(void)
	{
		MovR8Rax();

		CallEx(SelectDuel1__);
	}

	__declspec(noinline) static Return* SelectDuel2__(Register rcx, Register rdx, Register r8, Register r13)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ID id = previous_get_duel_id;

		ret->rcx = rcx;
		ret->rdx = rdx;
		ret->r8 = r8;
		ret->rip = SelectDuelHook2.Original;

		if (id >= 0xE2) {
			ret->rax = (Register)GetDuelUnlockedAddress(id);
		}
		else {
			ret->rax = r13 + (rdx + rcx * 2) * 4 + 0x10;
		}

		return ret;
	}

	__declspec(noinline) static void SelectDuel2_(void)
	{
		MovR9R13();

		CallEx(SelectDuel2__);
	}

	__declspec(noinline) static Return* SelectDuel3__(Register rcx, Register rax, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ID id = previous_get_duel_id;

		ret->rcx = rcx;
		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = SelectDuelHook3.Original;

		if (id >= 0xE2) {
			ret->rax = (Register)GetDuelUnlockedAddress(id);
		}
		else {
			ret->rax = rax + rcx * 8 + 0x10;
		}

		return ret;
	}

	__declspec(noinline) static void SelectDuel3_(void)
	{
		MovRdxRax();

		CallEx(SelectDuel3__);
	}

	__declspec(noinline) static Return* SelectDuel4__(Register rcx, Register rdx, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ID id = previous_get_duel_id;

		ret->rdx = rdx;
		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = SelectDuelHook4.Original;

		if (id >= 0xE2) {
			ret->rax = (Register)GetDuelUnlockedAddress(id) - 0x10;
			ret->rcx = 0;
			ret->rip += 0x4;
		}
		else {
			ret->rcx = rcx;
		}

		return ret;
	}

	__declspec(noinline) static void SelectDuel4_(void)
	{
		CallEx(SelectDuel4__);
	}

	__declspec(noinline) static Return* SelectDuel5__(Register rcx, Register rdx, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ID id = previous_get_duel_id;

		ret->rdx = rdx;
		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = SelectDuelHook5.Original;

		if (id >= 0xE2) {
			ret->rax = (Register)GetDuelUnlockedAddress(id) - 0x10;
			ret->rcx = 0;
			ret->rip += 0x4 + 0x3;
		}
		else {
			ret->rax = rcx * 0x4B8;
			ret->rcx = rcx;
		}

		return ret;
	}

	__declspec(noinline) static void SelectDuel5_(void)
	{
		CallEx(SelectDuel5__);
	}

	__declspec(noinline) static Return* UnlockReverseUpdateNonReverseDuel__(Register rax, Register rdx, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ID id = previous_get_duel_id;

		ret->rdx = rdx;
		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = UnlockReverseUpdateNonReverseDuelHook.Original;

		if (id >= 0xE2) {
			ret->rcx = (Register)GetDuelUnlockedAddress(id);
			ret->rip += 0x4 + 0x3;
		}
		else {
			ret->rax = rax;
		}

		return ret;
	}

	__declspec(noinline) static void UnlockReverseUpdateNonReverseDuel_(void)
	{
		MovRcxRax();

		CallEx(UnlockReverseUpdateNonReverseDuel__);
	}
};

unordered_map<ID, CampaignDuel> Campaign::duels;
ID Campaign::previous_get_duel_id;
