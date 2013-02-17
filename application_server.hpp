#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include <iostream>
#include "channel.hpp"
#include "client.hpp"

class Application_server : QObject
{
    Q_OBJECT

    //methodes publiques
    public:
        Application_server();
        int send(QByteArray m, int id); // envoyer un message m a id

        //slots
    private slots:
        int newClient();
        int delClient(int id);
        int newChannel();
        int delChannel(int id);
        int recv(int id); // recv un message  (traitement et attente du message

    private:
        /*
        ** erreurs :
        ** 1 : message trop court
        ** 2 : prblm de synchro
        */
        int processing(QByteArray m, int id); // on traite les messages recus.

        //methodes privees
    private:
        QTcpServer* m_server;
        QList< Channel*> m_channels;
        QList< Client> m_clients;
        QList< QTcpSocket*> m_sockets;
};

#endif // APPLICATION_HPP
