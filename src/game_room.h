#ifndef ASSIGNMENT_GAME_ROOM_H
#define ASSIGNMENT_GAME_ROOM_H

#include "game_object.h"
#include <vector>
#include <bitset>

class GameRoom : public GameObject {
private:
    std::vector<GameObject> doors;

public:
    GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size = glm::vec2(1.0f),
             glm::vec3 color = glm::vec3(1.0f));

    void Draw(SpriteRenderer &renderer) override;
};


#endif //ASSIGNMENT_GAME_ROOM_H
