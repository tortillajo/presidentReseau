#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <QtCore/QCoreApplication>
#include <QtNetwork>
#include <QDebug>
#include <QSignalMapper>
#include "channel.hpp"
#include "client.hpp"

typedef struct s_application_client s_application_client;
struct s_application_client
{
    Client client;
    QTcpSocket *socket;
};

/*
** DOCUMENTATION : LES METHODES DE LA CLASSE
**
** sendClient(QByteArray m, int client_id)
** envoie le message m au client dont l'id est client_id
**
** sendChannel(QByteArray m, int channel_id)
** envoie le message m à chaque client du channel dont l'id est channel_id
**
** findClientId(quint64 client_identifier)
** trouver l'id du client dont l'identifiant est client_identifier
**
** findChannelId(quint64 channel_identifier)
** trouver l'id du channel dont l'identifiant est channel_identifier
**
** findChannelIdAmongClient(quint64 client_identifier)
** trouver channel contientant le client dont l'identi est client_identifiant
**
** SLOT channelSendToClient(..)
** le channel envoie un message a tous ses clients
**
** clientJoinChannel(..)
** ajouter le client au channel
**
** clientLeaveChannel(..)
** supprimer le client du channel
**
** clientRename(..) PROCESSING
** renome le client, en informe le channel, etc...
**
** processing(..)
** traitement du message recu
**
**
** DOCUMENTATION : LES METHODES DE LA CLASSE
**
** m_server
** Serveur TCP
**
** m_channels
** liste de tous les channels (objets=Channel)
**
** m_clients
** liste des clients (structure=s_application_client)
*/

class Application_server : public QObject
{
    Q_OBJECT

    public:
        Application_server();
        int sendClient(QByteArray m, int client_id);
        int sendChannel(QByteArray m, int channel_id);
        int findClientId(quint64 client_identifier);
        int findChannelId(quint64 channel_identifier);
        int findChannelIdAmongClient(quint64 client_identifier);

        int newChannel();
        int delChannel(int channel_id);

    public slots:
        void newClient();
        void delClient(int client_id);
        // TODO : Doit envoyer des messages !
        void recv(int client_id);
        void channelSendToClient(QString m, quint64 channel_identifier);
        // TODO lors de la creation de chann, lier le signal a ce slot

    private:
        int clientJoinChannel(quint64 client_identifier, quint64 channel_identifier);
        int clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier);
        int clientRename(int client_id, QString pseudo, quint64 channel_identifier);
        int processing(QByteArray m, int client_id); // on traite les messages recus.

        //methodes privees
    private:
        QTcpServer* m_server;
        QList< Channel*> m_channels;
        QList< s_application_client> m_clients;
};

#endif // APPLICATION_HPP
