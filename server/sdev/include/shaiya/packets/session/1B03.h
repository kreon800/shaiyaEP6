#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    // CClientToMgr::OnRecv
    // ebx = packet

    #pragma pack(push, 1)
    struct PacketBuffer1B03
    {
        ULONG u0x00;
        void* p0x04;
        ULONG u0x08;
        ULONG u0x0C;
        UserId userId;            // ebx+0x02
        ProductCode productCode;  // ebx+0x06
        CharName targetName;      // ebx+0x1B
        UINT32 itemPrice;         // ebx+0x30
        UINT32 points;            // ebx+0x34
        // 0x46
    };
    #pragma pack(pop)
}
