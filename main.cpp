#include <QCoreApplication>
#include "httpserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qRegisterMetaType<qintptr>("qintptr");
    HTTPServer::instance();
    return a.exec();
}
