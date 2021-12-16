module;

#include <string>
#include <unordered_map>

#include "../globals.h"
#include "../util/assembly.h"
#include "../util/error.h"

export module character;

using namespace std;

import basictypes;
import campaign;
import log;
import save;
import stream;

#pragma pack(push, 1)
export struct Character { // Size: 0x68
	ID id; // 0x0
	CampaignSeries series; // 0x4
	ID deck_id; // 0x8
	Bool playable; // 0xC
	DLC dlc; // 0x10
	ID arena_id; // 0x14
	int unknown1; // 0x18
	int unknown2; // 0x1C
	String id_name; // 0x20
	GameString name; // 0x28
	GameString bio; //0x48

	~Character(void)
	{
		if (id_name) {
			delete[] id_name;
		}

		if (name.pointer) {
			delete[] name.pointer;
		}

		if (bio.pointer) {
			delete[] bio.pointer;
		}
	}
};
#pragma pack(pop)

// Called to show the opponent's deck at the player choose deck screen when starting a challenge duel
Patch ChooseChallengeDeckPatch = Patch(0x1408417AD, "\x90\x90\x48\x98\x89\xC1\xE8\xE6\xD4\xFB\xFF\x48\x89\xC1\x90\x90\x90\x90");

// Called when generating the bust image of a character. Usually it saved it in memory to be used later, but not anymore. It is now reloaded every time it's required.
Patch GenerateBustImagePatch1 = Patch(0x1407FE85A, "\x90\x90\x90\x90\x90\x90\x48\x63\xDA\x48\x83\x3C\xD9\x00\x90\x90\x90\x90\x90\x90\x48\x89\xD9\xE8\x28\x04\x00\x00\x49\x89\xC0\x90\x90\x90"); // Remove the hard 0xF0 limit
Patch GenerateBustImagePatch2 = Patch(0x1407FE8AE, "\x48\x89\xC3\x90"); // Don't save in memory busts.zip image, since it would overwrite after the static memory
Patch GenerateBustImagePatch3 = Patch(0x1407FE8F2, "\x48\x89\xD8\x90"); // Instead, save a pointer in memory

// Called when starting a challenge duel to get the characters' decks
Patch GetChallengeDuelDecksPatch1 = Patch(0x140840C59, "\x48\x98\x48\x89\xC1\xE8\x3B\xE0\xFB\xFF\x48\x89\xC1\x90\x90\x90");
Patch GetChallengeDuelDecksPatch2 = Patch(0x140840C8F, "\x48\x89\xD9\xE8\x07\xE0\xFB\xFF\x90");

// These functions do not exist, but are created using unused code inside other functions
Hook GetAvatarHook = Hook(0x1407FEAF8); // Inside GetCharacterIdName
Hook GetCharacterHook = Hook(0x1407FEC9E); // Inside GetCharacterName

Patch GetAvatarPatch = Patch(0x1407FEAF8, "\x90\x90\x90\x90\x90\x90\x90"); // Allows GetAvatarHook
Patch GetCharacterNamePatch = Patch(0x1407FEC9E, "\x90\x90\x90\x90\x90\x90\x90"); // Allows GetCharacterHook

// Gets the character id name, used, for example, to get the images for the characters
Hook GetCharacterIdNameHook = Hook(0x1407FEAF0);

// Gets whether the character has a valid id name
Hook GetCharacterHasIdNameHook = Hook(0x1407FED00);

// Gets the shown name of the character
Hook GetCharacterNameHook = Hook(0x1407FEC70);

// Gets if a character is playable
Hook GetCharacterPlayableHook = Hook(0x1407FED30);

// Gets the %s_neutral resource for a specific character, where %s is the id name
Patch GetCharacterNeutralPatch = Patch(0x1407FEC1F, "\x89\xD1\xE8\xCA\xFE\xFF\xFF\x90\x90\x90\x90\x90\x90\x90\x90\x90\x49\x89\xC0\x48\x89\xD9\x90\x90\x90");

