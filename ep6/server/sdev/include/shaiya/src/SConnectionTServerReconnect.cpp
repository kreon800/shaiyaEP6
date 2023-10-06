#include <include/shaiya/include/SConnectionTServerReconnect.h>
using namespace shaiya;

void SConnectionTServerReconnect::Send(void* clientToPtr, void* buf, int len)
{
    typedef void(__thiscall* LPFN)(void*, void*, int);
    (*(LPFN)0x4ED2D0)(clientToPtr, buf, len);
}
