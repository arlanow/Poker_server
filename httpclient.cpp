#include "httpclient.h"

HTTPClient::HTTPClient(qint64 id, QObject *parent) : QObject(parent)
{
    this->id = id;
    statusCodes.insert(100, "100 Continue");
    statusCodes.insert(200, "200 OK");
    statusCodes.insert(201, "201 Created");
    statusCodes.insert(404, "404 Not Found");

}

httpResponce HTTPClient::run(httpRequest request)
{
    if (request.path.contains("/user/register")) {
        QString name, password;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="password") password = values.at(i).split("=")[1];
        }
        QString registerResult = Users::instance().registerUser(name, password);
        qDebug() << "Register: " << name << password << registerResult;
        httpResponce responce;
        QJsonObject obj;
        if (registerResult!="taken") {
            obj.insert("result", true);
            obj.insert("accessToken", registerResult);
        } else {
            obj.insert("result", false);
            obj.insert("error", "Username already exists");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/user/login")) {
        QString name, password;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="password") password = values.at(i).split("=")[1];
        }
        QString registerResult = Users::instance().login(name, password);
        qDebug() << "Login: " << name << password << registerResult;
        httpResponce responce;
        QJsonObject obj;
        if (registerResult!="incorrect") {
            obj.insert("result", true);
            obj.insert("accessToken", registerResult);
        } else {
            obj.insert("result", false);
            obj.insert("error", "Username or password is incorrect");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/user/checkToken")) {
        QString name, accessToken;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
        }
        bool result = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "CheckToken: " << name << accessToken << result;
        httpResponce responce;
        QJsonObject obj;
        if (result) {
            obj.insert("result", true);
        } else {
            obj.insert("result", false);
            obj.insert("result", "Access token is incorrect");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/lobby/getRooms")) {
        QString name, accessToken;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "GetRooms: " << name << accessToken << access;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            obj.insert("result", true);
            obj.insert("rooms",Hall::instance().getRooms());
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/lobby/createRoom")) {
        QString name, accessToken, roomName, roomPassword = "";
        int maxPlayers = 8;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
            else if (key=="roomPassword") roomPassword = values.at(i).split("=")[1];
            else if (key=="maxPlayers") maxPlayers = values.at(i).split("=")[1].toInt();
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "CreateRoom: " << name << accessToken << access << roomName << roomPassword << maxPlayers;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            bool result = Hall::instance().createRoom(roomName, roomPassword, maxPlayers);
            qDebug() << "CreateRoom: " << name << result;
            obj.insert("result", result);
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/lobby/enterRoom")) {
        QString name, accessToken, roomName, roomPassword = "";
        int money = 0;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
            else if (key=="roomPassword") roomPassword = values.at(i).split("=")[1];
            else if (key=="money") money = values.at(i).split("=")[1].toInt();
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "EnterRoom: " << name << accessToken << access << roomName << roomPassword << money;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            if (money<=Users::instance().getUserMoney(name)) {
                bool result = Hall::instance().enterRoom(name, money, roomName, roomPassword);
                qDebug() << "EnterRoom: " << name << result;
                obj.insert("result", result);
            }
            else {
                qDebug() << "EnterRoom: " << name << "Not enough money";
                obj.insert("result", false);
                obj.insert("error", "Not enough money");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/status")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "GameStatus: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    obj.insert("result", true);
                    obj.insert("info", room->getInfo(name));
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/getMinimumBet")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "GetMinimumBet: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    obj.insert("result", true);
                    obj.insert("minimumBet",Hall::instance().getRoom(roomName)->getMinimumBet(name));
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/start")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "StartGame: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->gameStart()) {
                    obj.insert("result", true);
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "Game already runs");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/lobby/leaveRoom")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "LeaveRoom: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            bool result = Hall::instance().leaveRoom(name, roomName);
            if (result) {
                    obj.insert("result", true);
            } else {
                obj.insert("result", false);
                obj.insert("error", "Can't leave room?");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/bet")) {
        QString name, accessToken, roomName;
        int money = 0;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
            else if (key=="money") money = values.at(i).split("=")[1].toInt();
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "Bet: " << name << accessToken << access << roomName << money;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    if (room->bet(name, money)) {
                        obj.insert("result", true);
                    } else {
                        obj.insert("result", false);
                        obj.insert("error", "Bet less then minimal or not enough money");
                    }
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/fold")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "Fold: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    if (room->fold(name)) {
                        obj.insert("result", true);
                    } else {
                        obj.insert("result", false);
                        obj.insert("error", "Can't fold?");
                    }
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/call")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "Call: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    if (room->call(name)) {
                        obj.insert("result", true);
                    } else {
                        obj.insert("result", false);
                        obj.insert("error", "Bet less then minimal or not enough money");
                    }
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    if (request.path.contains("/game/lastWinners")) {
        QString name, accessToken, roomName;
        auto values = request.value.split("&");
        for (auto i=0; i<values.count(); i++) {
            auto key = values.at(i).split("=")[0];
            if (key=="username") name = values.at(i).split("=")[1];
            else if (key=="accessToken") accessToken = values.at(i).split("=")[1];
            else if (key=="roomName") roomName = values.at(i).split("=")[1];
        }
        bool access = Users::instance().checkAccessToken(name, accessToken);
        qDebug() << "LastWinners: " << name << accessToken << access << roomName;
        httpResponce responce;
        QJsonObject obj;
        if (access) {
            Room* room = Hall::instance().getRoom(roomName);
            if (room != nullptr) {
                if (room->checkAccess(name)) {
                    if (room->lastWinners.count()>0) {
                        obj.insert("result", true);
                        QJsonArray winners;
                        for (auto winner: room->lastWinners) {
                            winners.append(winner);
                        }
                        obj.insert("winners", winners);
                    } else {
                        obj.insert("result", false);
                        obj.insert("error", "Bet less then minimal or not enough money");
                    }
                } else {
                    obj.insert("result", false);
                    obj.insert("error", "You haven't access to this room");
                }
            } else {
                obj.insert("result", false);
                obj.insert("error", "Room is not exist");
            }
        } else {
            obj.insert("result", false);
            obj.insert("error", "Access denied");
        }
        responce.body = QJsonDocument(obj).toJson();
        responce.statusCode = 200;
        responce.headers.insert("contentType", "application/json");
        return responce;
    }
    return httpResponce();
}



HTTPClient::~HTTPClient()
{
    socket->disconnectFromHost();
    socket->~QTcpSocket();
}

void HTTPClient::init(qintptr socketDescriptor)
{
    this->socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this->socket, SIGNAL(disconnected()), this, SLOT(disconnect()));
}

