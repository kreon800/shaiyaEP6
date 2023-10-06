#include <map>
#include <random>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0806.h>
#include <include/shaiya/packets/0807.h>
#include <include/shaiya/packets/080D.h>
#include <include/shaiya/packets/dbAgent/0717.h>
#include <include/shaiya/include/CGameData.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/CZone.h>
#include <include/shaiya/include/SConnection.h>
#include <include/shaiya/include/SConnectionTServerReconnect.h>
using namespace shaiya;

namespace packet_gem
{
    enum struct PerfectLapisianType : UINT8
    {
        Weapon,
        Armor
    };

    constexpr int max_reqwis = 99;
    constexpr int weapon_lapisian_plus = 95004;
    constexpr int hot_time_lapisian = 95005;
    constexpr int armor_lapisian_plus = 95009;

    int find_available_slot(CUser* user, int bag)
    {
        for (int slot = 0; slot < MAX_INVENTORY_SLOT; ++slot)
            if (!user->inventory[bag][slot])
                return slot;

        return -1;
    }

    bool is_perfect_lapisian(CItem* lapisian, CItem* upgradeItem)
    {
        auto enchant_step = CItem::GetEnchantStep(upgradeItem);
        auto item_id = lapisian->itemInfo->itemId;
        auto lapisian_type = static_cast<PerfectLapisianType>(lapisian->itemInfo->country);

        if (CItem::IsWeapon(upgradeItem))
        {
            if (enchant_step < 10)
                if (item_id == weapon_lapisian_plus || item_id == hot_time_lapisian)
                    return true;

            if (lapisian_type == PerfectLapisianType::Weapon && lapisian->itemInfo->range == enchant_step)
                return true;
        }
        else
        {
            if (enchant_step < 10)
                if (item_id == armor_lapisian_plus || item_id == hot_time_lapisian)
                    return true;

            if (lapisian_type == PerfectLapisianType::Armor && lapisian->itemInfo->range == enchant_step)
                return true;
        }

        return false;
    }

