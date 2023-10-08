#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ExchangeConfirmRequest
    {
        UINT16 opcode{ 0xA0A };
        UINT8 state;
    };

    struct ExchangeConfirmResponse
    {
        UINT16 opcode{ 0xA0A };
        UINT8 state1;
        UINT8 state2;
    };
    #pragma pack(pop)
}
