#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ZoneLeaveItemOutgoing
    {
        UINT16 opcode{ 0x402 };
        ULONG zoneItemId;
    };
    #pragma pack(pop)
}
