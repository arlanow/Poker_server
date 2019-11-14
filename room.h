#ifndef ROOM_H
#define ROOM_H

#include <QObject>
#include "users.h"
#include <QJsonArray>
#include <QJsonObject>
#include "player.h"
#include <QRandomGenerator>
#include <QTimer>

#define FinalStep 4

class Room: public QObject
{
    Q_OBJECT

public:
    QTimer* gameTimer;
    Room();
    Room(QString name, QString password, int maxPlayers);
    bool addPlayer(QString username, int money);
    QJsonObject getInfo(QString username);
    void nextTurn();
    void findWinner();
    int calcScore(QList<int> cards);
    void endGame();
    bool checkAccess(QString username);
    int getMinimumBet(QString username);
    bool bet(QString username, int money);
    bool fold(QString username);
    bool call(QString username);
    void genCardPool();
    void newGame();
    bool gameStart();
    bool playersTurn(QString username);
    void next();
    int leave(QString username);
    QList<QString> lastWinners;
    QString name;
    QString password;
    int maxPlayers;
    QList<int> cardsPool;
    int freeMoney = 0;

    //QList<Player> players;
    QMap<QString, Player*> players;
    QList<int> cards;
    int step = 0;
    int game = 0;
    int dealer = 0;
    int blind = 2;
    int betTurn = 0;
public slots:
    void timeout();

};

#endif // ROOM_H
