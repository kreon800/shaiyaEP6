#pragma once
#include <sdev/include/shaiya/common.h>

// CUser::PacketUserItem

namespace shaiya
{
    #pragma pack(push, 1)
    struct UserItemRemoveIncoming
    {
        UINT16 opcode{ 0x702 };
        UserId userId;
        UINT8 bag;
        UINT8 slot;
        UINT8 count;
    };

    struct UserItemCraftNameIncoming
    {
        UINT16 opcode{ 0x717 };
        UserId userId;
        UINT8 bag;
        UINT8 slot;
        CraftName craftName;
    };
    #pragma pack(pop)
}