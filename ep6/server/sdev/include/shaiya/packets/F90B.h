#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct WorldNotice
    {
        UINT16 opcode{ 0xF90B };
        UINT8 length;
        // size = length
        Array<char, 128> text;
    };
    #pragma pack(pop)
}
