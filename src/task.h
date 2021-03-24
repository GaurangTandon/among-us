#ifndef AMONGUS_TASK_H
#define AMONGUS_TASK_H

#include "game_object.h"

const glm::vec2 TASK_SIZE = glm::vec2(50, 50);

class Task : public GameObject {
public:
    int type;

    Task(glm::vec2 pos, Texture2D texture, int t) : GameObject(pos, TASK_SIZE, texture), type(t) {
    }
};

class Powerup : public GameObject {
public:
    int score;

    Powerup(glm::vec2 pos, Texture2D texture, int s) : GameObject(pos, TASK_SIZE, texture), score(s) {
    }
};

#endif //AMONGUS_TASK_H
