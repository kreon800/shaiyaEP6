#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    // g_nPayLetterEnable 0x58799C

    // CClientToMgr::OnRecv
    // ebx = packet

    #pragma pack(push, 1)
    struct PayLetterEnable
    {
        UINT16 opcode{ 0x105 };
        bool enable;
    };

    // CClientToMgr::OnRecv handles this packet at address 0x4069CB
    // the result is g_nPayLetterEnable being set to false because the PayLetter server is not implemented
    struct PacketBuffer0105
    {
        ULONG u0x00;
        void* p0x04;
        ULONG u0x08;
        ULONG u0x0C;
        bool enable;  // ebx+0x02
        // 0x11
    };
    #pragma pack(pop)
}
