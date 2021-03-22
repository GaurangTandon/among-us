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

// a contains b
bool checkInside(const GameObject &a, const GameObject &b) {
    bool inside = true;

    for (int i = 0; i < 2; i++) {
        inside &= a.Position[i] <= b.Position[i];
        inside &= a.Position[i] + a.Size[i] >= b.Position[i] + b.Size[i];
    }

    return inside;
}

// a and b are disjoint
bool checkOutside(const GameObject &a, const GameObject &b) {
    for (int i = 0; i < 2; i++) {
        if (a.Position[i] + a.Size[i] <= b.Position[i]) return true;
        if (b.Position[i] + b.Size[i] <= a.Position[i]) return true;
    }

    return false;
}