httpRequest HTTPClient::parseRequest(QString request)
{
    httpRequest headers;

    try {
        QStringList parsedText = request.split("\r\n");
        QStringList generalHeaders = parsedText.at(0).split(" ");
        headers.method = generalHeaders.at(0);
        QString url = generalHeaders.at(1);
        if (url.contains("?")) {
            QStringList parsedUrl = url.split("?");
            headers.path = parsedUrl[0];
            headers.value = parsedUrl[1];
        } else headers.path = url;
        for (QString header: parsedText) {
            if (header.contains("Expect:")) headers.expect100Continue = true;
        }
    }
    catch(int a) {
    }
    return headers;
}

QByteArray HTTPClient::genResponce(QMap<QString, QString> headers)
{
    QByteArray responce;
    if (!headers.contains("version")) headers.insert("version", "HTTP/1.1");
    responce.append(headers.value("version"));
    if (!headers.contains("statusCode")) headers.insert("statusCode", "200 OK");
    responce.append(" ");
    responce.append(headers.value("statusCode"));
    responce.append("\r\n");
    if (headers.contains("contentLength")) {
        responce.append(QString("Content-Length: %1\r\n").arg(headers.value("contentLength")));
    }
    if (headers.contains("contentType")) {
        responce.append(QString("Content-Type: %1\r\n").arg(headers.value("contentType")));
    }
    responce.append("\r\n");
    return responce;
}

QByteArray HTTPClient::genResponce(QMap<QString, QString> headers, QByteArray body)
{
    headers.insert("contentLength", QString::number(body.size()));
    QByteArray responce = genResponce(headers);
    responce.append(body);
    responce.append("\r\n");
    return responce;
}

void HTTPClient::worker(QString text)
{
    httpRequest request = parseRequest(text);
    if (request.expect100Continue) {
        try {
            httpResponce e100CRequest;
            e100CRequest.headers.insert("statusCode", statusCodes.value(100));
            QByteArray e100CResponce = genResponce(e100CRequest.headers);

            socket->write(e100CResponce);
            socket->waitForReadyRead();
            request.body = socket->readAll();
        }
        catch(int a) {
        }
    }
    try {
        httpResponce responce = run(request);
        QByteArray socketResponce;

        if (!responce.headers.contains("statusCode")) responce.headers.insert("statusCode", statusCodes.value(responce.statusCode));
        if (responce.body.size()>0) socketResponce = genResponce(responce.headers, responce.body);
        else socketResponce = genResponce(responce.headers);
        socket->write(socketResponce);
    }
    catch(int e) {
        qDebug() << "request parse error:" << e;
        socket->write("error");
    }
    socket->disconnectFromHost();
}

void HTTPClient::readyRead()
{
    try {
        worker(QString(socket->readAll()));
    } catch (int e) {
        this->disconnect();
    }

}

void HTTPClient::disconnect()
{
    QTimer::singleShot(1000, this, SLOT(sockDisconnected()));
}

void HTTPClient::sockDisconnected()
{
    this->~HTTPClient();
}
