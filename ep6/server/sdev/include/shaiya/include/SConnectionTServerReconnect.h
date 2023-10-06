#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    // 0x587968
    static void* g_pClientToMgr = reinterpret_cast<void*>(0x10A2540);
    // 0x58796C
    static void* g_pClientToDBAgent = reinterpret_cast<void*>(0x10A2628);

    #pragma pack(push, 1)
    struct SConnectionTServerReconnect
    {
        static void Send(void* clientToPtr, void* buf, int len);
    };
    #pragma pack(pop)
}
