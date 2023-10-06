#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #ifdef SHAIYA_EP6
    constexpr int list_size_0307 = ITEM_LIST_SIZE;
    #else
    constexpr int list_size_0307 = 12;
    #endif

    #pragma pack(push, 1)
    struct Item0307
    {
        UINT8 slot;
        UINT8 type;
        UINT8 typeId;
        UINT16 quality;
        // or CloakBadge
        Gems gems;
        CraftName craftName;
    };

    struct InspectRequest
    {
        UINT16 opcode{ 0x307 };
        ULONG charId;
    };

    struct InspectResponse
    {
        UINT16 opcode{ 0x307 };
        UINT8 itemCount;
        Array<Item0307, list_size_0307> itemList;
    };
    #pragma pack(pop)
}
