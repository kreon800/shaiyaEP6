#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct GuildVsGuildOutgoingRequest
    {
        UINT16 opcode{ 0x2501 };
        CharId senderId;
    };

    struct GuildVsGuildIncomingRequest
    {
        UINT16 opcode{ 0x2501 };
        CharId senderId;
        CharId targetId;
    };
    #pragma pack(pop)
}
