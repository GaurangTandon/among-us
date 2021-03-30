#include "player.h"

const int HEALTH_DELTA = 10;

bool Player::isDead() {
    return hasEnemyHit or health < 0;
}

int Player::getHealth() {
    return health;
}

// booleans true if keys wasd pressed
void Player::update(bool wOrS, bool a, bool d) {
    assert(a + d <= 1);

    bool moving = wOrS or a or d;
    if (moving) {
#define WAIT 4
        timer++;
        timer %= movingSps.size() * WAIT;
        this->Sprite = movingSps[timer / WAIT];
    } else if (timer > 0) {
        timer = 0;
        this->Sprite = restSp;
    }

    if (a) {
        flipped = true;
    } else if (d) {
        flipped = false;
    }
}

void Player::enemyHit() {
    hasEnemyHit = true;
}

void Player::hitObstacle() {
    health -= HEALTH_DELTA;
}

void Player::pointsBoost() {
    health += HEALTH_DELTA;
}
