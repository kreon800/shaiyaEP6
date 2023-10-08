#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    enum struct BattleResponseType : UINT8
    {
        Rejected,
        Accepted,
        CannotRequest,
        DidNotRespond
    };

    struct BattleOutgoingResponse
    {
        UINT16 opcode{ 0x2402 };
        BattleResponseType responseType;
    };

    struct BattleIncomingResponse
    {
        UINT16 opcode{ 0x2402 };
        BattleResponseType responseType;
        CharId targetId;
    };
    #pragma pack(pop)
}
