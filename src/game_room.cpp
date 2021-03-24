#include <iostream>
#include "game_room.h"
#include "resource_manager.h"
#include "task.h"

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
    std::vector<glm::vec2> wallOffset = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0, 0.98f),
            glm::vec2(0, 0),
            glm::vec2(0.99f, 0)
    };
    const glm::vec2 HORIZONTAL_WALL_SIZE(5.0f, GameRoom::SIZE[1]);
    const glm::vec2 VERTICAL_WALL_SIZE(GameRoom::SIZE[0], 5.0f);

    auto doorSprite = ResourceManager::GetTexture("door");
    auto wallSprite = ResourceManager::GetTexture("wall");

    for (int i = 0; i < 4; i++) {
        if (doors[i]) {
            auto &offset = door_offsets[i];
            // https://stackoverflow.com/questions/13901119
            auto doorPos = pos + offset * size;

            for (int j = 0; j < 2; j++)
                if (offset[j] == 1) doorPos[j] -= DOOR_SIZE[j];
                else if (offset[j] > 0) doorPos[j] -= DOOR_SIZE[j] / 2;

            this->doors.emplace_back(i, doorPos, doorSprite);
        } else {
            auto wallPos = pos + wallOffset[i] * size;
            const auto &wallSize = i > 1 ? HORIZONTAL_WALL_SIZE : VERTICAL_WALL_SIZE;
            this->walls.emplace_back(wallPos, wallSize, wallSprite);
        }
    }
}

void GameRoom::Draw(SpriteRenderer &renderer) {
    GameObject::Draw(renderer);
}

void GameRoom::DrawAddons(SpriteRenderer &renderer) {
    for (auto &door : doors) door.Draw(renderer);
    for (auto &wall : walls) wall.Draw(renderer);
    for (auto &task : tasks) task.Draw(renderer);
}

void GameRoom::moveAll(const glm::vec2 &displace) {
    Position += displace;

    for (auto &door : doors) door.Position += displace;
    for (auto &wall : walls) wall.Position += displace;
    for (auto &object : tasks) object.Position += displace;
}


bool has_overlap(const GameObject &a, const GameObject &b) {
    float intersectArea = a.areaOverlap(b);
    return intersectArea >= 0.35 * b.area();
}

bool GameRoom::doorAllowsObject(const GameObject &object, int idx) {
    for (auto &door : doors)
        if ((idx == -1 or idx == door.idx) and has_overlap(door, object))
            return true;
    return false;
}

bool GameRoom::wallOverlaps(const GameObject &object) {
    for (auto &wall : walls)
        if (wall.hasOverlap(object))
            return true;

    return false;
}

glm::vec2 GameRoom::getDoorPosition(int idx) {
    for (auto &door : doors)
        if (door.idx == idx)
            return door.Position;

    assert(false);
}

int GameRoom::overlapsTask(const GameObject &object) {
    for (auto &task : tasks)
        if (task.hasOverlap(object)) return task.type;
    return 0;
}

bool GameRoom::addTask(const glm::vec2 &position, int type) {
    const auto &taskTex = ResourceManager::GetTexture((type == 1) ? "wall" : "wall2");

    auto task = Task(position, taskTex, type);
    tasks.push_back(task);

    return true;
}

glm::vec2 GameRoom::getCenterCoordinate() {
    return Position + Size / 2.0f;
}

void GameRoom::removeTask(int type) {
    // TODO: should check the type in case of bugs
    tasks.clear();
}