#pragma once
#include <include/shaiya/common.h>

namespace shaiya
{
    #undef SHAIYA_EP6_4 // temporary
    static Address g_QuestData = 0x4093300;

    typedef Array<char, 256> QuestName;

    #pragma pack(push, 1)
    struct QuestItem
    {
        UINT8 type;
        UINT8 typeId;
        UINT8 count;
    };

    #ifdef SHAIYA_EP6_4
    struct QuestResult
    {
        PAD(20);
        UINT32 exp;            //0x14
        UINT32 gold;           //0x18
        // 0x1C
        Array<QuestItem, 3> item;
        PAD(1);
        UINT16 unlockQuestId;  //0x26
        PAD(4);
        // 0x2C = size
    };
    #else
    struct QuestResult
    {
        PAD(20);
        UINT32 exp;            //0x14
        UINT32 gold;           //0x18
        UINT8 type;            //0x1C
        UINT8 typeId;          //0x1D
        UINT16 unlockQuestId;  //0x1E
        // 0x20 = size
    };
    #endif

    struct QuestInfo
    {
        UINT16 questId;         //0x00
        QuestName questName;    //0x02
        UINT16 minLevel;        //0x102
        UINT16 maxLevel;        //0x104
        Country country;        //0x106
        Grow grow;              //0x107
        PAD(2);
        bool attackFighter;     //0x10A
        bool defenseFighter;    //0x10B
        bool patrolRogue;       //0x10C
        bool shootRogue;        //0x10D
        bool attackMage;        //0x10E
        bool defenseMage;       //0x10F
        UINT16 prevQuestId1;    //0x110
        UINT16 prevQuestId2;    //0x112
        PAD(4);
        UINT16 prevQuestId3;    //0x118
        PAD(14);
        UINT16 timeLimit;       //0x128
        PAD(10);
        BOOL repeatable;        //0x134
        UINT8 recvQuestType;    //0x138
        UINT8 giveNpcType;      //0x139
        UINT16 giveNpcTypeId;   //0x13A
        PAD(2);
        // 0x13E
        Array<QuestItem, 3> recvItem;
        UINT8 questType;        //0x147
        UINT8 recvNpcType;      //0x148
        PAD(1);
        UINT16 recvNpcTypeId;   //0x14A
        // 0x14C
        Array<QuestItem, 3> farmItem;
        UINT8 killCount;        //0x155
        UINT16 mobId1;          //0x156
        UINT16 mobId2;          //0x158
        UINT8 mobCount1;        //0x15A
        UINT8 mobCount2;        //0x15B
        PAD(4);
        #ifdef SHAIYA_EP6_4
        // 0x160
        Array<QuestResult, 6> reward;
        // 0x268
        PAD(28);
        // 0x284 = size
        #else
        // 0x160
        Array<QuestResult, 3> reward;
        // 0x1C0
        PAD(16);
        // 0x1D0 = size
        #endif
    };

    struct CQuestData
    {
        static QuestInfo* GetQuest(int questId/*eax*/);
    };
    #pragma pack(pop)
}
