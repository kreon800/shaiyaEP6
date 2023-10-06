#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0303.h>
#include <include/shaiya/packets/051D.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/CZone.h>
#include <include/shaiya/include/SConnection.h>
using namespace shaiya;

namespace user_shape
{
    constexpr int max_equipment_slot = ITEM_LIST_SIZE;
    #ifdef SHAIYA_EP6_4
    constexpr int shape_size_without_cloak = 118;
    constexpr int shape_size_with_cloak = 124;
    #else
    constexpr int shape_size_without_cloak = 115;
    constexpr int shape_size_with_cloak = 121;
    #endif

    void init_clone_user(CUser* user, CUser* target)
    {
        std::memset(user->clone, 0, sizeof(CloneUser));

        user->clone->dead = target->stateType == StateType::Death ? true : false;
        user->clone->sitting = target->sitting;
        user->clone->country = target->country;
        user->clone->family = target->family;
        user->clone->hair = target->hair;
        user->clone->face = target->face;
        user->clone->size = target->size;
        user->clone->job = target->job;
        user->clone->sex = target->sex;
        user->clone->partyType = CUser::GetPartyType(target);
        user->clone->grow = target->grow;
        user->clone->kills = target->kills;
        
        for (int slot = 0; slot < max_equipment_slot; ++slot)
        {
            auto& item = target->inventory[0][slot];
            if (!item)
                continue;

            user->clone->equipment[slot].type = item->type;
            user->clone->equipment[slot].typeId = item->typeId;
            user->clone->equipment[slot].enchantStep = CItem::GetEnchantStep(item);
        }

        user->clone->charName = target->charName;

        auto& item = target->inventory[0][EquipmentSlot::Cloak];
        if (!item)
        {
            CUser::GetGuildName(target, user->clone->guildName.data());
            user->clone->packetLength = shape_size_without_cloak;
        }
        else
        {
            std::memcpy(&user->clone->cloakBadge, &item->gems, item->gems.size());
            CUser::GetGuildName(target, user->clone->guildName.data());
            user->clone->packetLength = shape_size_with_cloak;
        }
    }

    void send_zone_shape(CUser* user, CUser* target)
    {
        UserShape shape{};
        shape.charId = user->id;

        if (user->shapeType == ShapeType::Disguise && user->clone)
        {
            shape.dead = user->clone->dead;
            shape.sitting = user->clone->sitting;
            shape.country = user->clone->country;
            shape.family = user->clone->family;
            shape.hair = user->clone->hair;
            shape.face = user->clone->face;
            shape.size = user->clone->size;
            shape.job = user->clone->job;
            shape.sex = user->clone->sex;
            shape.partyType = user->clone->partyType;
            shape.grow = user->clone->grow;
            shape.kills = user->clone->kills;

            std::memcpy(&shape.equipment, &user->clone->equipment, sizeof(shape.equipment));
            shape.charName = user->clone->charName;

            if (user->clone->packetLength == shape_size_with_cloak)
            {
                shape.cloakBadge = user->clone->cloakBadge;
                shape.guildName = user->clone->guildName;
                SConnection::Send(&target->connection, &shape, shape_size_with_cloak);
            }
            else
            {
                std::memcpy(&shape.cloakBadge, &user->clone->guildName, user->clone->guildName.size());
                SConnection::Send(&target->connection, &shape, shape_size_without_cloak);
            }

            return;
        }

        shape.dead = user->stateType == StateType::Death ? true : false;
        shape.sitting = user->sitting;
        shape.country = user->country;
        shape.family = user->family;
        shape.hair = user->hair;
        shape.face = user->face;
        shape.size = user->size;
        shape.job = user->job;
        shape.sex = user->sex;
        shape.partyType = CUser::GetPartyType(user);
        shape.grow = user->grow;
        shape.kills = user->kills;

        for (int slot = 0; slot < max_equipment_slot; ++slot)
        {
            auto& item = user->inventory[0][slot];
            if (!item)
                continue;

            shape.equipment[slot].type = item->type;
            shape.equipment[slot].typeId = item->typeId;
            shape.equipment[slot].enchantStep = CItem::GetEnchantStep(item);
        }

        shape.charName = user->charName;

        auto& item = user->inventory[0][EquipmentSlot::Cloak];
        if (!item)
        {
            CUser::GetGuildName(user, reinterpret_cast<char*>(&shape.cloakBadge));
            SConnection::Send(&target->connection, &shape, shape_size_without_cloak);
        }
        else
        {
            std::memcpy(&shape.cloakBadge, &item->gems, item->gems.size());
            CUser::GetGuildName(user, shape.guildName.data());
            SConnection::Send(&target->connection, &shape, shape_size_with_cloak);
        }
    }

