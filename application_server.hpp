#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include <iostream>
#include "channel.hpp"
#include "client.hpp"

class Application_server : public QObject
{
    Q_OBJECT

    //methodes publiques
    public:
        Application_server();
        int sendClient(QByteArray m, int id_client);
        int sendChannel(QByteArray m, int id_channel);
        int findClientId(quint64 client_identifier);
        int findChannelId(quint64 channel_identifier);
        int findChannelAmongClient(quint64 client_identifier);
        //int findClientAmongChannel();

        //slots
    private slots:
        void newClient();
        void delClient(int id_client);

        void newChannel();
        void delChannel(int id_channel);
        void recv(int id_client);
        void channelSendToClient(QString m, quint64 channel_identifier);
        // TODO lors de la creation de chann, lier le signal a ce slot

    private:
        /*
        ** erreurs :
        ** 1 : message trop court
        ** 2 : prblm de synchro
        **
        ** 0xff11 :
        ** 0xff12 :
        */
        int processing(QByteArray m, int id_client); // on traite les messages recus.

        //methodes privees
    private:
        QTcpServer* m_server;
        QList< Channel*> m_channels;
        QList< Client> m_clients;
        QList< QTcpSocket*> m_sockets;
};

#endif // APPLICATION_HPP
