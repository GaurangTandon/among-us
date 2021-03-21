#include "game_room.h"

GameRoom::GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size, glm::vec3 color) : GameObject(pos,
                                                                                                                 size,
                                                                                                                 sprite,
                                                                                                                 color) {
    std::vector<glm::vec2> door_offsets = {
            glm::vec2(0.5, 0),
            glm::vec2(1, 0.5),
            glm::vec2(0, 0.5),
            glm::vec2(1, 1)
    };

    Texture2D doorSprite;

    for (int i = 0; i < 4; i++) {
        if (doors[i]) {
            auto &offset = door_offsets[i];
            auto doorPos = pos + offset;
            this->doors.emplace_back(doorPos, glm::vec2(1, 1), doorSprite);
        }
    }
}

void GameRoom::Draw(SpriteRenderer &renderer) {
    GameObject::Draw(renderer);

    for (auto &door : doors) door.Draw(renderer);
}