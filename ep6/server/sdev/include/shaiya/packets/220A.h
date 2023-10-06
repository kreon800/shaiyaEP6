#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct BlockRemoveRequest
    {
        UINT16 opcode{ 0x220A };
        ULONG charId;
    };

    struct BlockRemoveResponse
    {
        UINT16 opcode{ 0x220A };
        ULONG charId;
    };
    #pragma pack(pop)
}
