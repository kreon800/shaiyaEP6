#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct Item0711
    {
        UINT8 slot;
        UINT8 type;
        UINT8 typeId;
        UINT16 quality;
        // or CloakBadge
        Gems gems;
        UINT8 count;
        #ifdef SHAIYA_EP6
        ULONG fromDate;
        ULONG toDate;
        #endif
        CraftName craftName;
    };

    struct StoredItemList
    {
        UINT16 opcode{ 0x711 };
        UINT32 bankMoney;
        UINT8 itemCount;
        #ifdef SHAIYA_EP6
        Array<Item0711, 40> itemList;
        #else
        Array<Item0711, 50> itemList;
        #endif
    };
    #pragma pack(pop)
}
