#include <include/shaiya/include/CGameData.h>
#include <include/shaiya/include/ServerTime.h>
using namespace shaiya;

ULONG ServerTime::GetItemExpireTime(ULONG makeTime, CGameData::ItemInfo* itemInfo)
{
    if (!itemInfo->range)
        return 0;

    auto type = static_cast<ItemType>(itemInfo->type);
    if (type == ItemType::Pet || type == ItemType::Costume)
    {
        constexpr long day_as_seconds = 86400;
        constexpr long second_as_microseconds = 10000000;

        SYSTEMTIME st{};
        ServerTime::ServerTimeToSystemTime(makeTime, &st);

        FILETIME ft{};
        SystemTimeToFileTime(&st, &ft);

        ULARGE_INTEGER li{ ft.dwLowDateTime, ft.dwHighDateTime };
        auto seconds = static_cast<long long>(itemInfo->range) * day_as_seconds;

        li.QuadPart += seconds * second_as_microseconds;
        ft.dwLowDateTime = li.LowPart;
        ft.dwHighDateTime = li.HighPart;

        FileTimeToSystemTime(&ft, &st);
        return ServerTime::SystemTimeToServerTime(&st);
    }

    return 0;
}

ULONG ServerTime::GetSystemTime()
{
    typedef ULONG(__stdcall* LPFN)();
    return (*(LPFN)0x4E1A50)();
}

ULONG ServerTime::ServerTimeToSystemTime(ULONG time/*eax*/, LPSYSTEMTIME lpst/*ecx*/)
{
    Address u0x4E1CA0 = 0x4E1CA0;

    __asm
    {
        mov eax,time
        mov ecx,lpst
        call u0x4E1CA0
    }
}

ULONG ServerTime::SystemTimeToServerTime(LPSYSTEMTIME lpst/*ecx*/)
{
    Address u0x4E1CF0 = 0x4E1CF0;

    __asm
    {
        mov ecx,lpst
        call u0x4E1CF0
    }
}
