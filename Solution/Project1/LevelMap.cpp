#include "LevelMap.hpp"
#include "Player.hpp"

Block::Block(float x, float y, float width, float height, bool ground) {
    rect = { x, y, width, height };
    this->isGround = ground;
    this->type = BlockType::NORMAL;
    this->rotation = 0.0f;
}

Block::Block(float x, float y, float width, float height, BlockType type) {
    rect = { x, y, width, height };
    this->isGround = true;
    this->type = type;
    this->rotation = 0.0f;
}

void Block::Draw() {
    if (type == BlockType::RAMP_UP) {
        // Rampa que sube a la derecha - triángulo
        Vector2 points[3] = {
            { rect.x, rect.y + rect.height },
            { rect.x, rect.y },
            { rect.x + rect.width, rect.y + rect.height }
        };
        DrawTriangle(points[0], points[1], points[2], ColorAlpha(ORANGE, 0.8f));
        DrawTriangleLines(points[0], points[1], points[2], DARKGRAY);
    }
    else if (type == BlockType::RAMP_DOWN) {
        // Rampa que baja a la derecha - triángulo invertido
        Vector2 points[3] = {
            { rect.x, rect.y + rect.height },
            { rect.x + rect.width, rect.y },
            { rect.x + rect.width, rect.y + rect.height }
        };
        DrawTriangle(points[0], points[1], points[2], ColorAlpha(ORANGE, 0.8f));
        DrawTriangleLines(points[0], points[1], points[2], DARKGRAY);
    }
    else {
        Color color = isGround ? ColorAlpha(BLUE, 0.5f) : ColorAlpha(GREEN, 0.5f);
        DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);
        DrawRectangleLinesEx(rect, 1, DARKGRAY);
    }
}

void Block::Draw(float rotation) {
    Draw();  // Ignorar rotación para rampas
}

float Block::GetHeightAtX(float playerX) const {
    // Calcular posición relativa dentro de la rampa (0 = izquierda, 1 = derecha)
    float t = (playerX - rect.x) / rect.width;
    if (t < 0) t = 0;
    if (t > 1) t = 1;

    if (type == BlockType::RAMP_UP) {
        // Rampa que sube hacia la derecha
        // En X=rect.x (izquierda) -> altura = rect.y + rect.height (abajo)
        // En X=rect.x+width (derecha) -> altura = rect.y (arriba)
        return rect.y + rect.height - (t * rect.height);
    }
    else {
        // Rampa que baja hacia la derecha
        // En X=rect.x (izquierda) -> altura = rect.y (arriba)
        // En X=rect.x+width (derecha) -> altura = rect.y + rect.height (abajo)
        return rect.y + (t * rect.height);
    }
}

Rectangle Block::GetRect() const {
    return rect;
}