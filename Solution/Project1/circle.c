#include "circle.h"
#include "raymath.h"

// Create a Circle from position and radius
Circle Circle_Create(float x, float y, float radius)
{
    Circle result;
    result.center = (Vector2){ x, y };
    result.radius = radius;
    return result;
}

// Check collision between two circles
bool Circle_CheckCollisionCircle(Circle a, Circle b)
{
    Vector2 diff = Vector2Subtract(a.center, b.center);
    float distanceSq = Vector2LengthSqr(diff);

    float radiusSum = a.radius + b.radius;
    return distanceSq <= radiusSum * radiusSum;
}

// Check if a point is inside a circle
bool Circle_CheckCollisionPoint(Circle c, Vector2 point)
{
    Vector2 diff = Vector2Subtract(point, c.center);
    return Vector2LengthSqr(diff) <= c.radius * c.radius;
}

// Check collision between a circle and an AABB
bool Circle_CheckCollisionAABB(Circle c, AABB box)
{
    // Find the closest point on the AABB to the circle center
    Vector2 closest;
    closest.x = Clamp(c.center.x, box.position.x, box.position.x + box.size.x);
    closest.y = Clamp(c.center.y, box.position.y, box.position.y + box.size.y);

    // Calculate squared distance from closest point to circle center
    Vector2 diff = Vector2Subtract(c.center, closest);
    float distanceSq = Vector2LengthSqr(diff);

    return distanceSq <= c.radius * c.radius;
}
