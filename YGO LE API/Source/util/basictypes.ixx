export module basictypes;

export using Byte = unsigned char;
export using Word = unsigned short;
export using Dword = unsigned long;
export using Qword = unsigned long long;

export using SByte = char;
export using SWord = short;
export using SDword = long;
export using SQword = long long;

export using Address = Qword;
export using Bool = Dword;
export using Offset = Qword;
export using Pointer = void*;
export using String = const char*;
export using WString = const wchar_t*;

export using Undefined1 = Byte;
export using Undefined2 = Word;
export using Undefined4 = Dword;
export using Undefined8 = Qword;

export using DLC = Dword;
export using ID = Dword;
export using Length = size_t;
#pragma pack(push, 1)
export struct GameString { // Size: 0x20
	WString pointer; // 0x0
	int unknown3; // 0x8
	int unknown4; // 0xC
	Length length; // 0x10
	Length size; // 0x18
};
#pragma pack(pop)
