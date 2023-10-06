#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct BlockAddRequest
    {
        UINT16 opcode{ 0x2209 };
        CharName charName;
    };

    struct BlockAddResponse
    {
        UINT16 opcode{ 0x2209 };
        ULONG charId;
        CharName charName;
    };
    #pragma pack(pop)
}
