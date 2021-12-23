module;

#include <filesystem>
#include <fstream>
#include <vector>

#include <Windows.h>

#include "../globals.h"
#include "../util/assembly.h"

export module arena;

using namespace std;

import basictypes;
import log;

#pragma pack(push, 1)
struct Arena { // Size: 0x10
	Qword id; // 0x0
	const char* name; // 0x8
};
#pragma pack(pop)

// Reads bytes from a file from the .dat/.toc
Hook BustAndDecksFromZibHook = Hook(0x140766C50); // busts.zib and decks.zib
Hook GetDatBytesHook = Hook(0x140874E20);
Hook IllustrationFromZibHook = Hook(0x14086D050); // 2020.full.illust_j.jpg.zib
Hook PackFromZibHook = Hook(0x140766C80); // packs.zib

Patch ProcessArenasPatch = Patch(0x14076F246, "\x48\x8B\x1D\x43\x30\x2E\x00"); // Changes the original arena array into our own array

export class Arenas {
public:
	static void Setup(void)
	{
		ApplyPatches();
		Logger::Log("Arenas patches applied.");

		ApplyHooks();
		Logger::Log("Arenas hooks applied.");

		ReadExtraArenas();
		Logger::Log("Extra arenas loaded.");
	}

private:
	static void ApplyPatches(void)
	{
		ProcessArenasPatch.Apply();
	}

	static void ApplyHooks(void)
	{
		BustAndDecksFromZibHook.Apply(&BustAndDecksFromZib_);
		GetDatBytesHook.Apply(&GetDatBytes_);
		IllustrationFromZibHook.Apply(&IllustrationFromZib_);
		PackFromZibHook.Apply(&PackFromZib_);
	}

	static constexpr int max_arenas = 0xFF;

	static Arena arenas[max_arenas];

	struct ZibData {
		void* pointer;
		size_t size;
	};

	__declspec(noinline) static bool PackFromZib_(Register rcx, const char* name, void** buffer, size_t* size)
	{
		error_code filesystem_code;
		const char* actual_name;

		if (*(name + 0x18) < 0x10) // string small enough to be used without pointer
		{
			actual_name = name;
		}
		else // pointer to string
		{
			actual_name = *(char**)name;
		}

		string filename = string(overwrite_folder) + "packs.zib\\" + actual_name;

		if (!filesystem::is_directory(filename) && filesystem::exists(filename, filesystem_code))
		{
			Logger::Log("Overwriting Pack {}.", filename);

			ifstream ifs(filename, ios::binary | ios::ate);
			ifstream::pos_type pos = ifs.tellg();

			SByte* pointer;

			if (pos == 0) {
				pointer = new SByte[0];
			}
			else
			{
				pointer = new SByte[pos];

				ifs.seekg(0, std::ios::beg);
				ifs.read(&pointer[0], pos);
			}

			*buffer = pointer;
			*size = (size_t)pos;

			return true;
		}
		else
		{
			return PackFromZibHook.CallOriginal(&PackFromZib_)(rcx, name, buffer, size);
		}
	}

	__declspec(noinline) static ZibData* BustAndDecksFromZib_(Register rcx, const char* name)
	{
		error_code filesystem_code;
		const char* actual_name;

		if (*(name + 0x18) < 0x10) // string small enough to be used without pointer
		{
			actual_name = name;
		}
		else // pointer to string
		{
			actual_name = *(char**)name;
		}

		string filename = string(overwrite_folder) + "busts.zib\\" + + actual_name;

		if (!filesystem::is_directory(filename) && filesystem::exists(filename, filesystem_code))
		{
			Logger::Log("Overwriting Bust {}.", filename);

			ifstream ifs(filename, ios::binary | ios::ate);
			ifstream::pos_type pos = ifs.tellg();

			SByte* pointer;

			if (pos == 0) {
				pointer = new SByte[0];
			}
			else
			{
				pointer = new SByte[pos];

				ifs.seekg(0, std::ios::beg);
				ifs.read(&pointer[0], pos);
			}

			ZibData* res = new ZibData{ pointer, (size_t)pos };

			return res;
		}
		
		filename = string(overwrite_folder) + "decks.zib\\" + +actual_name;

		if (!filesystem::is_directory(filename) && filesystem::exists(filename, filesystem_code))
		{
			Logger::Log("Overwriting Deck {}.", filename);

			ifstream ifs(filename, ios::binary | ios::ate);
			ifstream::pos_type pos = ifs.tellg();

			SByte* pointer;

			if (pos == 0) {
				pointer = new SByte[0];
			}
			else
			{
				pointer = new SByte[pos];

				ifs.seekg(0, std::ios::beg);
				ifs.read(&pointer[0], pos);
			}

			ZibData* res = new ZibData{ pointer, (size_t)pos };

			return res;
		}

		return BustAndDecksFromZibHook.CallOriginal(&BustAndDecksFromZib_)(rcx, name);
	}

