#include "game_object.h"

//GameObject::GameObject()
//        : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
        : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite) {}

void GameObject::Draw(SpriteRenderer &renderer) {
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

float GameObject::area() const {
    return this->Size[0] * this->Size[1];
}

float GameObject::areaOverlap(const GameObject &b) const {
    float prod = 1;

    for (int i = 0; i < 2; i++) {
        float x = std::fmax(this->Position[i], b.Position[i]);
        float y = std::fmin(this->Position[i] + this->Size[i], b.Position[i] + b.Size[i]);
        prod *= std::fmax(y - x, 0.0f);
    }

    return prod;
}

bool GameObject::hasOverlap(const GameObject &b) const {
    return areaOverlap(b) >= 0.05 * b.area();
}
