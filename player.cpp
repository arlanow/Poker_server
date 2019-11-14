#include "player.h"

Player::Player(int money): money(money)
{

}

bool Player::bet(int money)
{
    if (decrementMoney(money-currentBet)) {
        currentBet = money;
        return true;
    }
    return false;
}

void Player::incrementMoney(int money)
{
    this->money += money;
}

bool Player::decrementMoney(int money)
{
    if (this->money>=money) {
        this->money -= money;
        return true;
    } else return false;
}
