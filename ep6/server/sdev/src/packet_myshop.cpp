#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/230B.h>
#include <include/shaiya/include/CItem.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/MyShop.h>
#include <include/shaiya/include/SConnection.h>
#include <include/shaiya/include/ServerTime.h>
using namespace shaiya;

namespace packet_myshop
{
    void send_item_list(CUser* user, MyShop* myShop)
    {
        constexpr int max_shop_slot = 20;

        MyShopViewResponse myshop{};
        myshop.itemCount = 0;

        for (int slot = 0; slot < max_shop_slot; ++slot)
        {
            auto src_bag = myShop->srcBag[slot];
            auto src_slot = myShop->srcSlot[slot];

            if (!src_bag)
                continue;

            auto& item = myShop->user->inventory[src_bag][src_slot];
            if (!item)
                continue;

            Item230B myshop_item{};
            myshop_item.slot = slot;
            myshop_item.price = myShop->price[slot];
            myshop_item.type = item->type;
            myshop_item.typeId = item->typeId;
            myshop_item.count = item->count;
            myshop_item.quality = item->quality;
            myshop_item.gems = item->gems;

            #ifdef SHAIYA_EP6
            myshop_item.toDate = ServerTime::GetItemExpireTime(item->makeTime, item->itemInfo);
            myshop_item.fromDate = myshop_item.toDate ? item->makeTime : 0;
            #endif

            myshop_item.craftName = item->craftName;
            std::memcpy(&myshop.itemList[myshop.itemCount], &myshop_item, sizeof(Item230B));
            ++myshop.itemCount;
        }

        constexpr int packet_size_without_list = 3;
        int packet_size = packet_size_without_list + (myshop.itemCount * sizeof(Item230B));
        SConnection::Send(&user->connection, &myshop, packet_size);
    }
}

unsigned u0x4877A6 = 0x4877A6;
void __declspec(naked) naked_0x48779E()
{
    __asm
    {
        pushad

        push esi // myShop
        push ebp // user
        call packet_myshop::send_item_list
        add esp,0x8

        popad
        
        jmp u0x4877A6
    }
}

void hook::packet_myshop()
{
    // CUser::PacketMyShop
    util::detour((void*)0x48779E, naked_0x48779E, 8);
}
