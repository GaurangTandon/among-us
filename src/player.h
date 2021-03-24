#ifndef AMONGUS_PLAYER_H
#define AMONGUS_PLAYER_H


#include "game_object.h"

const glm::vec2 PLAYER_SIZE = glm::vec2(40.0f, 40.0f);

class Player : public GameObject {
private:
    bool inCooldownTimeBased(int time);

    Texture2D normalSprite;
    Texture2D hitSprite;
    int health;
    int lastHitTime;

public:
    static constexpr int DAMAGE = 10;
    static constexpr int MAX_HEALTH = 50;
    static constexpr int COOLDOWN_TIME = 3;

    int currRoom;

    Player(int room, glm::vec2 pos, Texture2D nSprite, Texture2D hSprite) : GameObject(pos, PLAYER_SIZE, nSprite),
                                                                            currRoom(room), normalSprite(nSprite),
                                                                            hitSprite(hSprite), health(MAX_HEALTH),
                                                                            lastHitTime(-100) {
    }


    bool isDead();

    void hit(int time);

    void enemyHit();

    int getHealth();

    void update(int currenTime);
};


#endif //AMONGUS_PLAYER_H