// Loads the list of challenges when opening the challenge screen
Patch LoadChallengesPatch1 = Patch(0x14084194F, "\x48\x8D\x3D\x1A\x1B\x0D\x02\x44\x8B\xF8\x48\x83\xC7\x10\x90\x90"); // Fix rdi displacement
Patch LoadChallengesPatch2 = Patch(0x140841977, "\x92"); // End loop on the first character without name
Patch LoadChallengesPatch3 = Patch(0x1408419AF, "\x90\x90"); // Enable multiple challenges to use the same deck
Patch LoadChallengesPatch4 = Patch(0x1408419FD, "\x89\xD9\xE8\x9A\xD2\xFB\xFF\x48\x89\xC7\xE9\x4D\xFF\xFF\xFF\x90"); // Gets the next character and make the infinite loop until character with no name and load current Character* into rdi

// Removes the limit of 0xF0 avatars when loading them at the deck edit screen
Patch LoadDeckAvatarsPatch1 = Patch(0x1408A6173, "\x89"); // Make CharacterHasIdName first rather than CharacterPlayable
Patch LoadDeckAvatarsPatch2 = Patch(0x1408A617A, "\x6B"); // End loop on the first character without name
Patch LoadDeckAvatarsPatch3 = Patch(0x1408A617E, "\xAE"); // Make CharacterPlayable second rather than CharacterHasIdName
Patch LoadDeckAvatarsPatch4 = Patch(0x1408A618C, "\x90\x90");
Patch LoadDeckAvatarsPatch5 = Patch(0x1408A61E4, "\xEB"); // Infinite loop until character with no name

//////////////////////////////// Below must review
// Give Avatar
Patch RewardScreenShowPatch1 = Patch(0x1408A5378, "\x84"); // Make CharacterHasIdName first rather than CharacterPlayable
Patch RewardScreenShowPatch2 = Patch(0x1408A5380, "\xA3"); // End loop on the first character without name
Patch RewardScreenShowPatch3 = Patch(0x1408A5387, "\xA5"); // Make CharacterPlayable second rather than CharacterHasIdName
Patch RewardScreenShowPatch4 = Patch(0x1408A5399, "\x90\x90"); // ????? NeedToGetAvatarUnlocked for rdi right below it
Patch RewardScreenShowPatch5 = Patch(0x1408A53BD, "\x90\x90"); // ????? NeedToGetCharacter*
Patch RewardScreenShowPatch6 = Patch(0x1408A5421, "\xE9\x4F\xFF\xFF\xFF\x90"); // Infinite loop until character with no name
//////////////////////////// Above must review

// SavedMiscData = Save + (4056 = 0xfd8)
// Unlocked Avatars = SavedMiscData + 0x18

// Gets if an avatar is unlocked to be used in the duel edit screen
Hook LoadUnlockedAvatarsHook = Hook(0x1408A618E);

Patch LoadUnlockedAvatarsPatch = Patch(0x1408A6193, "\x85\xF0\x90\x90\x90"); // Allows LoadUnlockedAvatarsHook

// Writes to the save file once a campaign duel ends
Hook RewardAvatarHook1 = Hook(0x1408807BF); // Checks if already unlocked
Hook RewardAvatarHook2 = Hook(0x1408807E4); // Unlock it

Patch RewardAvatarPatch1 = Patch(0x1408807BB, "\x90\x90\x90\x90\x48\x89\xC0\xEB\x08\x90\x90\x90\x43\x8B\x44\x81\x18\x90\x90\x90\x90\x90\x90"); // Allows RewardAvatarHook1
Patch RewardAvatarPatch2 = Patch(0x1408807E1, "\x4C\x89\xD1\x48\x89\xD0\xEB\x0E\x90"); // Allows RewardAvatarHook2
Patch RewardAvatarPatch3 = Patch(0x1408809D6, "\x48\x89\xC1\xE8\xC0\xE2\xF7\xFF\x48\x89\xC2\x4C\x8D\x62\x10\x90\x90\x90\x48\x8D\x42\x08\x90\x90\x90"); // Calls to GetCharacter inside the reward routine

export class Characters {
public:
	static void Setup(void)
	{
		Logger::Log("Setting up for extra characters.");

		ApplyPatches();
		Logger::Log("Characters patches applied.");

		ApplyHooks();
		Logger::Log("Characters hooks applied.");

		ReadExtraCharacters();
		Logger::Log("Extra characters loaded.");
	}

private:
	static unordered_map<ID, Character> characters;

