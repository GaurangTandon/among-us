#ifndef AMONGUS_PLAYER_H
#define AMONGUS_PLAYER_H


#include "game_object.h"

const glm::vec2 PLAYER_SIZE = glm::vec2(40.0f, 40.0f);

class Player : public GameObject {
public:
    int currRoom;
    glm::vec2 roomOffset;

    Player(int room, glm::vec2 pos, Texture2D sprite) : GameObject(pos, PLAYER_SIZE, sprite), currRoom(room),
                                                        roomOffset(1.0f) {
    }
};


#endif //AMONGUS_PLAYER_H
