#include "httpserver.h"

HTTPServer::HTTPServer(QObject *parent) :
    QTcpServer(parent)
{
    quint16 port;
    port = 5202;
    listen(QHostAddress::Any, port);
}

HTTPServer::~HTTPServer()
{
}

void HTTPServer::incomingConnection(qintptr socketDescriptor)
{
    HTTPClient* client = new HTTPClient(0);
    QMetaObject::invokeMethod(client, "init",
                Qt::QueuedConnection,
                Q_ARG(qintptr, socketDescriptor));
}

HTTPServer &HTTPServer::instance()
{
    static HTTPServer instance;
    return instance;
}

