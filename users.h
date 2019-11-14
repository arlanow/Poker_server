#ifndef USERS_H
#define USERS_H

#include <QObject>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDebug>

struct User {
    int id;
    QString name;
    QString password;
    QString accessToken;
    int money;
};

class Users
{
    Users();
    int nextId = 0;
public:
    QList<User> usersList;
    static Users &instance();
    QString registerUser(QString name, QString password);
    QString login(QString name, QString password);
    bool checkAccessToken(QString name, QString accessToken);
    QString getUserName(int id);
    int getUserMoney(QString name);
    void incrementMoney(QString name, int money);
    bool decrementMoney(QString name, int money);
};

#endif // USERS_H
