#pragma once
#include <raylib.h>
#include <vector>
#include "Player.hpp"

// Tipos de bloque
enum class BlockType {
    NORMAL,     // Bloque normal
    RAMP_UP,    // Rampa que sube hacia la derecha
    RAMP_DOWN   // Rampa que baja hacia la derecha
};

class Block {
public:
    Block(float x, float y, float width, float height, bool ground = true);
    Block(float x, float y, float width, float height, BlockType type);  // Constructor para rampas

    void Draw();
    void Draw(float rotation);  // Mantenemos por compatibilidad
    Rectangle GetRect() const;
    bool IsGround() const { return isGround; }
    BlockType GetType() const { return type; }
    bool IsRamp() const { return type != BlockType::NORMAL; }

    // Para compatibilidad con el editor (aunque no se use en rampas)
    void SetRotation(float rot) { rotation = rot; }
    float GetRotation() const { return rotation; }

    // Para rampas
    float GetHeightAtX(float playerX) const;

private:
    Rectangle rect;
    bool isGround;
    BlockType type = BlockType::NORMAL;
    float rotation = 0.0f;  // Para compatibilidad
};