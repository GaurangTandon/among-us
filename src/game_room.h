#ifndef ASSIGNMENT_GAME_ROOM_H
#define ASSIGNMENT_GAME_ROOM_H

#include "resource_manager.h"
#include "game_object.h"
#include "task.h"
#include <vector>
#include <bitset>

class GameRoom : public GameObject {
private:
    std::vector<GameObject> walls;
    std::vector<Task> tasks;
    std::vector<Powerup> powerups;
    bool isExitNode;
    GameObject exitLabel;

public:
    constexpr static glm::vec2 SIZE = glm::vec2(200.0f, 200.0f);

    GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size = SIZE,
             glm::vec3 color = glm::vec3(1.0f));

    void Draw(SpriteRenderer &renderer) override;

    void DrawAddons(SpriteRenderer &renderer);

    void moveAll(const glm::vec2 &displace);

    bool wallOverlaps(const GameObject &object);

    int overlapsTask(const GameObject &object);

    bool addTask(int type);

    void removeTask(int type);

    void setExitNode();

    bool exitNodeOverlap(const GameObject &b);

    glm::vec2 getExitNodePosition();

    static glm::vec2 getPelicanSize();

    void removeExit();
};


#endif //ASSIGNMENT_GAME_ROOM_H