	static void ApplyHooks(void)
	{
		// Existing functions
		LoadUnlockedAvatarsHook.Apply(&EditDeck_);
		GetCharacterIdNameHook.Apply(&GetCharacterIdName_);
		GetCharacterHasIdNameHook.Apply(&GetCharacterHasIdName_);
		GetCharacterNameHook.Apply(&GetCharacterName_);
		GetCharacterPlayableHook.Apply(&GetCharacterPlayable_);
		RewardAvatarHook1.Apply(&RewardAvatar1_);
		RewardAvatarHook2.Apply(&RewardAvatar2_);

		// Added functions
		GetAvatarHook.Apply(&GetAvatar_);
		GetCharacterHook.Apply(&GetCharacter_);
	}

	static void ApplyPatches(void)
	{
		ChooseChallengeDeckPatch.Apply();
		GenerateBustImagePatch1.Apply();
		GenerateBustImagePatch2.Apply();
		GenerateBustImagePatch3.Apply();
		GetChallengeDuelDecksPatch1.Apply();
		GetChallengeDuelDecksPatch2.Apply();
		GetAvatarPatch.Apply();
		GetCharacterNamePatch.Apply();
		GetCharacterNeutralPatch.Apply();
		LoadChallengesPatch1.Apply();
		LoadChallengesPatch2.Apply();
		LoadChallengesPatch3.Apply();
		LoadChallengesPatch4.Apply();
		LoadDeckAvatarsPatch1.Apply();
		LoadDeckAvatarsPatch2.Apply();
		LoadDeckAvatarsPatch3.Apply();
		LoadDeckAvatarsPatch4.Apply();
		LoadDeckAvatarsPatch5.Apply();
		LoadUnlockedAvatarsPatch.Apply();
		RewardAvatarPatch1.Apply();
		RewardAvatarPatch2.Apply();
		RewardAvatarPatch3.Apply();
		RewardScreenShowPatch1.Apply();
		RewardScreenShowPatch2.Apply();
		RewardScreenShowPatch3.Apply();
		RewardScreenShowPatch4.Apply();
		RewardScreenShowPatch5.Apply();
		RewardScreenShowPatch6.Apply();
	}

	static void ReadExtraCharacters(void)
	{
		ByteStream extrachars(string(data_folder) + "extrachars.bin");

		if (!extrachars) {
			Logger::Show(Warning("Skipping extra characters because the file \"extrachars.bin\" is missing."));

			return;
		}

		size_t n_chars = extrachars.Read<size_t>();
		characters.reserve(n_chars);

		for (unsigned int index = 0; index < n_chars; ++index) {
			ID id = extrachars.Read<ID>() + 0xEF;
			Character& character = (*characters.insert({ id, Character{} }).first).second;
			character.id = id;

			extrachars.Read(character.series);
			extrachars.Read(character.deck_id);
			extrachars.Read(character.playable);

			extrachars.Read(character.dlc);
			if (character.dlc == -1) {
				character.dlc = 1;
			}

			extrachars.Read<int>(); // field 6: The game reads this but immediately discards it
			extrachars.Read(character.arena_id);
			extrachars.Read<int>(); // field 8

			size_t id_name = extrachars.Read<size_t>();
			size_t name = extrachars.Read<size_t>();
			/*size_t bio = */extrachars.Read<size_t>();

			extrachars.Push();
			extrachars.SeekAndReadString(id_name, character.id_name);
			extrachars.SeekAndReadString(name, character.name.pointer, character.name.length, character.name.size);
			extrachars.SeekAndReadString(name, character.bio.pointer, character.bio.length, character.bio.size);
			extrachars.Pop();
		}
	}

	// extrachars.bin Hooks

	__declspec(noinline) static Character* GetCharacter(ID id)
	{
		if (characters.contains(id)) {
			return &characters[id];
		}
		else {
			return (Character*)(0x142913470 + sizeof(Character) * id);
		}
	}

	__declspec(noinline) static String GetCharacterIdName(ID id)
	{
		if (characters.contains(id)) {
			return characters[id].id_name;
		}
		else {
			return GetCharacterIdNameHook.CallOriginal(&GetCharacterIdName)(id);
		}
	}

