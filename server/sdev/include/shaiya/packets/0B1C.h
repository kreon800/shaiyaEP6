#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct RaidMiniMapPing
    {
        UINT16 opcode{ 0xB1C };
        UINT32 x;
        UINT32 y;
        UINT32 unknown;
    };
    #pragma pack(pop)
}
