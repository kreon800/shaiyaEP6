#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ExchangeRequest
    {
        UINT16 opcode{ 0xA05 };
        UINT8 state;
    };

    struct ExchangeResponse
    {
        UINT16 opcode{ 0xA05 };
        UINT8 state1;
        UINT8 state2;
    };
    #pragma pack(pop)
}
