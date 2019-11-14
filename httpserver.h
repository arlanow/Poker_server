#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include "httpclient.h"
#include <QTcpSocket>
#include <QDebug>

class HTTPServer : public QTcpServer
{
    Q_OBJECT
    explicit HTTPServer(QObject *parent = nullptr);
    ~HTTPServer();

public:
    static HTTPServer &instance();
signals:
protected:
    virtual void incomingConnection(qintptr socketDescriptor);
};

#endif // HTTPSERVER_H
