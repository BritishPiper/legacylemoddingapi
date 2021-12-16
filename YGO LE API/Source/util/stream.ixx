module;

#include <fstream>
#include <stack>
#include <vector>

#include "error.h"

export module stream;

using namespace std;

import basictypes; // Byte

export class ByteStream {
public:
    ByteStream(ifstream& file)
    {
        ReadAllBytes(file);
    }

    ByteStream(const char* filename)
    {
        ifstream file(filename, ios::binary);

        ReadAllBytes(file);
    }

    ByteStream(const string& filename) : ByteStream(filename.c_str()) {}

    ByteStream(const wchar_t* filename)
    {
        ifstream file(filename, ios::binary);

        ReadAllBytes(file);
    }

    ByteStream(const wstring& filename) : ByteStream(filename.c_str()) {}

    ByteStream(size_t size)
    {
        bytes = vector<Byte>(size);
        cursor = &bytes[0];
    }

    template <typename Type>
    Type Read(void)
    {
        Type value = *reinterpret_cast<Type*>(cursor);

        CheckEOF<Type>();

        return value;
    }

    template <typename Type>
    void Read(Type& value)
    {
        value = Read<Type>();
    }

    template <typename Character, typename Size>
    Character* ReadFixedString(Size length)
    {
        Character* str;

        str = new Character[length + 1];
        for (Size index = 0; index < length; ++index) {
            str[index] = Read<Character>();
        }
        str[length] = (Character)0x0;

        return str;
    }

    template <typename Character, typename Size>
    void ReadFixedString(const Character*& value, Size length)
    {
        auto [value_, length_] = ReadFixedString<Character, Size>(length);

        value = value_;
        length = length_;
    }

    template <typename Character, typename Size>
    auto ReadString(void)
    {
        Character character;
        Character* str;
        Size length = 1;

        Push();
        while (character = Read<Character>()) {
            ++length;
        }
        Pop();

        str = new Character[length];
        for (Size index = 0; index < length; ++index) {
            str[index] = Read<Character>();
        }

        struct result { const Character* str; Size length; };
        return result{ str, length - 1 };
    }

    template <typename Character, typename Size>
    void ReadString(const Character*& value, Size& length)
    {
        auto [value_, length_] = ReadString<Character, Size>();

        value = value_;
        length = length_;
    }

    void Push(void)
    {
        cursors.push(cursor);
    }

    void Pop(void)
    {
        cursor = cursors.top();
        cursors.pop();
    }

    void Seek(size_t position)
    {
        cursor = (&bytes[0]) + position;

        CheckEOF();
    }

    template <typename Type>
    void SeekAndRead(size_t position, Type& value)
    {
        Seek(position);
        Read(value);
    }

    template <typename Type>
    Type SeekAndRead(size_t position)
    {
        Seek(position);
        return Read<Type>();
    }

    template <typename Character>
    auto SeekAndReadString(size_t position)
    {
        Seek(position);
        return ReadString<Character>();
    }

    template <typename Character, typename Size>
    void SeekAndReadString(size_t position, const Character*& value, Size& length)
    {
        Seek(position);
        ReadString(value, length);
    }

    template <typename Character, typename Size>
    void SeekAndReadString(size_t position, const Character*& value, Size& length, Size& size)
    {
        Seek(position);
        ReadString(value, length);
        size = length + 1;
    }

    template <typename Character>
    void SeekAndReadString(size_t position, const Character*& value)
    {
        size_t length = 0;
        SeekAndReadString(position, value, length);
    }

    operator bool(void) const
    {
        return bytes.size() > 0;
    }

private:
    vector<Byte> bytes;
    Byte* cursor;
    stack<Byte*> cursors;

    void ReadAllBytes(ifstream& file)
    {
        if (file) {
            file.seekg(0, ios::end);
            ifstream::pos_type size = file.tellg();
            file.seekg(0, ios::beg);

            bytes.resize(size);
            cursor = &bytes[0];
            cursors = stack<Byte*>();

            file.read((char*)cursor, size);
        }
    }

    template <typename Type>
    void CheckEOF(void)
    {
        cursor += sizeof(Type);

        CheckEOF();
    }

    void CheckEOF(void)
    {
        if (cursor > &(*bytes.end())) {
            throw Exception("Trying to read past the end of a ByteStream.");
        }
    }
};
