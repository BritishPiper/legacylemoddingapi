#include <filesystem>
#include <fstream>
#include <iostream>

#include <Windows.h>

using namespace std;

#define FAILURE __COUNTER__
#define SUCCESS 0

void ShowMessage(const wchar_t* message)
{
	MessageBox(NULL, message, L"YGO Mod Starter", MB_ICONERROR | MB_OK);
}

constexpr const char plugin_manager_dll[] = "YGO Plugin Manager.dll";

int main(void)
{
	error_code filesystem_code;

	if (!filesystem::exists(plugin_manager_dll, filesystem_code)) {
		ShowMessage(L"YGO Plugin Manager.dll does not exist in the current folder.");

		return FAILURE;
	}
	else { // Fix unaligned accesses
		fstream ifs(plugin_manager_dll, ios::in | ios::out | ios::binary | ios::ate);
		fstream::pos_type pos = ifs.tellg();

		vector<char> v;

		if (pos == 0) {
			v = vector<char>();
		}
		else
		{
			v = vector<char>(pos);

			ifs.seekg(0, std::ios::beg);
			ifs.read(&v[0], pos);
		}

		// 0F 28 44 24 20 66 0F 7F 44 24 40
		const char* aligned_code = "\x0F\x28\x44\x24\x20\x66\x0F\x7F\x44\x24\x40";
		auto it = std::search(v.begin(), v.end(), aligned_code, aligned_code + strlen(aligned_code));

		if (it != v.end()) {
			cout << "Found one" << endl;

			*(it + 1) = '\x10'; // movaps xmm0, xmmword ptr ss:[rsp+0x20] -> movups xmm0, xmmword ptr ss:[rsp+0x20]
			*(it + 5) = '\xF3'; // movdqa xmmword ptr ss:[rsp+0x40], xmm0 -> movdqu xmmword ptr ss:[rsp+0x40], xmm0
		}

		it = std::search(it, v.end(), aligned_code, aligned_code + strlen(aligned_code));

		if (it != v.end()) {
			cout << "Found two" << endl;

			*(it + 1) = '\x10'; // movaps xmm0, xmmword ptr ss:[rsp+0x20] -> movups xmm0, xmmword ptr ss:[rsp+0x20]
			*(it + 5) = '\xF3'; // movdqa xmmword ptr ss:[rsp+0x40], xmm0 -> movdqu xmmword ptr ss:[rsp+0x40], xmm0
		}

		ifs.seekg(0, std::ios::beg);
		ifs.write(v.data(), v.size());

		ifs.close();
	}
}