    void rune_combine_handler(CUser* user, Packet packet)
    {
        if (!user->zone)
            return;

        auto npc_id = util::read_bytes<std::uint32_t>(packet, 6);
        if (!CZone::FindNpc(user->zone, npc_id))
            return;

        auto rune_bag = util::read_bytes<std::uint8_t>(packet, 2);
        auto rune_slot = util::read_bytes<std::uint8_t>(packet, 3);

        if (!rune_bag || rune_bag > user->bagsUnlocked || rune_slot >= MAX_INVENTORY_SLOT)
            return;

        auto& rune = user->inventory[rune_bag][rune_slot];
        if (!rune)
            return;

        if (rune->count < 2 || rune->itemInfo->effect != CGameData::ItemEffect::ItemCompose)
        {
            RuneCombineResponse response{ 0x80D, RuneCombineResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        auto vial_bag = util::read_bytes<std::uint8_t>(packet, 4);
        auto vial_slot = util::read_bytes<std::uint8_t>(packet, 5);

        if (!vial_bag || vial_bag > user->bagsUnlocked || vial_slot >= MAX_INVENTORY_SLOT)
            return;

        auto& vial = user->inventory[vial_bag][vial_slot];
        if (!vial)
            return;

        CGameData::ItemInfo* info = nullptr;

        switch (vial->itemInfo->effect)
        {
        case CGameData::ItemEffect::ItemRemakeStr:
            info = CGameData::GetItemInfo(101, 1);
            break;
        case CGameData::ItemEffect::ItemRemakeDex:
            info = CGameData::GetItemInfo(101, 2);
            break;
        case CGameData::ItemEffect::ItemRemakeInt:
            info = CGameData::GetItemInfo(101, 3);
            break;
        case CGameData::ItemEffect::ItemRemakeWis:
            info = CGameData::GetItemInfo(101, 4);
            break;
        case CGameData::ItemEffect::ItemRemakeRec:
            info = CGameData::GetItemInfo(101, 5);
            break;
        case CGameData::ItemEffect::ItemRemakeLuc:
            info = CGameData::GetItemInfo(101, 6);
            break;
        default:
            RuneCombineResponse response{ 0x80D, RuneCombineResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        if (info)
        {
            RuneCombineResponse response{ 0x80D, RuneCombineResult::Success, 1, 0, info->type, info->typeId };
            response.count = 1;

            while (response.bag <= user->bagsUnlocked)
            {
                response.slot = find_available_slot(user, response.bag);

                if (response.slot != -1)
                {
                    if (!CUser::ItemCreate(user, info, response.count))
                        break;

                    CUser::ItemUseNSend(user, rune_bag, rune_slot, false);
                    CUser::ItemUseNSend(user, rune_bag, rune_slot, false);
                    CUser::ItemUseNSend(user, vial_bag, vial_slot, false);

                    SConnection::Send(&user->connection, &response, sizeof(RuneCombineResponse));
                    break;
                }

                ++response.bag;
            }
        }
    }

    void item_compose_handler(CUser* user, Packet packet)
    {
        auto rune_bag = util::read_bytes<std::uint8_t>(packet, 2);
        auto rune_slot = util::read_bytes<std::uint8_t>(packet, 3);

        if (!rune_bag || rune_bag > user->bagsUnlocked || rune_slot >= MAX_INVENTORY_SLOT)
            return;

        auto& rune = user->inventory[rune_bag][rune_slot];
        if (!rune)
            return;

        auto item_bag = util::read_bytes<std::uint8_t>(packet, 4);
        auto item_slot = util::read_bytes<std::uint8_t>(packet, 5);

        if (item_bag > user->bagsUnlocked || item_slot >= MAX_INVENTORY_SLOT)
            return;

        auto& item = user->inventory[item_bag][item_slot];
        if (!item)
            return;

        if (item->itemInfo->realType > CGameData::ItemRealType::Bracelet)
        {
            ItemComposeResponse response{ 0x806, ItemComposeResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        if (!item->itemInfo->maxOjCount)
        {
            ItemComposeResponse response{ 0x806, ItemComposeResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        if (item->itemInfo->reqWis <= 0 || item->itemInfo->reqWis > max_reqwis)
        {
            ItemComposeResponse response{ 0x806, ItemComposeResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        // optional
        if (item->makeType == MakeType::Q)
        {
            ItemComposeResponse response{ 0x806, ItemComposeResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        std::random_device seed;
        std::mt19937 eng(seed());

        std::uniform_int_distribution<int> uni(1, item->itemInfo->reqWis);
        std::uint8_t bonus = uni(eng);
        std::string text = std::to_string(bonus);

        if (bonus < 10)
            text.insert(text.begin(), 1, '0');

        auto max_health = user->maxHealth;
        auto max_mana = user->maxHealth;
        auto max_stamina = user->maxHealth;

        switch (rune->itemInfo->effect)
        {
        case CGameData::ItemEffect::ItemCompose:
            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);
                CItem::ReGenerationCraftExpansion(item, true);
                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            CItem::ReGenerationCraftExpansion(item, true);

            break;
        case CGameData::ItemEffect::ItemComposeStr:
            if (!item->craftStrength)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftStrength = bonus;
                item->craftName[0] = text[0];
                item->craftName[1] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftStrength = bonus;
            item->craftName[0] = text[0];
            item->craftName[1] = text[1];

            break;
        case CGameData::ItemEffect::ItemComposeDex:
            if (!item->craftDexterity)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftDexterity = bonus;
                item->craftName[2] = text[0];
                item->craftName[3] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftDexterity = bonus;
            item->craftName[2] = text[0];
            item->craftName[3] = text[1];

            break;
        case CGameData::ItemEffect::ItemComposeInt:
            if (!item->craftIntelligence)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftIntelligence = bonus;
                item->craftName[6] = text[0];
                item->craftName[7] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftIntelligence = bonus;
            item->craftName[6] = text[0];
            item->craftName[7] = text[1];

            break;
        case CGameData::ItemEffect::ItemComposeWis:
            if (!item->craftWisdom)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftWisdom = bonus;
                item->craftName[8] = text[0];
                item->craftName[9] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftWisdom = bonus;
            item->craftName[8] = text[0];
            item->craftName[9] = text[1];

            break;
        case CGameData::ItemEffect::ItemComposeRec:
            if (!item->craftReaction)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftReaction = bonus;
                item->craftName[4] = text[0];
                item->craftName[5] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftReaction = bonus;
            item->craftName[4] = text[0];
            item->craftName[5] = text[1];

            break;
        case CGameData::ItemEffect::ItemComposeLuc:
            if (!item->craftLuck)
                return;

            if (!item_bag)
            {
                CUser::ItemEquipmentOptionRem(user, item);

                item->craftLuck = bonus;
                item->craftName[10] = text[0];
                item->craftName[11] = text[1];

                CUser::ItemEquipmentOptionAdd(user, item);

                if (!user->isInitEquipment)
                {
                    if (max_health != user->maxHealth)
                        CUser::SendMaxHP(user);

                    if (max_mana != user->maxMana)
                        CUser::SendMaxMP(user);

                    if (max_stamina != user->maxStamina)
                        CUser::SendMaxSP(user);
                }

                CUser::SetAttack(user);
                break;
            }

            item->craftLuck = bonus;
            item->craftName[10] = text[0];
            item->craftName[11] = text[1];

            break;
        default:
            ItemComposeResponse response{ 0x806, ItemComposeResult::Failure };
            SConnection::Send(&user->connection, &response, 3);
            return;
        }

        ItemComposeResponse response{ 0x806, ItemComposeResult::Success, item_bag, item_slot };
        response.craftName = item->craftName;
        SConnection::Send(&user->connection, &response, sizeof(ItemComposeResponse));

        SaveItemCraftName item_craft_name{ 0x717, user->userId, item_bag, item_slot };
        item_craft_name.craftName = item->craftName;
        SConnectionTServerReconnect::Send(g_pClientToDBAgent, &item_craft_name, sizeof(SaveItemCraftName));

        CUser::ItemUseNSend(user, rune_bag, rune_slot, false);
    }
}

unsigned u0x479FBC = 0x479FBC;
void __declspec(naked) naked_0x479FB4()
{
    __asm
    {
        movzx eax,word ptr[esi]
        cmp eax,0x80D
        je case_0x80D
        cmp eax,0x830
        je case_0x830
        cmp eax,0x832
        je case_0x832

        // original
        add eax,-0x801
        jmp u0x479FBC

        case_0x80D:
        pushad

        push esi // packet
        push edi // user
        call packet_gem::rune_combine_handler
        add esp,0x8
        
        popad

        jmp exit_switch

        // chaotic squares: not on to-do list

        case_0x830:
        jmp exit_switch

        case_0x832:
        exit_switch:
        pop edi
        pop esi
        pop ecx
        retn
    }
}

unsigned u0x47A00C = 0x47A00C;
void __declspec(naked) naked_0x47A003()
{
    __asm
    {
        pushad

        push esi // packet
        push edi // user
        call packet_gem::item_compose_handler
        add esp,0x8
        
        popad

        jmp u0x47A00C
    }
}

unsigned u0x47AAE0 = 0x47AAE0;
unsigned u0x46CCF5 = 0x46CCF5;
unsigned u0x46CD83 = 0x46CD83;
void __declspec(naked) naked_0x46CCF0()
{
    __asm
    {
        // original
        call u0x47AAE0

        pushad

        mov eax,[esp+0x3C]
        push eax // item
        push esi // lapisian
        call packet_gem::is_perfect_lapisian
        add esp,0x8
        test al,al
        
        popad

        jne add_enchant_step
        jmp u0x46CCF5

        add_enchant_step:
        // isWeapon
        movzx ecx,byte ptr[esp+0x18]
        // enchant step
        movzx edx,byte ptr[esp+0x13]
        lea edx,[ecx+edx*2]
        jmp u0x46CD83
    }
}

void hook::packet_gem()
{
    // CUser::PacketGem
    util::detour((void*)0x479FB4, naked_0x479FB4, 8);
    // CUser::PacketGem case 0x806
    util::detour((void*)0x47A003, naked_0x47A003, 9);
    // CUser::ItemLapisianAdd
    util::detour((void*)0x46CCF0, naked_0x46CCF0, 5);
}
