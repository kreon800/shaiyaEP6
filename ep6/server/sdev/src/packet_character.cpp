#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0101.h>
#include <include/shaiya/packets/0119.h>
#include <include/shaiya/packets/0403.h>
#include <include/shaiya/packets/0711.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/SConnection.h>
#include <include/shaiya/include/SConnectionTServerReconnect.h>
#include <include/shaiya/include/ServerTime.h>
using namespace shaiya;

namespace packet_character
{
    // custom packets

    #pragma pack(push, 1)
    struct DBNameAvailableRequest
    {
        UINT16 opcode{ 0x40D };
        UserId userId;
        Array<char, 19> charName;
    };

    struct DBNameAvailableResponse
    {
        UINT16 opcode{ 0x40D };
        UserId userId;
        bool available;
    };
    #pragma pack(pop)

    void name_available_handler(CUser* user, const char* name)
    {
        constexpr int packet_size_without_name = 6;
        constexpr int min_name_len = 3;
        constexpr int max_name_len = 13;

        DBNameAvailableRequest request{};
        request.userId = user->userId;

        std::string char_name(name);
        if (char_name.length() < min_name_len || char_name.length() > max_name_len)
        {
            CharNameAvailableResponse response{ 0x119, false };
            SConnection::Send(&user->connection, &response, sizeof(CharNameAvailableResponse));
            return;
        }

        std::memcpy(request.charName.data(), char_name.data(), char_name.length());
        int packet_size = packet_size_without_name + char_name.length() + 1;
        SConnectionTServerReconnect::Send(g_pClientToDBAgent, &request, packet_size);
    }

    void send_name_available(CUser* user, Packet packet)
    {
        auto available = util::read_bytes<bool>(packet, 6);
        CharNameAvailableResponse response{ 0x119, available };
        SConnection::Send(&user->connection, &response, sizeof(CharNameAvailableResponse));
    }

    void send_warehouse(CUser* user)
    {
        // the packet length within the loop
        // 
        // ep5: 1657 = 7 + (33 * 50)
        // ep6: 2057 = 7 + (41 * 50)
        //
        // 2057 bytes will crash ps_game (max is 2048)
        // shen1l changed 50 to 40
        constexpr int max_item_send_count = 40;
        constexpr int packet_size_without_list = 7;

        StoredItemList warehouse{};
        warehouse.bankMoney = user->bankMoney;
        warehouse.itemCount = 0;

        for (int slot = 0; slot < MAX_WAREHOUSE_SLOT; ++slot)
        {
            auto& item = user->warehouse[slot];
            if (!item)
                continue;

            Item0711 warehouse_item{};
            warehouse_item.slot = slot;
            warehouse_item.type = item->type;
            warehouse_item.typeId = item->typeId;
            warehouse_item.quality = item->quality;
            warehouse_item.gems = item->gems;
            warehouse_item.count = item->count;

            #ifdef SHAIYA_EP6
            warehouse_item.toDate = ServerTime::GetItemExpireTime(item->makeTime, item->itemInfo);
            warehouse_item.fromDate = warehouse_item.toDate ? item->makeTime : 0;
            #endif

            warehouse_item.craftName = item->craftName;
            std::memcpy(&warehouse.itemList[warehouse.itemCount], &warehouse_item, sizeof(Item0711));
            ++warehouse.itemCount;

            if (warehouse.itemCount != max_item_send_count)
                continue;
            else
            {
                int packet_size = packet_size_without_list + (warehouse.itemCount * sizeof(Item0711));
                SConnection::Send(&user->connection, &warehouse, packet_size);

                std::memset(&warehouse.itemList, 0, sizeof(warehouse.itemList));
                warehouse.itemCount = 0;
            }
        }

        if (warehouse.itemCount <= 0)
            return;

        int packet_size = packet_size_without_list + (warehouse.itemCount * sizeof(Item0711));
        SConnection::Send(&user->connection, &warehouse, packet_size);
    }

    void send_character(CUser* user, Character0403* dbCharacter)
    {
        CharacterList character{};
        character.slot = dbCharacter->slot;
        character.charId = dbCharacter->id;
        character.regDate = dbCharacter->regDate;
        character.level = dbCharacter->level;
        character.family = dbCharacter->family;
        character.grow = dbCharacter->grow;
        character.hair = dbCharacter->hair;
        character.face = dbCharacter->face;
        character.size = dbCharacter->size;
        character.job = dbCharacter->job;
        character.sex = dbCharacter->sex;
        character.mapId = dbCharacter->mapId;
        character.strength = dbCharacter->strength;
        character.dexterity = dbCharacter->dexterity;
        character.reaction = dbCharacter->reaction;
        character.intelligence = dbCharacter->intelligence;
        character.wisdom = dbCharacter->wisdom;
        character.luck = dbCharacter->luck;
        character.health = dbCharacter->health;
        character.mana = dbCharacter->mana;
        character.stamina = dbCharacter->stamina;
        std::memcpy(&character.equipment, &dbCharacter->equipment, sizeof(Equipment0403));
        std::memcpy(character.charName.data(), dbCharacter->charName.data(), character.charName.size() - 1);
        character.deleted = dbCharacter->deleted;
        character.nameChange = dbCharacter->nameChange;
        character.cloakBadge = dbCharacter->cloakBadge;
        SConnection::Send(&user->connection, &character, sizeof(character));
    }
}

unsigned u0x47A237 = 0x47A237;
unsigned u0x47A574 = 0x47A574;
void __declspec(naked) naked_0x47A231()
{
    __asm
    {
        // original
        add eax,-0x102
        push ebx

        cmp eax,0x17
        je case_0x119
        jmp u0x47A237

        case_0x119:
        pushad

        lea eax,[edi+0x2]
        push eax // name
        push esi // user
        call packet_character::name_available_handler
        add esp,0x8

        popad

        jmp u0x47A574
    }
}

void __declspec(naked) naked_0x492660()
{
    __asm
    {
        pushad

        push ebp // user
        call packet_character::send_warehouse
        add esp,0x4

        popad
        
        retn 0x4
    }
}

unsigned u0x47B9B8 = 0x47B9B8;
void __declspec(naked) naked_0x47B8FB()
{
    __asm
    {
        pushad
        
        push esi // packet
        push ebp // user
        call packet_character::send_character
        add esp,0x8

        popad

        jmp u0x47B9B8
    }
}

unsigned u0x47B3EC = 0x47B3EC;
unsigned u0x47BDCC = 0x47BDCC;
void __declspec(naked) naked_0x47B3E7()
{
    __asm
    {
        // original
        add eax,-0x401
        cmp eax,0xC
        je case_0x40D
        jmp u0x47B3EC

        case_0x40D:
        pushad

        push edi // packet
        push ebp // user
        call packet_character::send_name_available
        add esp,0x8

        popad

        mov al,0x1
        jmp u0x47BDCC
    }
}

void hook::packet_character()
{
    // CUser::PacketCharacter switch
    util::detour((void*)0x47A231, naked_0x47A231, 6);
    // CUser::SendCharacterBank
    util::detour((void*)0x492660, naked_0x492660, 6);
    // CUser::PacketUserDBChar case 0x403
    util::detour((void*)0x47B8FB, naked_0x47B8FB, 6);
    // CUser::PacketUserDBChar
    util::detour((void*)0x47B3E7, naked_0x47B3E7, 5);

    std::uint8_t character_data_size = 104;
    // CUser::PacketUserDBChar
    util::write_memory((void*)0x47B4EC, &character_data_size, 1);
    util::write_memory((void*)0x47B9C9, &character_data_size, 1);
}
