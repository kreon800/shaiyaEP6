#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct ClientHeartbeat
    {
        UINT16 opcode{ 0x003 };
    };
    #pragma pack(pop)
}
