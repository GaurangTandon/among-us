#include <iostream>
#include "player.h"


void Player::hit(int time) {
    if (inCooldownTimeBased(time)) return;

    lastHitTime = time;
    health -= DAMAGE;
}

bool Player::inCooldownTimeBased(int time) {
    auto diff = time - lastHitTime;
    return diff <= COOLDOWN_TIME and diff >= 0;
}

bool Player::isDead() {
    return health == 0;
}

int Player::getHealth() {
    return health;
}

void Player::update(int currentTime) {
    bool stateNow = inCooldownTimeBased(currentTime), statePrev = inCooldownTimeBased(currentTime - 1);
    if (stateNow == statePrev) return;

    if (stateNow) Sprite = hitSprite;
    else Sprite = normalSprite;
}

