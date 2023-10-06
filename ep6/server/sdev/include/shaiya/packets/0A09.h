#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ExchangeItem
    {
        UINT16 opcode{ 0xA09 };
        UINT8 destSlot;
        UINT8 type;
        UINT8 typeId;
        UINT8 count;
        UINT16 quality;
        #ifdef SHAIYA_EP6
        ULONG fromDate;
        ULONG toDate;
        #endif
        // or CloakBadge
        Gems gems;
        CraftName craftName;
    };
    #pragma pack(pop)
}
