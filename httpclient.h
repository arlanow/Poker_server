#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QtConcurrent/QtConcurrentRun>
#include <QJsonDocument>
#include <QJsonObject>
#include "users.h"
#include "hall.h"
#include <QTimer>
#include <QUuid>
struct httpResponce {
    QMap<QString, QString> headers;
    QByteArray body;
    int statusCode;
};
struct httpRequest {
    QString method, path, value;
    bool expect100Continue = false;
    QByteArray body;
};

class HTTPClient : public QObject
{
    Q_OBJECT
    QTcpSocket* socket;
    void firstParse(QString text);
    qint64 id;
public:
    explicit HTTPClient(qint64 id, QObject *parent = nullptr);
    QMap<int, QString> statusCodes;
    ~HTTPClient();
    Q_INVOKABLE void init(qintptr socketDescriptor);
    httpRequest parseRequest(QString request);
    QByteArray genResponce(QMap<QString, QString> headers);
    QByteArray genResponce(QMap<QString, QString> headers, QByteArray body);
    void worker(QString text);
    httpResponce run(httpRequest request);
public slots:
    void disconnect();
    void readyRead();
    void sockDisconnected();
};

#endif // HTTPCLIENT_H
