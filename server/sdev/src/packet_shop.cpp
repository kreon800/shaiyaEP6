#include <array>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/2602.h>
#include <include/shaiya/include/CClientToMgr.h>
#include <include/shaiya/include/CGameData.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/SConnection.h>
#include <include/shaiya/include/ServerTime.h>
using namespace shaiya;

unsigned g_nPayLetterEnable = 0x58799C;

// CClientToMgr::OnRecv
// ebx = packet

#pragma pack(push, 1)
// this packet is sent from ps_session at address 0x402EF0
// CClientToMgr::OnRecv handles it at address 0x4069CB and returns to a loop at address 0x4ECF40
// the result is g_nPayLetterEnable being set to false because the PayLetter server is not implemented
struct PacketBuffer0105
{
    ULONG u0x00;
    void* p0x04;
    ULONG u0x08;
    ULONG u0x0C;
    bool enable;  // ebx+0x02
    // 0x11
};

// billing packets

struct PacketBuffer1B02
{
    ULONG u0x00;
    void* p0x04;
    ULONG u0x08;
    ULONG u0x0C;
    UserId userId;                // ebx+0x02
    ULONG charId;                 // ebx+0x06
    UINT8 result;                 // ebx+0x0A
    Array<char, 25> itemKey;      // ebx+0x0B
    UINT32 u0x32;                 // ebx+0x24
    Array<char, 16> orderNumber;  // ebx+0x28
    // user->id
    ULONG id;                     // ebx+0x38
    UINT32 u0x4A;                 // ebx+0x3C
    UINT32 itemCount;             // ebx+0x40
    // 0x52
};

struct PacketBuffer1B03
{
    ULONG u0x00;
    void* p0x04;
    ULONG u0x08;
    ULONG u0x0C;
    UserId userId;            // ebx+0x02
    ProductCode productCode;  // ebx+0x06
    CharName targetName;      // ebx+0x1B
    UINT32 itemPrice;         // ebx+0x30
    UINT32 points;            // ebx+0x34
    // 0x46
};
#pragma pack(pop)

namespace packet_shop
{
    void raise_event_0x105(bool enable)
    {
        PacketBuffer0105 packet{};
        packet.u0x00 = 0;
        packet.p0x04 = &packet.u0x0C;
        packet.u0x08 = 0;
        // 00 00 05 01
        packet.u0x0C = 0x1050000;
        packet.enable = enable;
        CClientToMgr::OnRecv(&packet);
    }

    void raise_event_0x1B02(CUser* user)
    {
        PacketBuffer1B02 packet{};
        packet.u0x00 = 0;
        packet.p0x04 = &packet.u0x0C;
        packet.u0x08 = 0;
        // 00 00 02 1B
        packet.u0x0C = 0x1B020000;
        packet.userId = user->userId;
        packet.charId = user->charId;
        CClientToMgr::OnRecv(&packet);
    }

    void raise_event_0x1B03(CUser* user, const char* targetName, const char* productCode, int itemPrice)
    {
        PacketBuffer1B03 packet{};
        packet.u0x00 = 0;
        packet.p0x04 = &packet.u0x0C;
        packet.u0x08 = 0;
        // 00 00 03 1B
        packet.u0x0C = 0x1B030000;
        packet.userId = user->userId;
        std::memcpy(&packet.productCode, productCode, packet.productCode.size());
        std::memcpy(&packet.targetName, targetName, packet.targetName.size());
        packet.itemPrice = itemPrice;
        packet.points = user->points;
        CClientToMgr::OnRecv(&packet);
    }

    void purchase_item_async(CUser* user)
    {
        std::thread([=] {
            raise_event_0x1B02(user);
            }).detach();
    }

    void send_present_async(CUser* user, const char* targetName, const char* productCode, int usePoint)
    {
        std::thread([=] {
            raise_event_0x1B03(user, targetName, productCode, usePoint);
            }).detach();
    }

    void set_pay_letter_enable_async(bool enable)
    {
        std::thread([=] {
            raise_event_0x105(enable);
            }).detach();
    }

