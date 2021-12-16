module;

#include <cstdint>

export module assembly;

export namespace assembly {
    // For functions that return
    extern "C" void SetupShadowSpace(void);
    extern "C" void ClearShadowSpace(void);

    // For functions that jump back
    extern "C" void SetupShadowSpaceEx(void);
    extern "C" void ClearShadowSpaceEx(void);

    extern "C" void ClearShadowSpaceEx2(void);

    using Register = uint64_t;

    extern "C" Register MovRcxRax(void);
    extern "C" Register MovRcxRbx(void);
    extern "C" Register MovRcxRdi(void);

    extern "C" Register MovRdxRax(void);
    extern "C" Register MovRdxRbx(void);
    extern "C" Register MovRdxR15(void);

    extern "C" Register MovR8Rax(void);
    extern "C" Register MovR8Rdi(void);

    extern "C" Register MovR9R13(void);

#pragma pack(push, 1)
    struct Return {
        // Returning address
        Register rip;

        // Function arguments
        Register rcx;
        Register rdx;
        Register r8;
        Register r9;

        // Return value
        Register rax;
    };
#pragma pack(pop)
}
