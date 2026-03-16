#include "aabb.h"

// Create an AABB from position and dimensions
AABB AABB_Create(float x, float y, float width, float height)
{
    AABB result;
    result.position = (Vector2){ x, y };
    result.size = (Vector2){ width, height };
    return result;
}

// Check collision between two AABBs (overlap test on both axes)
bool AABB_CheckCollisionAABB(AABB a, AABB b)
{
    bool collisionX = (a.position.x < b.position.x + b.size.x) && (a.position.x + a.size.x > b.position.x);
    bool collisionY = (a.position.y < b.position.y + b.size.y) && (a.position.y + a.size.y > b.position.y);
    return collisionX && collisionY;
}

// Check if a point is inside an AABB
bool AABB_CheckCollisionPoint(AABB a, Vector2 point)
{
    bool insideX = (point.x >= a.position.x) && (point.x <= a.position.x + a.size.x);
    bool insideY = (point.y >= a.position.y) && (point.y <= a.position.y + a.size.y);
    return insideX && insideY;
}
