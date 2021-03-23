#ifndef ASSIGNMENT_GAME_ROOM_H
#define ASSIGNMENT_GAME_ROOM_H

#include "game_object.h"
#include <vector>
#include <bitset>

const glm::vec2 DOOR_SIZE = glm::vec2(50, 50);

class RoomDoor : public GameObject {
public:
    int idx;

    RoomDoor(int door_idx, glm::vec2 position, Texture2D texture) : idx(door_idx),
                                                                    GameObject(position, DOOR_SIZE, texture) {

    }
};

class GameRoom : public GameObject {
private:
    std::vector<RoomDoor> doors;
    std::vector<GameObject> walls;
    std::vector<GameObject> staticObjects;

public:
    constexpr static glm::vec2 SIZE = glm::vec2(200.0f, 200.0f);

    GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size = SIZE,
             glm::vec3 color = glm::vec3(1.0f));

    void Draw(SpriteRenderer &renderer) override;

    void DrawAddons(SpriteRenderer &renderer);

    void moveAll(const glm::vec2 &displace);

    bool doorAllowsObject(const GameObject &object, int idx = -1);

    glm::vec2 getDoorPosition(int idx);

    bool wallOverlaps(const GameObject &object);
};


#endif //ASSIGNMENT_GAME_ROOM_H
