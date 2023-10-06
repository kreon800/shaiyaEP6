#pragma once
#include <include/shaiya/common.h>
#include <include/shaiya/include/SNode.h>

namespace shaiya
{
    FWDDECL CItem;

    typedef Array<char, 66> GuildRemark;

    #pragma pack(push, 1)
    struct CGuild
    {
        SNode node;                //0x00
        ULONG id;                  //0x08
        GuildName name;            //0x0C
        CharName bossName;         //0x25
        PAD(2);
        UINT32 numSubBosses;       //0x3C
        UINT32 country;            //0x40
        UINT32 points;             //0x44
        UINT32 rank;               //0x48
        UINT32 etin;               //0x4C
        UINT32 keepEtin;           //0x50
        bool hasHouse;             //0x54
        bool buyHouse;             //0x55
        GuildRemark remark;        //0x56
        UINT32 guildRankPoints;    //0x98
        UINT32 etinReturnCount;    //0x9C
        UINT32 grbJoinCount;       //0xA0
        // 0xA4
        Array<CItem*, 240> warehouse;
        // 0x464
        PAD(196);
        UINT32 numEnterUsers;      //0x528
        PAD(68);
        UINT32 numLeaveUsers;      //0x570
        PAD(148);
        // 0x608

        static void Send(CGuild* guild, void* data/*ecx*/, int len/*eax*/);
        static void SendAdmin(CGuild* guild, void* data/*ecx*/, int len/*eax*/);
    };
    #pragma pack(pop)
}