	__declspec(noinline) static bool IllustrationFromZib_(Register rcx, ID id, void** buffer, size_t* size)
	{
		error_code filesystem_code;
		string filename = string(overwrite_folder) + "2020.full.illust_j.jpg.zib\\" + std::to_string(id) + ".jpg";

		if (!filesystem::is_directory(filename) && filesystem::exists(filename, filesystem_code))
		{
			Logger::Log("Overwriting Illustration {}.", filename);

			ifstream ifs(filename, ios::binary | ios::ate);
			ifstream::pos_type pos = ifs.tellg();

			SByte* pointer;

			if (pos == 0) {
				pointer = new SByte[0];
			}
			else
			{
				pointer = new SByte[pos];

				ifs.seekg(0, std::ios::beg);
				ifs.read(&pointer[0], pos);
			}

			*buffer = pointer;
			*size = (size_t)pos;;

			return true;
		}
		else
		{
			return IllustrationFromZibHook.CallOriginal(&IllustrationFromZib_)(rcx, id, buffer, size);
		}
	}

	__declspec(noinline) static size_t GetDatBytes_(const char* name, SByte** data, bool flag1, size_t size)
	{
		error_code filesystem_code;

		const char* actual_name;

		if (*(name + 0x18) < 0x10) // string small enough to be used without pointer
		{
			actual_name = name;
		}
		else // pointer to string
		{
			actual_name = *(char**)name;
		}

		string filename = string(overwrite_folder) + actual_name;

		if (!filesystem::is_directory(filename) && filesystem::exists(filename, filesystem_code))
		{
			Logger::Log("Overwriting File {}.", actual_name);

			ifstream ifs(filename, ios::binary | ios::ate);
			ifstream::pos_type pos = ifs.tellg();

			SByte* pointer;

			if (pos == 0) {
				pointer = new SByte[0];
			}
			else
			{
				pointer = new SByte[pos];

				ifs.seekg(0, std::ios::beg);
				ifs.read(&pointer[0], pos);
			}

			*data = pointer;

			return (size_t)pos;
		}
		else
		{
			return GetDatBytesHook.CallOriginal(&GetDatBytes_)(name, data, flag1, size);
		}
	}

	static void ReadExtraArenas(void)
	{
		int latest = 14;
		memcpy(&arenas, (Pointer)0x140A52290, latest * sizeof(Arena));

		DWORD protection;
		VirtualProtectEx(GetCurrentProcess(), (void*)0x140A52290, sizeof(Arena*), PAGE_EXECUTE_READWRITE, &protection);
		*((Arena**)0x140A52290) = &arenas[0];
		VirtualProtectEx(GetCurrentProcess(), (void*)0x140A52290, sizeof(Arena*), protection, nullptr);

		ifstream file(string(data_folder) + "extraarenas.txt", ios::in);

		if (file)
		{
			int id;
			string name;

			while (file >> id >> name)
			{
				if (latest >= max_arenas) break;
				//Logger::Log("Arena {} = {}.", id, name.c_str());

				if (id < max_arenas)
				{
					char* stored_name = new char[name.size() + 1];

					strcpy_s(stored_name, name.size() + 1, name.c_str());

					arenas[latest].id = id;
					arenas[latest++].name = stored_name;
				}
			}

			file.close();
		}
		else
		{
			Logger::Log("Failed to open \"data/extraarenas.txt\".");
		}
	}
};

Arena Arenas::arenas[Arenas::max_arenas];
