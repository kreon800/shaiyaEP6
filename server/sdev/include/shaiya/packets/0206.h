#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct UserItemDropIncoming
    {
        UINT16 opcode{ 0x206 };
        UINT8 bag;
        UINT8 slot;
        UINT8 count;
    };

    struct UserItemDropOutgoing
    {
        UINT16 opcode{ 0x206 };
        UINT8 bag;
        UINT8 slot;
        UINT8 type;
        UINT8 typeId;
        UINT8 count;
    };
    #pragma pack(pop)
}
