#pragma once
#include "../common.h"

namespace shaiya
{
    #pragma pack(push, 1)
    struct SConnection
    {
        PAD(104);
        Socket socket;   //0x68
        PAD(4);
        ULONG ipv4Addr;  //0x70
        PAD(92);
        // 0xD0

        static void Send(SConnection* connection/*ecx*/, void* buf, int len);
        static void Close(SConnection* connection/*ecx*/, int closeType, int closeErr);
    };
    #pragma pack(pop)
}
