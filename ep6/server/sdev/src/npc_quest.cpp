#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0903.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CQuest.h>
#include <include/shaiya/include/CQuestData.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/SConnection.h>
using namespace shaiya;

// use this until the ep6.4 rewards are implemented
#pragma pack(push, 1)
struct TempQuestEndResponse
{
    UINT16 opcode{ 0x903 };
    ULONG npcId;
    UINT16 questId;
    bool success;
    UINT8 rewardIndex;
    UINT32 exp;
    UINT32 gold;
    UINT8 count;
    UINT8 bag;
    UINT8 slot;
    UINT8 type;
    UINT8 typeId;
    PAD(10);
};
#pragma pack(pop)

namespace npc_quest
{
    constexpr int max_reward_index = 2; // to-do: 5
    constexpr int reward_list_size = 3;

    void send_admin_remove(CUser* user, CQuest* quest)
    {
        TempQuestEndResponse response{};
        response.questId = quest->id;
        SConnection::Send(&user->connection, &response, sizeof(TempQuestEndResponse));
    }

    void send_failure_result(CUser* user, CQuest* quest, ULONG npcId)
    {
        TempQuestEndResponse response{};
        response.npcId = npcId;
        response.questId = quest->id;
        SConnection::Send(&user->connection, &response, sizeof(TempQuestEndResponse));
    }

    void send_success_result(CUser* user, CQuest* quest, Packet packet)
    {
        auto npc_id = util::read_bytes<std::uint32_t>(packet, 2);
        auto reward_index = util::read_bytes<std::uint8_t>(packet, 9);

        if (reward_index > max_reward_index)
        {
            send_failure_result(user, quest, npc_id);
            return;
        }

        TempQuestEndResponse response{};
        response.npcId = npc_id;
        response.questId = quest->id;
        response.success = true;
        response.rewardIndex = reward_index;

        response.exp = quest->questInfo->reward[reward_index].exp;
        response.gold = quest->questInfo->reward[reward_index].gold;

        // temporary

        int type = quest->questInfo->reward[reward_index].type;
        int type_id = quest->questInfo->reward[reward_index].typeId;
        // see ps_game.0x48DF68
        std::uint8_t item_count = 1;

        int bag, slot;
        auto item_info = std::make_unique<CGameData::ItemInfo*>();
        if (CUser::QuestAddItem(user, type, type_id, item_count, &bag, &slot, item_info.get()))
        {
            response.count = item_count;
            response.bag = static_cast<std::uint8_t>(bag);
            response.slot = static_cast<std::uint8_t>(slot);
            response.type = (*item_info)->type;
            response.typeId = (*item_info)->typeId;
        }

        SConnection::Send(&user->connection, &response, sizeof(TempQuestEndResponse));
    }
}

unsigned u0x48D237 = 0x48D237;
void __declspec(naked) naked_0x48D1F2()
{
    __asm
    {
        pushad

        push ebp // quest
        push ebx // user
        call npc_quest::send_admin_remove
        add esp,0x8

        popad

        jmp u0x48D237
    }
}

unsigned u0x48DC91 = 0x48DC91;
void __declspec(naked) naked_0x48DC3D()
{
    __asm
    {
        // original
        mov ecx,[esp+0xE0]

        pushad

        push ecx // npcId
        push ebx // quest
        push edi // user
        call npc_quest::send_failure_result
        add esp,0xC

        popad

        jmp u0x48DC91
    }
}

unsigned u0x48E009 = 0x48E009;
void __declspec(naked) naked_0x48DF4A()
{
    __asm
    {
        lea edx,[esp+0x10]

        pushad

        push edx // packet
        push ebx // quest
        push edi // user
        call npc_quest::send_success_result
        add esp,0xC

        popad

        jmp u0x48E009
    }
}

void hook::npc_quest()
{
    // CUser::QuestRemoveAdmin
    util::detour((void*)0x48D1F2, naked_0x48D1F2, 9);
    // CUser::QuestEnd
    util::detour((void*)0x48DC3D, naked_0x48DC3D, 11);
    // CUser::QuestSuccessResult
    util::detour((void*)0x48DF4A, naked_0x48DF4A, 7);
}
