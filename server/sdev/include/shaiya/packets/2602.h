#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    enum struct ProductPurchaseResult : UINT8
    {
        Success,
        // or g_bDisableShop == true
        ProductInfoIsNull,
        InsufficientCash,
        InventoryFull
    };

    struct ProductPurchaseRequest
    {
        UINT16 opcode{ 0x2602 };
        ProductCode productCode;
    };

    struct Item2602
    {
        UINT8 bag;
        UINT8 slot;
        UINT8 type;
        UINT8 typeId;
        UINT8 count;
        #ifdef WITH_ITEM_DURATION
        ULONG fromDate;
        ULONG toDate;
        #endif
    };

    struct ProductPurchaseResponse
    {
        UINT16 opcode{ 0x2602 };
        ProductPurchaseResult result;
        UINT32 points;
        ProductCode productCode;
        ULONG purchaseDate;
        UINT32 itemPrice;
        UINT8 itemCount;
        // size = itemCount (not 24)
        Array<Item2602, 24> itemList;
    };
    #pragma pack(pop)
}