    void send_zone_view(CUser* user)
    {
        if (!user->zone)
            return;

        UserShape shape{};
        shape.charId = user->id;

        if (user->shapeType == ShapeType::Disguise && user->clone)
        {
            shape.dead = user->clone->dead;
            shape.sitting = user->clone->sitting;
            shape.country = user->clone->country;
            shape.family = user->clone->family;
            shape.hair = user->clone->hair;
            shape.face = user->clone->face;
            shape.size = user->clone->size;
            shape.job = user->clone->job;
            shape.sex = user->clone->sex;
            shape.partyType = user->clone->partyType;
            shape.grow = user->clone->grow;
            shape.kills = user->clone->kills;

            std::memcpy(&shape.equipment, &user->clone->equipment, sizeof(shape.equipment));
            shape.charName = user->clone->charName;

            if (user->clone->packetLength == shape_size_with_cloak)
            {
                shape.cloakBadge = user->clone->cloakBadge;
                shape.guildName = user->clone->guildName;
                CZone::SendView(user->zone, &shape, shape_size_with_cloak, user->cellX, user->cellZ);
            }
            else
            {
                std::memcpy(&shape.cloakBadge, &user->clone->guildName, user->clone->guildName.size());
                CZone::SendView(user->zone, &shape, shape_size_without_cloak, user->cellX, user->cellZ);
            }

            return;
        }

        shape.dead = user->stateType == StateType::Death ? true : false;
        shape.sitting = user->sitting;
        shape.country = user->country;
        shape.family = user->family;
        shape.hair = user->hair;
        shape.face = user->face;
        shape.size = user->size;
        shape.job = user->job;
        shape.sex = user->sex;
        shape.partyType = CUser::GetPartyType(user);
        shape.grow = user->grow;
        shape.kills = user->kills;

        for (int slot = 0; slot < max_equipment_slot; ++slot)
        {
            auto& item = user->inventory[0][slot];
            if (!item)
                continue;

            shape.equipment[slot].type = item->type;
            shape.equipment[slot].typeId = item->typeId;
            shape.equipment[slot].enchantStep = CItem::GetEnchantStep(item);
        }

        shape.charName = user->charName;

        auto& item = user->inventory[0][EquipmentSlot::Cloak];
        if (!item)
        {
            CUser::GetGuildName(user, reinterpret_cast<char*>(&shape.cloakBadge));
            CZone::SendView(user->zone, &shape, shape_size_without_cloak, user->cellX, user->cellZ);
        }
        else
        {
            std::memcpy(&shape.cloakBadge, &item->gems, item->gems.size());
            CUser::GetGuildName(user, shape.guildName.data());
            CZone::SendView(user->zone, &shape, shape_size_with_cloak, user->cellX, user->cellZ);
        }
    }

    void send_zone_shape_type(CUser* user, Packet packet)
    {
        UserShapeType shape{};
        shape.charId = user->id;
        shape.shapeType = util::read_bytes<ShapeType>(packet, 6);
        
        #ifdef SHAIYA_EP6_4
        auto& vehicle = user->inventory[0][EquipmentSlot::Vehicle];
        if (vehicle)
        {
            shape.vehicleType = vehicle->type;
            shape.vehicleTypeId = vehicle->typeId;
        }
        #endif

        if (!user->zone)
            return;

        CZone::SendView(user->zone, &shape, sizeof(UserShapeType), user->cellX, user->cellZ);
    }

