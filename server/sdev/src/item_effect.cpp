#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0221.h>
#include <include/shaiya/include/CGameData.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CNpcData.h>
#include <include/shaiya/include/CObject.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/SConnectionTServerReconnect.h>
#include <include/shaiya/include/SVector.h>
using namespace shaiya;

namespace item_effect
{
    constexpr int town_scroll_cast_time = 5000;
    constexpr int town_scroll_max_location = 2;

    int handler(CUser* user, CItem* item, CGameData::ItemEffect effect, int bag, int slot)
    {
        switch (effect)
        {
        case CGameData::ItemEffect::TownTeleportScroll:
        {
            NpcGateKeeper* gateKeeper = nullptr;

            switch (item->itemInfo->itemId)
            {
            case 101102:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 111);
                break;
            }
            case 101103:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 112);
                break;
            }
            case 101104:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 101);
                break;
            }
            case 101105:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 102);
                break;
            }
            case 101106:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 103);
                break;
            }
            case 101107:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 104);
                break;
            }
            case 101108:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 105);
                break;
            }
            case 101109:
            {
                gateKeeper = CNpcData<NpcGateKeeper*>::GetNpc(2, 106);
                break;
            }
            default:
                return 0;
            }

            if (!gateKeeper)
                return 0;

            user->recallMapId = gateKeeper->gate[user->townScrollLocation].mapId;
            user->recallPos = gateKeeper->gate[user->townScrollLocation].outPos;
            user->recallType = RecallType::TownTeleportScroll;
            user->recallTime = GetTickCount() + town_scroll_cast_time;

            ItemCast packet{ 0x221, user->id };
            CObject::PSendViewCombat(user, &packet, sizeof(ItemCast));
            return 1;
        }
        default:
            return 0;
        }
    }

    void town_scroll_packet_handler(CUser* user, Packet buffer)
    {
        if (user->stateType == StateType::Death)
            return;

        // to-do
        if (user->unknown0x5880)
            return;

        // to-do
        if (user->unknown0x1358)
            return;

        auto bag = util::read_bytes<std::uint8_t>(buffer, 2);
        auto slot = util::read_bytes<std::uint8_t>(buffer, 3);

        if (!bag || bag > user->bagsUnlocked || slot >= MAX_INVENTORY_SLOT)
            return;

        auto& item = user->inventory[bag][slot];
        if (!item)
            return;

        if (item->itemInfo->effect != CGameData::ItemEffect::TownTeleportScroll)
            return;

        auto location = util::read_bytes<std::uint8_t>(buffer, 4);
        if (location > town_scroll_max_location)
            return;

        user->recallItemBag = bag;
        user->recallItemSlot = slot;
        user->townScrollLocation = location;

        CUser::CancelActionExc(user);
        MyShop::Ended(&user->myShop);
        CUser::ItemUse(user, bag, slot, user->id, 0);
    }

    int town_scroll_event_handler(CUser* user)
    {
        auto& item = user->inventory[user->recallItemBag][user->recallItemSlot];
        if (!item)
            return 0;

        if (item->itemInfo->realType != CGameData::ItemRealType::Teleportation)
            return 0;

        if (item->itemInfo->effect != CGameData::ItemEffect::TownTeleportScroll)
            return 0;

        CUser::ItemUseNSend(user, user->recallItemBag, user->recallItemSlot, false);
        return 1;
    }
}

unsigned u0x47469F = 0x47469F;
unsigned u0x474690 = 0x474690;
void __declspec(naked) naked_0x47468A()
{
    __asm
    {
        pushad

        mov edx,[esp+0xB7C]
        mov edi,[esp+0xB78]

        push edx // bag
        push edi // slot
        push ecx // effect
        push ebx // item
        push ebp // user
        call item_effect::handler
        add esp,0x14
        test eax,eax

        popad

        jne _0x47469F

        // original
        mov eax,[esp+0x40]
        test eax,eax
        jmp u0x474690

        _0x47469F:
        jmp u0x47469F
    }
}

unsigned u0x4784DB = 0x4784DB;
unsigned u0x479155 = 0x479155;
void __declspec(naked) naked_0x4784D6()
{
    __asm
    {
        add eax,-0x501
        cmp eax,0x59
        je case_0x55A
        jmp u0x4784DB

        case_0x55A:
        pushad

        push ebp // packet
        push ecx // user
        call item_effect::town_scroll_packet_handler
        add esp,0x8

        popad

        jmp u0x479155
    }
}

unsigned u0x49DDC8 = 0x49DDC8;
unsigned u0x49DEB5 = 0x49DEB5;
unsigned u0x49E8D1 = 0x49E8D1;
void __declspec(naked) naked_0x49DDBF()
{
    __asm
    {
        cmp eax,0x7
        je town_scroll

        // original
        cmp eax,0x1
        jne _0x49DEB5
        jmp u0x49DDC8

        town_scroll:
        pushad

        push edi // user
        call item_effect::town_scroll_event_handler
        add esp,0x4
        test eax,eax

        popad

        je _0x49E8D1
        // success
        jmp u0x49DDC8
        
        _0x49E8D1:
        // failure
        jmp u0x49E8D1

        _0x49DEB5:
        jmp u0x49DEB5
    }
}

void hook::item_effect()
{
    // CUser::ItemUse
    util::detour((void*)0x47468A, naked_0x47468A, 6);
    // CUser::PacketPC
    util::detour((void*)0x4784D6, naked_0x4784D6, 5);
    // CUser::UpdateResetPosition
    util::detour((void*)0x49DDBF, naked_0x49DDBF, 9);
}