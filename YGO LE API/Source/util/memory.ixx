module;

#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/ZydisDisassembler.hpp>

#include <memory>

#include "error.h"

export module memory;

using namespace std;

import basictypes; // Address, Byte, Offset, Pointer, SByte

PLH::ZydisDisassembler disassembler = PLH::ZydisDisassembler(PLH::Mode::x64);

export template<typename FunctionA, typename FunctionB>
FunctionB FunctionCast(FunctionA pointer, FunctionB function) {
    return PLH::FnCast(pointer, function);
}

export class Hook {
public:
    Hook(Address address) : address(address), detour(nullptr) {}

    template <typename Function>
    void Apply(Function function)
    {
        detour = unique_ptr<PLH::x64Detour>(new PLH::x64Detour((SByte*)address, (SByte*)function, &original, disassembler));

        if (!detour->hook()) {
            throw MajorError("Failed to apply hook at {:#X}.", address);
        }
    }

    void Remove(void)
    {
        if (detour) {
            detour->unHook();
            detour.reset();
        }
    }

    template<typename Signature>
    Signature CallOriginal(Signature signature, Offset offset = 0x0) {
        return PLH::FnCast(original + offset, signature);
    }

    Address GetOriginal(void) {
        return original;
    }

    __declspec(property(get = GetOriginal)) Address Original;

private:
    Address address;
    unique_ptr<PLH::x64Detour> detour;
    Address original;
};

export class Patch {
public:
    template <size_t patch_length>
    Patch(Address address, const SByte(&patch)[patch_length]) : address(address), size(patch_length - 1)
    {
        original = new Byte[size];
        patched = new Byte[size];

        const Byte* const start = (Byte*)address;

        for (size_t index = 0; index < size; ++index) {
            original[index] = *(start + index);
            patched[index] = patch[index];
        }
    }

    ~Patch(void)
    {
        delete[] original;
        delete[] patched;
    }

    void Apply(void)
    {
        Overwrite(patched);
    }

    void Remove(void)
    {
        Overwrite(original);
    }

private:
    Address address;
    Byte* original;
    Byte* patched;
    size_t size;

    void Overwrite(const Byte* source)
    {
        DWORD protection;
        Pointer pointer = (Pointer)address;

        VirtualProtectEx(GetCurrentProcess(), pointer, size, PAGE_EXECUTE_READWRITE, &protection);
        memcpy_s(pointer, size, source, size);
        VirtualProtectEx(GetCurrentProcess(), pointer, size, protection, nullptr);
    }
};
