#ifndef HALL_H
#define HALL_H

#include <QObject>
#include "room.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

struct RoomInfo {
    QString name;
    bool hasPassword;
    int blind;
    int maxPlayers;
    int players;
};

class Hall
{
    Hall();
public:
    QList<Room*> rooms;
    static Hall &instance();
    QJsonArray getRooms();
    bool createRoom(QString name, QString password, int maxPlayers);
    bool enterRoom(QString username, int money, QString name, QString password);
    bool leaveRoom(QString username, QString roomName);
    Room *getRoom(QString roomName);
};

#endif // HALL_H
