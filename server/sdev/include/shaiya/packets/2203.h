#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    enum struct FriendAddResultType : UINT8
    {
        Rejected,
        Accepted,
        NotFound
    };

    #pragma pack(push, 1)
    struct FriendAddResult
    {
        UINT16 opcode{ 0x2203 };
        FriendAddResultType result;
    };
    #pragma pack(pop)
}