    void send_user_shape_type(CUser* target, CUser* user, ShapeType shapeType)
    {
        UserShapeType shape{};
        shape.charId = user->id;
        shape.shapeType = shapeType;

        #ifdef SHAIYA_EP6_4
        auto& vehicle = user->inventory[0][EquipmentSlot::Vehicle];
        if (vehicle)
        {
            shape.vehicleType = vehicle->type;
            shape.vehicleTypeId = vehicle->typeId;
        }
        #endif

        SConnection::Send(&target->connection, &shape, sizeof(UserShapeType));
    }
}

unsigned u0x426CA0 = 0x426CA0;
void __declspec(naked) naked_0x426948()
{
    __asm
    {
        pushad
        
        push ebp // target
        push ebx // user
        call user_shape::send_zone_shape
        add esp,0x8

        popad

        jmp u0x426CA0
    }
}

unsigned u0x411B26 = 0x411B26;
unsigned u0x51B26B = 0x51B26B;
void __declspec(naked) naked_0x411B1F()
{
    __asm
    {
        push 0x80 // size
        call u0x51B26B // malloc
        jmp u0x411B26
    }
}

unsigned u0x491EBF = 0x491EBF;
void __declspec(naked) naked_0x491B13()
{
    __asm
    {
        push esi
        push edi

        pushad

        push ebp // user
        call user_shape::send_zone_view
        add esp,0x4
        
        popad

        jmp u0x491EBF
    }
}

unsigned u0x45A246 = 0x45A246;
void __declspec(naked) naked_0x45A365()
{
    __asm
    {
        pushad

        push esi // target
        push edi // user
        call user_shape::init_clone_user
        add esp,0x8
        
        popad

        jmp u0x45A246
    }
}

unsigned u0x4914A9 = 0x4914A9;
void __declspec(naked) naked_0x491490()
{
    __asm
    {
        pushad

        lea eax,[esp+0x20]

        push eax // packet
        push ecx // user
        call user_shape::send_zone_shape_type
        add esp,0x8
        
        popad

        jmp u0x4914A9
    }
}

unsigned u0x42A57E = 0x42A57E;
void __declspec(naked) naked_0x42A56C()
{
    __asm
    {
        pushad

        push eax // shapeType
        push esi // user
        push ebx // target
        call user_shape::send_user_shape_type
        add esp,0xC
        
        popad

        jmp u0x42A57E
    }
}

unsigned u0x4263B8 = 0x4263B8;
void __declspec(naked) naked_0x4263AD()
{
    __asm
    {
        // shapeType
        mov byte ptr[esp+0x3E],al

        xor eax,eax
        mov [esp+0x3F],eax
        mov [esp+0x43],eax

        // user->inventory[0][13]
        mov eax,[edi+0x1F0]
        test eax,eax
        je without_vehicle

        // vehicle->type
        movzx ecx,byte ptr[eax+0x40]
        mov [esp+0x3F],ecx

        // vehicle->typeId
        movzx ecx,byte ptr[eax+0x41]
        mov [esp+0x43],ecx

        without_vehicle:

        lea ecx,[esp+0x38]
        push 0xF // length
        push ecx // packet

        jmp u0x4263B8
    }
}

void hook::user_shape()
{
    // CZone::SendUserShape
    util::detour((void*)0x426948, naked_0x426948, 7);
    // CObjectMgr::CreateCloneUser
    util::detour((void*)0x411B1F, naked_0x411B1F, 7);
    // CUser::SendUserShape
    util::detour((void*)0x491B13, naked_0x491B13, 7);
    // CUser::CheckTargetUser case 2
    util::detour((void*)0x45A365, naked_0x45A365, 7);
    #ifdef SHAIYA_EP6_4
    // CUser::SendShape
    util::detour((void*)0x491490, naked_0x491490, 6);
    //
    util::detour((void*)0x42A56C, naked_0x42A56C, 6);
    // CZone::SendMoveUser
    util::detour((void*)0x4263AD, naked_0x4263AD, 6);
    #endif
}
