#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct NameAvailableRequest
    {
        UINT16 opcode{ 0x119 };
        Array<char, 19> name;
    };

    struct NameAvailableResponse
    {
        UINT16 opcode{ 0x119 };
        bool available;
    };
    #pragma pack(pop)
}
