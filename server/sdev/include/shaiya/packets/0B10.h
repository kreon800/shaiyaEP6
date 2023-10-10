#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct RaidSetAutoJoinRequest
    {
        UINT16 opcode{ 0xB10 };
        bool isAutoJoin;
    };

    struct RaidSetAutoJoinResponse
    {
        UINT16 opcode{ 0xB10 };
        bool isAutoJoin;
    };
    #pragma pack(pop)
}
