#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

class Player
{

public:
    Player(int money);
    int money;
    QPair<int, int> cards = QPair<int, int>(0,0);
    int currentBet = 0;
    bool bet(int money);
    void incrementMoney(int money);
    bool decrementMoney(int money);
};

#endif // PLAYER_H
