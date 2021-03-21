#include "game_room.h"
#include "resource_manager.h"

GameRoom::GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size, glm::vec3 color) : GameObject(
        pos,
        size,
        sprite,
        color) {

    std::vector<glm::vec2> door_offsets = {
            glm::vec2(0, 0.5),
            glm::vec2(1, 0.5),
            glm::vec2(0.5, 0),
            glm::vec2(0.5, 1)
    };

    auto doorSprite = ResourceManager::GetTexture("door");

    for (int i = 0; i < 4; i++) {
        if (doors[i]) {
            auto &offset = door_offsets[i];
            auto doorPos = pos + glm::vec2(offset[0] * size[0], offset[1] * size[1]);

            this->doors.emplace_back(doorPos, glm::vec2(50, 50), doorSprite);
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

    for (auto &door : doors) {
        door.Position += displace;
    }
}