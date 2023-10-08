#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ItemAcquired
    {
        UINT16 opcode{ 0x205 };
        UINT8 bag;
        UINT8 slot;
        UINT8 type;
        UINT8 typeId;
        UINT8 count;
        UINT16 quality;
        // or CloakBadge
        Gems gems;
        // or '\0'
        CraftName craftName;
    };
    #pragma pack(pop)
}