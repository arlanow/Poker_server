#include "users.h"

Users::Users()
{
}

Users &Users::instance()
{
    static Users instance;
    return instance;
}

QString Users::registerUser(QString name, QString password)
{
    for (auto user: usersList) {
        if (user.name == name) return "taken";
    }
    QString accessToken = QCryptographicHash::hash(QString("%1_%2_%3").arg(name).arg(password).arg(1/*QRandomGenerator::system()->bounded(10000)*/).toLatin1(),QCryptographicHash::Md5).toBase64().left(22);
    usersList.append(User{nextId++,name,password,accessToken,5000});
    return accessToken;
}

QString Users::login(QString name, QString password)
{
    for (auto i=0; i<usersList.count(); i++) {
        if (usersList.at(i).name == name) {
           if (usersList.at(i).password == password) {
               QString accessToken = QCryptographicHash::hash(QString("%1_%2_%3").arg(name).arg(password).arg(1/*QRandomGenerator::system()->bounded(10000)*/).toLatin1(),QCryptographicHash::Md5).toBase64().left(22);
               auto user = usersList.at(i);
               user.accessToken = accessToken;
               usersList.insert(i,user);
               qDebug() << usersList.at(i).accessToken;
               return accessToken;
           }
        }
    }
    return "incorrect";
}

bool Users::checkAccessToken(QString name, QString accessToken)
{
    for (auto user: usersList) {
        if (user.name == name) {
            if (user.accessToken == accessToken) return true;
        }
    }
    return false;
}

QString Users::getUserName(int id)
{
    for (auto user:usersList) {
        if (user.id == id) return user.name;
    }
    return "noone";
}

int Users::getUserMoney(QString name)
{
    for (auto user: usersList) {
        if (user.name == name) {
            return user.money;
        }
    }
    return -1;
}

void Users::incrementMoney(QString name, int money)
{
    User user;
    for (auto i=0; i<usersList.count(); i++) {
        if (usersList.at(i).name == name) {
            user = usersList.takeAt(i);
            break;
        }
    }
    user.money += money;
    usersList.append(user);
}

bool Users::decrementMoney(QString name, int money)
{
    User user;
    for (auto i=0; i<usersList.count(); i++) {
        if (usersList.at(i).name == name) {
            user = usersList.takeAt(i);
            break;
        }
    }
    if (user.money>=money) {
        user.money -= money;
        usersList.append(user);
        return true;
    } else {
        usersList.append(user);
        return false;
    }
}
