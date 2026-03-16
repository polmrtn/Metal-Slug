#pragma once

#include "raylib.h"

typedef struct AABB {
    Vector2 position;   // Top-left corner
    Vector2 size;       // Width and height
} AABB;

AABB AABB_Create(float x, float y, float width, float height);
bool AABB_CheckCollisionAABB(AABB a, AABB b);
bool AABB_CheckCollisionPoint(AABB a, Vector2 point);

