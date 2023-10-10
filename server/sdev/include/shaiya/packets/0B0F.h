#pragma once
#include <include/shaiya/common.h>
#include <include/shaiya/include/CParty.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct RaidSetItemDivTypeRequest
    {
        UINT16 opcode{ 0xB0F };
        ItemDivType itemDivType;
    };

    struct RaidSetItemDivTypeResponse
    {
        UINT16 opcode{ 0xB0F };
        ItemDivType itemDivType;
    };
    #pragma pack(pop)
}
