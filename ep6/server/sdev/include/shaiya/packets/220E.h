#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    enum struct BlockAddResultType : UINT8
    {
        Failure,
        Success
    };

    #pragma pack(push, 1)
    struct BlockAddResult
    {
        UINT16 opcode{ 0x220E };
        BlockAddResultType result;
    };
    #pragma pack(pop)
}
