#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include <iostream>
#include "channel.hpp"
#include "client.hpp"

typedef struct s_application_client s_application_client;
struct s_application_client
{
    Client client;
    QTcpSocket *socket;
};

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
        int clientJoinChannel(quint64 client_identifier, quint64 channel_identifier);
        int clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier);
        int processing(QByteArray m, int id_client); // on traite les messages recus.

        //methodes privees
    private:
        QTcpServer* m_server;
        QList< Channel*> m_channels;
        QList< s_application_client> m_clients;
};

#endif // APPLICATION_HPP
