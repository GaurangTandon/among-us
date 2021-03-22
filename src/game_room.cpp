#include "game_room.h"
#include "resource_manager.h"

GameRoom::GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size, glm::vec3 color) : GameObject(
        pos,
        size,
        sprite,
        color) {
    std::vector<glm::vec2> door_offsets = {
            glm::vec2(0.5, 0),
            glm::vec2(0.5, 1),
            glm::vec2(0, 0.5),
            glm::vec2(1, 0.5)
    };

    auto doorSprite = ResourceManager::GetTexture("door");
    auto doorSize = glm::vec2(50, 50);

    for (int i = 0; i < 4; i++) {
        if (doors[i]) {
            auto &offset = door_offsets[i];
            auto doorPos = pos + glm::vec2(offset[0] * size[0], offset[1] * size[1]);

            for (int j = 0; j < 2; j++)
                if (offset[j] == 1) doorPos[j] -= doorSize[j];
                else if (offset[j] > 0) doorPos[j] -= doorSize[j] / 2;

            this->doors.emplace_back(doorPos, doorSize, doorSprite);
        }
    }
}

void GameRoom::Draw(SpriteRenderer &renderer) {
    GameObject::Draw(renderer);
}

void GameRoom::DrawAddons(SpriteRenderer &renderer) {
    for (auto &door : doors) door.Draw(renderer);
}


void GameRoom::moveAll(const glm::vec2 &displace) {
    Position += displace;

    for (auto &door : doors) door.Position += displace;
}

float areaOverlap(const GameObject &a, const GameObject &b) {
    float prod = 1;

    for (int i = 0; i < 2; i++) {
        float x = std::fmax(a.Position[i], b.Position[i]);
        float y = std::fmin(a.Position[i] + a.Size[i], b.Position[i] + b.Size[i]);
        prod *= std::fmax(y - x, 0.0f);
    }

    return prod;
}

bool GameRoom::doorAllowsObject(const GameObject &object) {
    for (auto &door : doors) {
        float intersectArea = areaOverlap(door, object);
        if (intersectArea >= 0.45 * object.area()) return true;
    }
    return false;
}
