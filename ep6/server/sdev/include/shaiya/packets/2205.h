#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct FriendRemoveRequest
    {
        UINT16 opcode{ 0x2205 };
        ULONG charId;
    };

    struct FriendRemoveResponse
    {
        UINT16 opcode{ 0x2205 };
        ULONG charId;
    };
    #pragma pack(pop)
}
