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

        int newClient();
        int delClient(int id_client);
        int newChannel();
        int delChannel(int id_channel);
    signals:
    private slots:
        // TODO : Doit envoyer des messages !
        void recv(int id_client);
        void channelSendToClient(QString m, quint64 channel_identifier);
        // TODO lors de la creation de chann, lier le signal a ce slot

    private:
        /*
        ** erreurs (note : le client n'est pas forcement en faute.
        **                 les erreurs sont informatives et peuvent ne
        **                 pas poser de problèmes. )
        **
        ** 0xff11 : [NOTIC] client déjà present dans le channel
        ** 0xff12 : [NOTIC] client non present dans le channel
        ** 0xff13 : [NOTIC] max client atteint dans le channel
        ** 0xfff1 : [CRITI] erreur de synchronisation
        ** 0xffff : [GRAVE] unknow error
        **
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
