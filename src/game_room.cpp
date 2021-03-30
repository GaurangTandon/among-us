#include <iostream>
#include "game_room.h"
#include "resource_manager.h"
#include "task.h"

GameRoom::GameRoom(glm::vec2 pos, Texture2D sprite, std::bitset<4> doors, glm::vec2 size, glm::vec3 color) : GameObject(
        pos,
        size,
        sprite,
        color), isExitNode(false) {
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

    auto wallSprite = ResourceManager::GetTexture("wall");

    for (int i = 0; i < 4; i++) {
        if (not doors[i]) {
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
    for (auto &wall : walls) wall.Draw(renderer);
    for (auto &task : tasks) task.Draw(renderer);

    if (isExitNode) exitLabel.Draw(renderer);
}

void GameRoom::moveAll(const glm::vec2 &displace) {
    Position += displace;

    for (auto &wall : walls) wall.move(displace);
    for (auto &object : tasks) object.move(displace);

    if (isExitNode) exitLabel.move(displace);
}

bool GameRoom::wallOverlaps(const GameObject &object) {
    for (auto &wall : walls)
        if (wall.hasOverlap(object))
            return true;

    return false;
}

int GameRoom::overlapsTask(const GameObject &object) {
    for (auto &task : tasks)
        if (task.hasOverlap(object)) return task.type;
    return 0;
}

bool GameRoom::addTask(const glm::vec2 &position, int type) {
    const auto taskTexPath =
            type == 1 ? "enemy_killer" : type == 2 ? "powerup_release" : type == 3 ? "obstacle" : "points";
    const auto &taskTex = ResourceManager::GetTexture(taskTexPath);

    auto task = Task(position, taskTex, type);
    tasks.push_back(task);

    return true;
}

void GameRoom::removeTask(int type) {
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i].type == type) {
            tasks.erase(tasks.begin() + i);
            i--;
        }
    }
}

glm::vec2 GameRoom::getPelicanSize() {
    return glm::vec2(0.9f * SIZE[0], 0.4f * SIZE[1]);
}

void GameRoom::setExitNode() {
    isExitNode = true;
    tasks.clear();

    auto exitTex = ResourceManager::GetTexture("exit");
    auto pos = getCenterCoordinate();
    auto size = getPelicanSize();
    pos -= size / 2.0f;
    exitLabel = GameObject(pos, size, exitTex);
}

bool GameRoom::exitNodeOverlap(const GameObject &b) {
    return isExitNode and exitLabel.hasOverlap(b);
}

glm::vec2 GameRoom::getExitNodePosition() {
    return exitLabel.Position;
}

void GameRoom::removeExit() {
    isExitNode = false;
}
