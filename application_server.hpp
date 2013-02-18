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
        int send(QByteArray m, int id);
        int findClientIdentifier(quint64 identifier);
        int findChannelIdentifier(quint64 identifier);

        //slots
    private slots:
        void newClient();
        void delClient(int id);
        void newChannel();
        void delChannel(int id);
        void recv(int id);
        void channelSendToClient(QString m, quint64 identifier);
        // TODO lors de la creation de chann, lier le signal a ce slot

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
