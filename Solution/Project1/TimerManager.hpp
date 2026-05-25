#pragma once

enum class TimerType {
    // runtime timers / counters
    SHOOT_TIMER,
    GRENADE_COOLDOWN,
    MACHINEGUN_BURST_TIMER,
    MACHINEGUN_SOUND_TIMER,
    CREDIT_COOLDOWN,

    // configured delays / constants
    DELAY_PISTOL,
    DELAY_MACHINEGUN,
    DELAY_GRENADE,
    MACHINEGUN_BURST_DELAY,
    CREDIT_DELAY,
    MACHINEGUN_SOUND_DELAY,

    // integer counters (returned as float via GetTimer)
    MACHINEGUN_SOUND_COUNT,
    MACHINEGUN_BURST_COUNT,
    MACHINEGUN_SOUND_SHOTS_CONST
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();

    // Advance timers by deltaTime (seconds)
    void Update(float deltaTime);

    // Returns the current value for the requested timer/constant as float
    float GetTimer(TimerType type) const;

    // Start a timer (sets current timer to the configured delay)
    void StartTimer(TimerType type);

    // Reset current timer to zero
    void ResetTimer(TimerType type);

    // Return true if a runtime timer is ready (<= 0.0f).
    // For constants/delays this returns true (they're not blocking).
    bool IsReady(TimerType type) const;

    // Explicit setter for a runtime timer or counter (use sparingly).
    void SetTimerValue(TimerType type, float value);

    void FullReset();

private:
    // runtime timers / counters
    float shootTimer = 0.0f;
    float grenadeCooldown = 0.0f;
    float machinegunBurstTimer = 0.0f;
    float machinegunSoundTimer = 0.0f;
    float creditCooldown = 0.0f;

    // configured delays / constants
    float shootDelayPistol = 0.15f;
    float shootDelayMachinegun = 0.5f;
    float grenadeDelay = 0.15f;
    float machinegunBurstDelay = 0.04f;
    float creditDelay = 0.3f;
    static constexpr float MACHINEGUN_SOUND_DELAY = 0.10f;

    // integer counters
    int machinegunSoundCount = 0;
    int machinegunBurstCount = 0;
    static constexpr int MACHINEGUN_SOUND_SHOTS = 4;
};