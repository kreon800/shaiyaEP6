#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct FriendAddResponse
    {
        UINT16 opcode{ 0x2204 };
        ULONG charId;
        UINT8 unknown;
        CharName charName;
    };
    #pragma pack(pop)
}
