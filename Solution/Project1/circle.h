#pragma once

#include "aabb.h"
#include "raylib.h"

//------------------------------------------------------------------------------------
// Types and Structures Definition
//------------------------------------------------------------------------------------
typedef struct Circle {
    Vector2 center;     // Center position
    float radius;       // Circle radius
} Circle;

//------------------------------------------------------------------------------------
// Functions Declaration
//------------------------------------------------------------------------------------
Circle Circle_Create(float x, float y, float radius);
bool Circle_CheckCollisionCircle(Circle a, Circle b);
bool Circle_CheckCollisionPoint(Circle c, Vector2 point);
bool Circle_CheckCollisionAABB(Circle c, AABB box);
