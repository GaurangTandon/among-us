#ifndef ASSIGNMENT_GAME_ROOM_H
#define ASSIGNMENT_GAME_ROOM_H

#include "game_object.h"
#include <vector>
#include <bitset>

class GameRoom : public GameObject {
private:
    std::vector<GameObject> doors;

public:
    constexpr static glm::vec2 SIZE = glm::vec2(200.0f, 200.0f);

    GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size = SIZE,
             glm::vec3 color = glm::vec3(1.0f));

    void Draw(SpriteRenderer &renderer) override;

    void DrawAddons(SpriteRenderer &renderer);
};


#endif //ASSIGNMENT_GAME_ROOM_H
