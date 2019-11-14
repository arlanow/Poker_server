#include "hall.h"

Hall::Hall()
{

}

Hall &Hall::instance()
{
    static Hall instance;
    return instance;
}

QJsonArray Hall::getRooms()
{
    auto result = QJsonArray();
    for (auto room: rooms) {
        QJsonObject info;
        info.insert("name", room->name);
        info.insert("blind", room->blind);
        info.insert("players", room->players.count());
        info.insert("maxPlayers", room->maxPlayers);
        info.insert("hasPassword", !room->password.isEmpty());
        result.append(info);
    }
    return result;
}

bool Hall::createRoom(QString name, QString password, int maxPlayers)
{
    for (auto room:rooms) {
        if (room->name == name) return false;
    }
    auto room = new Room(name, password, maxPlayers);
    rooms.append(room);
    return true;
}

bool Hall::enterRoom(QString username, int money, QString name, QString password)
{
    for (auto room:rooms) {
        if (room->name == name)
        if (room->password == password) {
            if (!room->gameTimer->isActive()) {
                if (Users::instance().decrementMoney(username, money)) {
                    return room->addPlayer(username, money);
                } else return false;
            }
        }
    }
    return false;
}

bool Hall::leaveRoom(QString username, QString roomName)
{
    for (auto room:rooms) {
        if (room->name == roomName)
            Users::instance().incrementMoney(username, room->leave(username));
    }
    return true;
}

Room *Hall::getRoom(QString roomName)
{
    for (auto room:rooms) {
        if (room->name == roomName)
            return room;
    }
    return nullptr;
}
