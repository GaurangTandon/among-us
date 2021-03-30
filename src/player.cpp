#include "player.h"

bool Player::isDead() {
    return dead;
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
    dead = true;
}