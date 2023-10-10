#pragma once
#include <include/shaiya/common.h>
#include <include/shaiya/include/CFriend.h>

namespace shaiya
{
    #pragma pack(push, 1)
    struct BlockSaveMemoRequest
    {
        UINT16 opcode{ 0x220B };
        ULONG charId;
        // w/ null terminator
        UINT8 memoLength;
        Memo memo;
    };

    struct BlockSaveMemoResponse
    {
        UINT16 opcode{ 0x220B };
        ULONG charId;
        // w/o null terminator
        UINT8 memoLength;
        Memo memo;
    };
    #pragma pack(pop)
}
