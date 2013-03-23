#include <QtCore/QCoreApplication>
#include <iostream>
#include "application_server.hpp"

int main(int argc, char *argv[])
{
    qDebug() << "Début du programme, " << QTime::currentTime().toString("HH:mm::ss");
    QCoreApplication app(argc, argv);
    Application_server server;
    return (app.exec());
}
