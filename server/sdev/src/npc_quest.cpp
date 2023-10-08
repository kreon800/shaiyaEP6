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

// temporary
#pragma pack(push, 1)
struct Temp0903
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
        Temp0903 response{};
        response.questId = quest->id;
        SConnection::Send(&user->connection, &response, sizeof(Temp0903));
    }

    void send_failure_result(CUser* user, CQuest* quest, ULONG npcId)
    {
        Temp0903 response{};
        response.npcId = npcId;
        response.questId = quest->id;
        SConnection::Send(&user->connection, &response, sizeof(Temp0903));
    }

    void send_success_result(CUser* user, CQuest* quest, Packet buffer)
    {
        auto npcId = util::read_bytes<std::uint32_t>(buffer, 2);
        auto rewardIndex = util::read_bytes<std::uint8_t>(buffer, 9);

        if (rewardIndex > max_reward_index)
        {
            send_failure_result(user, quest, npcId);
            return;
        }

        Temp0903 response{};
        response.npcId = npcId;
        response.questId = quest->id;
        response.success = true;
        response.rewardIndex = rewardIndex;

        response.exp = quest->questInfo->reward[rewardIndex].exp;
        response.gold = quest->questInfo->reward[rewardIndex].gold;

        // temporary

        int type = quest->questInfo->reward[rewardIndex].type;
        int typeId = quest->questInfo->reward[rewardIndex].typeId;
        // see ps_game.0x48DF68
        std::uint8_t itemCount = 1;

        int bag, slot;
        auto itemInfo = std::make_unique<CGameData::ItemInfo*>();
        if (CUser::QuestAddItem(user, type, typeId, itemCount, &bag, &slot, itemInfo.get()))
        {
            response.count = itemCount;
            response.bag = static_cast<std::uint8_t>(bag);
            response.slot = static_cast<std::uint8_t>(slot);
            response.type = (*itemInfo)->type;
            response.typeId = (*itemInfo)->typeId;
        }

        SConnection::Send(&user->connection, &response, sizeof(Temp0903));
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
