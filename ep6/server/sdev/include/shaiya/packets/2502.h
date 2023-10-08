#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    enum struct GuildVsGuildResponseType : UINT8
    {
        Rejected,
        Accepted,
        CannotRequest,
        DidNotRespond
    };

    struct GuildVsGuildOutgoingResponse
    {
        UINT16 opcode{ 0x2502 };
        GuildVsGuildResponseType responseType;
    };

    struct GuildVsGuildIncomingResponse
    {
        UINT16 opcode{ 0x2502 };
        GuildVsGuildResponseType responseType;
        CharId targetId;
    };
    #pragma pack(pop)
}