	__declspec(noinline) static Bool GetCharacterHasIdName(ID id)
	{
		if (id == 0) {
			return 1; // necessary since iterating on LoadChallenges or UnlockRecipe start at index 0
		}
		else if (characters.contains(id)) {
			return characters[id].id_name == nullptr ? 0 : 1;
		}
		else {
			return GetCharacterHasIdNameHook.CallOriginal(&GetCharacterHasIdName)(id);
		}
	}

	__declspec(noinline) static WString GetCharacterName(ID id)
	{
		if (characters.contains(id)) {
			Character& character = characters[id];

			if (character.unknown1 < 8) {
				return characters[id].name.pointer;
			}
			else {
				return nullptr;
			}
		}
		else if (id >= 0xF0) {
			return nullptr;
		}
		else {
			return GetCharacterNameHook.CallOriginal(&GetCharacterName)(id);
		}
	}

	__declspec(noinline) static Bool GetCharacterPlayable(ID id)
	{
		if (characters.contains(id)) {
			return characters[id].playable;
		}
		else if (id >= 0xF0) {
			return 0;
		}
		else {
			return GetCharacterPlayableHook.CallOriginal(&GetCharacterPlayable)(id);
		}
	}

	// Save/Load Hooks

	__declspec(noinline) static int GetAvatar(ID id)
	{
		auto& character = ExtraSave::Character();
		string string_id = to_string(id);

		if (character.hasKey(string_id)) {
			return character[string_id].ToBool() ? 0xFFFFFFF : 0x0;
		}

		character[string_id] = false;

		return 0x0;
	}

	__declspec(noinline) static void UnlockAvatar(ID id)
	{
		string string_id = to_string(id);

		ExtraSave::Character()[string_id] = true;

		Logger::LogDelayed(string("Avatar unlocked for character ") + string_id + ".");
	}

	__declspec(noinline) static void GetAvatar_(void)
	{
		Call(GetAvatar);
	}

	__declspec(noinline) static void GetCharacter_(void)
	{
		Call(GetCharacter);
	}

	__declspec(noinline) static void GetCharacterIdName_(void)
	{
		Call(GetCharacterIdName);
	}

	__declspec(noinline) static void GetCharacterHasIdName_(void)
	{
		Call(GetCharacterHasIdName);
	}

	__declspec(noinline) static void GetCharacterName_(void)
	{
		Call(GetCharacterName);
	}

	__declspec(noinline) static void GetCharacterPlayable_(void)
	{
		Call(GetCharacterPlayable);
	}
	__declspec(noinline) static Return* RewardAvatar1__(Register rcx, [[maybe_unused]] Register rdx, ID id, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ret->rcx = rcx;
		ret->rdx = 1ll << rcx;
		ret->r8 = id >> 5;
		ret->r9 = r9;
		ret->rip = RewardAvatarHook1.Original;

		if (id >= 0xF0) {
			ret->rax = GetAvatar(id);
		}
		else {
			ret->rip += 0x3 + 0x2;
		}

		return ret;
	}

	__declspec(noinline) static void RewardAvatar1_(void)
	{
		CallEx(RewardAvatar1__);
	}

	__declspec(noinline) static Return* RewardAvatar2__(ID id, Register rdx, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ret->rcx = id & 0x1F;
		ret->rdx = rdx;
		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = RewardAvatarHook2.Original;

		if (id >= 0xF0) {
			UnlockAvatar(id);
		}
		else {
			ret->rip += 0x3 + 0x2;
		}

		return ret;
	}

	__declspec(noinline) static void RewardAvatar2_(void)
	{
		CallEx(RewardAvatar2__);
	}

	__declspec(noinline) static Return* EditDeck__(ID id, Register r15, Register r8, Register r9)
	{
		Return* ret = (Return*)_malloca(sizeof(Return));

		ret->r8 = r8;
		ret->r9 = r9;
		ret->rip = LoadUnlockedAvatarsHook.Original + 0x3 + 0x2;

		if (id >= 0xF0) {
			ret->rax = GetAvatar(id);
		}
		else {
			ret->rax = *(Bool*)(r15 + (id >> 5) * 4 + 0x18);
		}

		return ret;
	}

	__declspec(noinline) static void EditDeck_(void)
	{
		MovRcxRbx();
		MovRdxR15();

		CallEx(EditDeck__);
	}
};

unordered_map<ID, Character> Characters::characters;
