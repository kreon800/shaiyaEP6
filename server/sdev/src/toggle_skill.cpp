#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/0511.h>
#include <include/shaiya/include/CGameData.h>
#include <include/shaiya/include/CSkill.h>
#include <include/shaiya/include/CUser.h>
#include <include/shaiya/include/CZone.h>
#include <include/shaiya/include/SConnection.h>
using namespace shaiya;

namespace toggle_skill
{
    void send_frenzied_state(CUser* user, CGameData::SkillInfo* skillInfo)
    {
        SkillUseOutgoing packet{};
        packet.senderId = user->id;
        packet.targetId = user->id;
        packet.skillId = skillInfo->skillId;
        packet.skillLv = skillInfo->skillLv;

        if (!user->toggleSkill.triggered)
        {
            packet.targetType = static_cast<std::uint8_t>(skillInfo->targetType);
            #ifdef WITH_EXTENDED_0511
            packet.toggleState = ToggleState::Triggered;
            #endif

            user->toggleSkill.triggered = true;
            user->toggleSkill.skillId = packet.skillId;
            user->toggleSkill.skillLv = packet.skillLv;
            user->toggleSkill.keepTime = GetTickCount() + (skillInfo->keepTime * 1000);

            SConnection::Send(&user->connection, &packet, sizeof(SkillUseOutgoing));
            CUser::AddApplySkillBuff(user, skillInfo);

            auto percentage = (user->health * skillInfo->ability[0].value) / 100;
            user->health -= percentage;
            CUser::SendRecoverSet(user, user->health, user->stamina, user->mana);
            return;
        }
        else
        {
            packet.targetType = static_cast<std::uint8_t>(skillInfo->targetType);
            #ifdef WITH_EXTENDED_0511
            packet.toggleState = ToggleState::Stopped;
            #endif

            user->toggleSkill.triggered = false;
            user->toggleSkill.skillId = 0;
            user->toggleSkill.skillLv = 0;
            user->toggleSkill.keepTime = 0;

            SConnection::Send(&user->connection, &packet, sizeof(SkillUseOutgoing));
            CUser::RemApplySkillBuff(user, skillInfo);
            return;
        }
    }

    void maybe_send_damage(CUser* user)
    {
        if (!user)
            return;

        auto now = GetTickCount();
        if (!user->toggleSkill.triggered || now < user->toggleSkill.keepTime)
            return;

        auto skillInfo = CGameData::GetSkillInfo(user->toggleSkill.skillId, user->toggleSkill.skillLv);
        if (!skillInfo)
            return;

        auto percentage = (user->health * skillInfo->ability[0].value) / 100;
        user->health -= percentage;
        CUser::SendRecoverSet(user, user->health, user->stamina, user->mana);

        user->toggleSkill.keepTime = now + (skillInfo->keepTime * 1000);
    }

    void maybe_send_state(CUser* sender, CUser* target, CGameData::SkillInfo* skillInfo, Packet buffer)
    {
        SkillUseOutgoing packet{};
        packet.targetType = util::read_bytes<std::uint8_t>(buffer, 2);
        packet.senderId = sender->id;
        packet.targetId = target->id;
        packet.skillId = util::read_bytes<std::uint16_t>(buffer, 11);
        packet.skillLv = util::read_bytes<std::uint8_t>(buffer, 13);
        packet.health = util::read_bytes<std::uint16_t>(buffer, 14);
        packet.stamina = util::read_bytes<std::uint16_t>(buffer, 16);
        packet.mana = util::read_bytes<std::uint16_t>(buffer, 18);

        switch (packet.skillId)
        {
        case SkillId::FrenziedForce:
        case SkillId::FrenziedFocus:
        case SkillId::FrenziedFinesse:
        case SkillId::FrenziedFortune:
        {
            send_frenzied_state(sender, skillInfo);
            return;
        }
        default:
            break;
        }

        if (!sender->zone)
            return;

        CZone::PSendView(sender->zone, &packet, sizeof(SkillUseOutgoing), &sender->pos, 60, sender->id, target->id, 5);
    }

    bool is_toggle_skill(CSkill* skill)
    {
        switch (skill->skillId)
        {
        case SkillId::FrenziedForce:
        case SkillId::FrenziedFocus:
        case SkillId::FrenziedFinesse:
        case SkillId::FrenziedFortune:
            return true;
        default:
            break;
        }

        return false;
    }
}

unsigned u0x45CD11 = 0x45CD11;
void __declspec(naked) naked_0x45CCE3()
{
    __asm
    {
        pushad

        lea eax,[esp+0x3C]

        push eax // packet
        push esi // skillInfo
        push edi // target
        push ebp // user
        call toggle_skill::maybe_send_state
        add esp,0x10

        popad

        jmp u0x45CD11
    }
}

unsigned u0x493BCF = 0x493BCF;
unsigned u0x493C3F = 0x493C3F;
void __declspec(naked) naked_0x493BC6()
{
    __asm
    {
        pushad

        push ebx // skill
        call toggle_skill::is_toggle_skill
        add esp,0x4
        test al,al

        popad

        jne _0x493C3F

        // original
        mov edx,[ebp+0x5C]
        imul edx,edx,0x3E8
        jmp u0x493BCF

        _0x493C3F:
        jmp u0x493C3F
    }
}

unsigned u0x49DB20 = 0x49DB20;
unsigned u0x428ADA = 0x428ADA;
void __declspec(naked) naked_0x428AD5()
{
    __asm
    {
        // original
        call u0x49DB20

        pushad

        lea edx,[esi-0xD0]
        push edx // user
        call toggle_skill::maybe_send_damage
        add esp,0x4

        popad

        jmp u0x428ADA
    }
}

void hook::toggle_skill()
{
    // CUser::SkillAttackRange
    util::detour((void*)0x45CCE3, naked_0x45CCE3, 6);
    // CUser::AddApplySkill
    util::detour((void*)0x493BC6, naked_0x493BC6, 9);
    // CZone::UpdateApplySkill
    util::detour((void*)0x428AD5, naked_0x428AD5, 5);
}
