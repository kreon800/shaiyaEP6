#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <include/main.h>
#include <include/util.h>
#include <include/shaiya/packets/050F.h>
#include <include/shaiya/packets/0511.h>
#include <include/shaiya/include/CDamage.h>
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
        UseSkillResponse response{};
        response.senderId = user->id;
        response.targetId = user->id;
        response.skillId = skillInfo->skillId;
        response.skillLv = skillInfo->skillLv;

        if (!user->toggleSkill.triggered)
        {
            response.targetType = static_cast<std::uint8_t>(skillInfo->targetType);
            response.toggleState = ToggleState::Triggered;

            user->toggleSkill.triggered = true;
            user->toggleSkill.skillId = response.skillId;
            user->toggleSkill.skillLv = response.skillLv;
            user->toggleSkill.keepTime = GetTickCount() + (skillInfo->keepTime * 1000);

            SConnection::Send(&user->connection, &response, sizeof(UseSkillResponse));
            CUser::AddApplySkillBuff(user, skillInfo);

            auto percentage = (user->health * skillInfo->ability[0].value) / 100;
            user->health -= percentage;
            CUser::SendRecoverSet(user, user->health, user->stamina, user->mana);
            return;
        }
        else
        {
            response.targetType = static_cast<std::uint8_t>(skillInfo->targetType);
            response.toggleState = ToggleState::Stopped;

            user->toggleSkill.triggered = false;
            user->toggleSkill.skillId = 0;
            user->toggleSkill.skillLv = 0;
            user->toggleSkill.keepTime = 0;

            SConnection::Send(&user->connection, &response, sizeof(UseSkillResponse));
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
        UseSkillResponse response{};
        response.targetType = util::read_bytes<std::uint8_t>(buffer, 2);
        response.senderId = sender->id;
        response.targetId = target->id;
        response.skillId = util::read_bytes<std::uint16_t>(buffer, 11);
        response.skillLv = util::read_bytes<std::uint8_t>(buffer, 13);
        response.health = util::read_bytes<std::uint16_t>(buffer, 14);
        response.stamina = util::read_bytes<std::uint16_t>(buffer, 16);
        response.mana = util::read_bytes<std::uint16_t>(buffer, 18);

        switch (response.skillId)
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

        CZone::PSendView(sender->zone, &response, sizeof(UseSkillResponse), &sender->pos, 60, sender->id, target->id, 5);
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