    void send_purchase(CUser* user, Packet buffer)
    {
        constexpr int packet_size_without_list = 37;
        constexpr int item_size_without_dates = 5;

        ProductPurchaseResponse response{};
        response.opcode = util::read_bytes<std::uint16_t>(buffer, 0);
        response.result = util::read_bytes<ProductPurchaseResult>(buffer, 2);
        response.points = util::read_bytes<std::uint32_t>(buffer, 3);
        std::memcpy(&response.productCode, &buffer[7], response.productCode.size());
        response.purchaseDate = util::read_bytes<std::uint32_t>(buffer, 28);
        response.itemPrice = util::read_bytes<std::uint32_t>(buffer, 32);
        response.itemCount = util::read_bytes<std::uint8_t>(buffer, 36);

        int offset = 0;
        for (int i = 0; i < response.itemCount; ++i)
        {
            Item2602 item2602{};
            item2602.bag = util::read_bytes<std::uint8_t>(buffer, 37 + offset);
            item2602.slot = util::read_bytes<std::uint8_t>(buffer, 38 + offset);
            item2602.type = util::read_bytes<std::uint8_t>(buffer, 39 + offset);
            item2602.typeId = util::read_bytes<std::uint8_t>(buffer, 40 + offset);
            item2602.count = util::read_bytes<std::uint8_t>(buffer, 41 + offset);

            #ifdef SHAIYA_EP6
            auto itemInfo = CGameData::GetItemInfo(item2602.type, item2602.typeId);
            if (itemInfo)
            {
                item2602.toDate = ServerTime::GetItemExpireTime(response.purchaseDate, itemInfo);
                item2602.fromDate = item2602.toDate ? response.purchaseDate : 0;
            }
            #endif

            std::memcpy(&response.itemList[i], &item2602, sizeof(Item2602));
            offset += item_size_without_dates;
        }

        int length = packet_size_without_list + (response.itemCount * sizeof(Item2602));
        SConnection::Send(&user->connection, &response, length);
    }
}

unsigned u0x488D5F = 0x488D5F;
void __declspec(naked) naked_0x48876F()
{
    __asm
    {
        pushad

        push edi // user
        call packet_shop::purchase_item_async
        add esp,0x4

        popad

        // original
        jmp u0x488D5F
    }
}

unsigned u0x47D163 = 0x47D163;
void __declspec(naked) naked_0x47D151()
{
    __asm 
    {
        xor ebp,ebp
        add ebx,0x5998
        mov eax,[esi+0x6]
        // user->points
        mov[ecx+0x5AC0],eax
        jmp u0x47D163
    }
}

void __declspec(naked) naked_0x488A80()
{
    __asm
    {
        pushad

        mov eax,[esp+0x174]
        push eax // itemPrice
        lea eax,[esp+0x14E]
        push eax // productCode
        lea eax,[esp+0x167]
        push eax // targetName
        push edi // user
        call packet_shop::send_present_async
        add esp,0x10

        popad

        // original
        jmp u0x488D5F
    }
}

unsigned u0x47D3DC = 0x47D3DC;
void __declspec(naked) naked_0x47D3D7()
{
    __asm
    {
        // user->disableShop
        mov [ebx+0x5AC4],0x0
        // original
        mov al,0x1
        pop edi
        pop ebp
        pop ebx

        jmp u0x47D3DC
    }
}

unsigned u0x488709 = 0x488709;
void __declspec(naked) naked_0x4886E0()
{
    __asm
    {
        // purchaseDate
        mov [esp+0x188],esi
        // price
        mov [esp+0x18C],edx
        // itemCount
        mov byte ptr[esp+0x190],bl

        pushad

        lea ecx,[esp+0x18C]

        push ecx // packet
        push edi // user
        call packet_shop::send_purchase
        add esp,0x8

        popad

        jmp u0x488709
    }
}

void hook::packet_shop()
{
    // CUser::PacketShop case 0x2602
    util::detour((void*)0x48876F, naked_0x48876F, 5);
    // CUser::PacketUserDBPoint case 0xE01
    util::detour((void*)0x47D151, naked_0x47D151, 6);
    // CUser::PacketShop case 0x2603
    util::detour((void*)0x488A80, naked_0x488A80, 5);
    // CUser::PacketUserDBPoint case 0xE03
    util::detour((void*)0x47D3D7, naked_0x47D3D7, 5);

    // fake a 0x105 event
    packet_shop::set_pay_letter_enable_async(true);

    #ifdef SHAIYA_EP6
    // CUser::PacketShop case 0x2602
    util::detour((void*)0x4886E0, naked_0x4886E0, 5);
    #endif
}
