#pragma once
#include <sdev/include/shaiya/common.h>

// CUser::PacketUserSetStatus

namespace shaiya
{
    #pragma pack(push, 1)
    enum struct UserSetStatusGroup
    {
        // hg
        Honor = 1,
        // vg, cg, og, ig
        Other = 2,
    };

    struct UserSetStatusExpIncoming
    {
        UINT16 opcode{ 0x602 };
        UserId userId;
        UINT32 exp;
    };

    struct UserSetStatusMoneyIncoming
    {
        UINT16 opcode{ 0x603 };
        UserId userId;
        UINT32 money;
    };

    struct UserSetStatusUpIncoming
    {
        UINT16 opcode{ 0x604 };
        UserId userId;
        UINT16 statusPoint;
        UINT16 strength;
        UINT16 dexterity;
        UINT16 recovery;
        UINT16 intelligence;
        UINT16 wisdom;
        UINT16 luck;
    };

    struct UserSetStatusGroupIncoming
    {
        UINT16 opcode{ 0x605 };
        UserId userId;
        UserSetStatusGroup group;
        UINT16 value;
    };

    struct UserSetStatusQuickSlot
    {
        UINT8 bag;
        UINT8 slot;
        UINT8 srcBag;
        UINT16 srcSlot;
    };

    struct UserSetStatusQuickSlotIncoming
    {
        UINT16 opcode{ 0x609 };
        UserId userId;
        UINT8 quickSlotCount;
        // size = quickSlotCount
        Array<UserSetStatusQuickSlot, 128> quickSlot;
    };

    struct UserSetStatusLevelIncoming
    {
        UINT16 opcode{ 0x60A };
        UserId userId;
        UINT32 level;
    };

    // not implemented
    struct UserSetStatusBankMoneyIncoming
    {
        UINT16 opcode{ 0x60B };
        UserId userId;
        UINT32 bankMoney;
    };

    struct UserSetStatusKillPCIncoming
    {
        UINT16 opcode{ 0x60C };
        UserId userId;
        KillPC killPC;
        UINT32 kills;
    };

    struct UserSetStatusGrowIncoming
    {
        UINT16 opcode{ 0x60D };
        UserId userId;
        UINT32 grow;
    };

    struct UserSetStatusSkillPointIncoming
    {
        UINT16 opcode{ 0x60E };
        UserId userId;
        UINT32 skillPoint;
    };

    struct UserSetStatusPointIncoming
    {
        UINT16 opcode{ 0x60F };
        UserId userId;
        UINT32 statusPoint;
    };
    #pragma pack(pop)
}
