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

    public:
        Application_server();
        int sendClient(QByteArray m, int id_client);
        int sendChannel(QByteArray m, int id_channel);
        int findClientId(quint64 client_identifier);
        int findChannelId(quint64 channel_identifier);
        int findChannelIdAmongClient(quint64 client_identifier);

    signals:
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
        ** erreurs (note : le client n'est pas forcement en faute.
        **                 les erreurs sont informatives et peuvent ne
        **                  pas poser de problèmes. )
        ** 1 : message trop court (ctb thibaut)
        ** 2 : prblm de synchro
        **
        ** 0xff11 : client déjà present dans le channel
        ** 0xff12 : client non present dans le channel
        ** 0xff13 : max client atteint dans le channel
        */
        void clientJoinChannel(quint64 client_identifier, quint64 channel_identifier);
        void clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier);
        int processing(QByteArray m, int id_client); // on traite les messages recus.

        //methodes privees
    private:
        QTcpServer* m_server;
        QList< Channel*> m_channels;
        QList< Client> m_clients;
        QList< QTcpSocket*> m_sockets;
};

#endif // APPLICATION_HPP
