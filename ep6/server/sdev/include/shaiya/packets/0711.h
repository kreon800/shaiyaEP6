#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #ifdef SHAIYA_EP6
    constexpr int list_size_0711 = 40;
    #else
    constexpr int list_size_0711 = 50;
    #endif

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
        Array<Item0711, list_size_0711> itemList;
    };
    #pragma pack(pop)
}
