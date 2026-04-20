#pragma once
#include <raylib.h>

enum class SoldierState {
    IDLE,
    WALKING,
    SNEAK,
    ATTACKING,
    DEAD,
    BOMB
};