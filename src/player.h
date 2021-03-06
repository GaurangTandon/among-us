#ifndef AMONGUS_PLAYER_H
#define AMONGUS_PLAYER_H


#include <utility>
#include <vector>
#include "game_object.h"

const glm::vec2 PLAYER_SIZE = glm::vec2(35.0f, 40.0f);

class Player : public GameObject {
private:
    int health;
    bool hasEnemyHit;
    int timer;

    Texture2D restSp;
    std::vector<Texture2D> movingSps;

public:
    int currRoom;

    Player(int room, glm::vec2 pos, Texture2D restSprite, std::vector<Texture2D> movingSprites) : GameObject(pos,
                                                                                                             PLAYER_SIZE,
                                                                                                             restSprite),
                                                                                                  currRoom(room),
                                                                                                  timer(0), health(50),
                                                                                                  hasEnemyHit(false),
                                                                                                  restSp(restSprite),
                                                                                                  movingSps(std::move(
                                                                                                          movingSprites)) {
    }


    bool isDead();

    void enemyHit();

    int getHealth();

    void update(bool, bool, bool);

    void hitObstacle();

    void pointsBoost();
};


#endif //AMONGUS_PLAYER_H
