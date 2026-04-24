#include "TimerManager.hpp"
#include <algorithm>

TimerManager::TimerManager() = default;
TimerManager::~TimerManager() = default;

void TimerManager::Update(float deltaTime)
{
    if (deltaTime <= 0.0f) return;
    shootTimer = std::max(0.0f, shootTimer - deltaTime);
    grenadeCooldown = std::max(0.0f, grenadeCooldown - deltaTime);
    machinegunBurstTimer = std::max(0.0f, machinegunBurstTimer - deltaTime);
    machinegunSoundTimer = std::max(0.0f, machinegunSoundTimer - deltaTime);
    creditCooldown = std::max(0.0f, creditCooldown - deltaTime);

}

float TimerManager::GetTimer(TimerType type) const
{
    switch (type)
    {
    // runtime timers
    case TimerType::SHOOT_TIMER:               return shootTimer;
    case TimerType::GRENADE_COOLDOWN:          return grenadeCooldown;
    case TimerType::MACHINEGUN_BURST_TIMER:    return machinegunBurstTimer;
    case TimerType::MACHINEGUN_SOUND_TIMER:    return machinegunSoundTimer;
    case TimerType::CREDIT_COOLDOWN:           return creditCooldown;

    // configured delays / constants
    case TimerType::DELAY_PISTOL:              return shootDelayPistol;
    case TimerType::DELAY_MACHINEGUN:          return shootDelayMachinegun;
    case TimerType::DELAY_GRENADE:             return grenadeDelay;
    case TimerType::MACHINEGUN_BURST_DELAY:    return machinegunBurstDelay;
    case TimerType::CREDIT_DELAY:              return creditDelay;
    case TimerType::MACHINEGUN_SOUND_DELAY:    return MACHINEGUN_SOUND_DELAY;

    // integer counters returned as float
    case TimerType::MACHINEGUN_SOUND_COUNT:    return static_cast<float>(machinegunSoundCount);
    case TimerType::MACHINEGUN_BURST_COUNT:    return static_cast<float>(machinegunBurstCount);
    case TimerType::MACHINEGUN_SOUND_SHOTS_CONST: return static_cast<float>(MACHINEGUN_SOUND_SHOTS);

    default:
        return 0.0f;
    }
}

void TimerManager::StartTimer(TimerType type)
{
    switch (type)
    {
    case TimerType::DELAY_PISTOL:
        shootTimer = shootDelayPistol;
        break;
    case TimerType::DELAY_MACHINEGUN:
        shootTimer = shootDelayMachinegun;
        break;
    case TimerType::DELAY_GRENADE:
        grenadeCooldown = grenadeDelay;
        break;
    case TimerType::MACHINEGUN_BURST_DELAY:
        machinegunBurstTimer = machinegunBurstDelay;
        break;
    case TimerType::CREDIT_DELAY:
        creditCooldown = creditDelay;
        break;
    case TimerType::MACHINEGUN_SOUND_DELAY:
        machinegunSoundTimer = MACHINEGUN_SOUND_DELAY;
        break;
    default:
        // counters/constants do nothing here
        break;
    }
}

void TimerManager::ResetTimer(TimerType type)
{
    switch (type)
    {
    case TimerType::SHOOT_TIMER:
        shootTimer = 0.0f;
        break;
    case TimerType::GRENADE_COOLDOWN:
        grenadeCooldown = 0.0f;
        break;
    case TimerType::MACHINEGUN_BURST_TIMER:
        machinegunBurstTimer = 0.0f;
        break;
    case TimerType::MACHINEGUN_SOUND_TIMER:
        machinegunSoundTimer = 0.0f;
        break;
    case TimerType::CREDIT_COOLDOWN:
        creditCooldown = 0.0f;
        break;
    case TimerType::MACHINEGUN_SOUND_COUNT:
        machinegunSoundCount = 0;
        break;
    case TimerType::MACHINEGUN_BURST_COUNT:
        machinegunBurstCount = 0;
        break;
    default:
        // constants / delays left untouched
        break;
    }
}

bool TimerManager::IsReady(TimerType type) const
{
    // For runtime timers: ready if <= 0
    switch (type)
    {
    case TimerType::SHOOT_TIMER:               return shootTimer <= 0.0f;
    case TimerType::GRENADE_COOLDOWN:          return grenadeCooldown <= 0.0f;
    case TimerType::MACHINEGUN_BURST_TIMER:    return machinegunBurstTimer <= 0.0f;
    case TimerType::MACHINEGUN_SOUND_TIMER:    return machinegunSoundTimer <= 0.0f;
    case TimerType::CREDIT_COOLDOWN:           return creditCooldown <= 0.0f;

    // For configured delays/constants treat as "always ready" (they are values)
    default: return true;
    }
}

void TimerManager::SetTimerValue(TimerType type, float value)
{
    switch (type)
    {
    case TimerType::SHOOT_TIMER:               shootTimer = value; break;
    case TimerType::GRENADE_COOLDOWN:          grenadeCooldown = value; break;
    case TimerType::MACHINEGUN_BURST_TIMER:    machinegunBurstTimer = value; break;
    case TimerType::MACHINEGUN_SOUND_TIMER:    machinegunSoundTimer = value; break;
    case TimerType::CREDIT_COOLDOWN:           creditCooldown = value; break;

    case TimerType::MACHINEGUN_SOUND_COUNT:    machinegunSoundCount = static_cast<int>(value); break;
    case TimerType::MACHINEGUN_BURST_COUNT:    machinegunBurstCount = static_cast<int>(value); break;

    default:
        // Do nothing for constants/delays
        break;
    }
}
