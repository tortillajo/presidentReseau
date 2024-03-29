#include "application_server.hpp"

Application_server::Application_server()
{
    m_server = new QTcpServer;
    if (m_server->listen(QHostAddress::Any,9033))
    {
        qDebug() << "Server start successfull!";
        connect(m_server, SIGNAL(newConnection()), this, SLOT(newClient()));
    }
    else
    {
        qDebug() << m_server->errorString();
    }
}

/*
** Envoyer m au client [id]
*/
int Application_server::sendClient(QByteArray m, int client_id)
{
    QByteArray  paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);

    if (client_id >= 0)
    {
            mess_stream << (quint16)m.size();
            mess_stream << m;
            m_clients[client_id].socket->write(paquet);
            return (0);
    }
    else if (client_id < 0)
    {
        int i;
        i = 0;

        while (i < m_clients.size())
        {
            sendClient(m,i);
            i++;
        }
        return (0);
    }
    else
    {
        qDebug()<< "[NOTIC] : UNABLE TO DO THAT !";
        return (-1);
    }
}

/*
** envoyer un message m a chaque client inclue dans channellist[id]
*/
int Application_server::sendChannel(QByteArray m, int channel_id)
{
    QByteArray  paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);

    if (channel_id >= 0)
    {
        int i;
        QList< quint64> identifier_id;
        i = 0;

        mess_stream << (quint16)m.size();
        mess_stream << m;
        identifier_id = m_channels[channel_id]->listClientIdentifier();

        while (i < identifier_id.size())
        {
            m_clients[findClientId(identifier_id[i])].socket->write(paquet);
            i++;
        }
        return (0);
    }
    else
    {
        qDebug() << "[NOTIC] : UNABLE TO DO THAT !";
        return (-1);
    }
}

/*
** trouver un id de client a partir de l'identifier
*/
int Application_server::findClientId(quint64 client_identifier)
{
    int i;
    i = 0;

    while ( i < m_clients.size())
    {
        if (m_clients[i].client.identifier() == client_identifier)
        {
            return (i);
        }
        else
        {
            i++;
        }
    }
    return (-1);
}

/*
** trouver un id de channel avec sont identifier
*/
int Application_server::findChannelId(quint64 channel_identifier)
{
    int i;
    i = 0;

    while ( i < m_channels.size())
    {
        if (m_channels[i]->identifier() == channel_identifier)
        {
            return (i);
        }
        else
        {
            i++;
        }
    }
    return (-1);
}

/*
** trouver l'id du channel auquel apartient le client
** si pas de channel , renvoie 0
*/
int Application_server::findChannelIdAmongClient(quint64 client_identifier)
{
    int i;
    i = 0;

    while (i < m_channels.size())
    {
        if (m_channels[i]->clientIncluded(client_identifier))
        {
            return (i);
        }
        i++;
    }
    return (0);
}

/*
** ajouter un client au serveur
*/
void Application_server::newClient()
{
    s_application_client    client;
    quint64                 client_identifier;
    int                     id;
    QSignalMapper*          signalDelClient = new QSignalMapper (this);
    QSignalMapper*          signalRecv = new QSignalMapper (this);

    qDebug()<< ">New connexion!";
    m_clients.append(client);
    client_identifier   = client.client.identifier();
    id                  = findClientId(client_identifier);
    m_clients[id].socket = m_server->nextPendingConnection();

    connect(m_clients[id].socket, SIGNAL(disconnected()), signalDelClient, SLOT(map()));
    connect(m_clients[id].socket, SIGNAL(readyRead()), signalRecv, SLOT(map()));

    signalDelClient->setMapping(m_clients[id].socket, id);
    signalRecv->setMapping(m_clients[id].socket, id);

    connect(signalDelClient, SIGNAL(mapped(int)), this, SLOT(delClient(int)));
    connect(signalRecv, SIGNAL(mapped(int)), this, SLOT(recv(int)));

    qDebug()<< ">ADDING THE CLIENT COMPLETED !";
    return;
}

/*
** Enleve le client de la liste
** et en informe les autres clients du channel
*/
void Application_server::delClient(int client_id)
{
    if (client_id < 0)
        return;

    int     channel_id;
    quint64 channel_identifier;

    client_id           = m_clients[client_id].client.identifier();
    channel_id          = findChannelIdAmongClient(client_id);
    channel_identifier  = m_channels[channel_id]->identifier();

    if (channel_identifier != 0)
    {
        QByteArray message;
        message = QString("scq" + QString::number(channel_identifier)).toUtf8();
        sendChannel(message, findChannelId(channel_identifier));
    }
    else
    {
        qDebug() << "[NOTIC] UNABLE TO DO THAT.";
    }
    m_clients.removeAt(client_id);

    return;
}

/*
** Créer un nouveau channel
*/
int Application_server::newChannel()
{
    Channel *channel;
    m_channels.append(channel);
    return (0);
}

/*
** Enleve le channel de la liste
*/
int Application_server::delChannel(int channel_id)
{
    if (channel_id < 0)
        return (-1);

    QList< quint64> client_list;

    client_list = m_channels[channel_id]->listClientIdentifier();
    sendChannel("CHANNEL DELETED " + QString::number(m_channels[channel_id]->identifier()).toUtf8(), channel_id);

    while (!client_list.isEmpty())
    {
        m_channels[channel_id]->delClient(client_list[0]);
    }

    m_channels.removeAt(channel_id);

    return (0);
}

/*
** si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
** donc on charge les 16 bits dans la taille du message
** on attend d'avoir tout recu
*/
void Application_server::recv(int client_id)
{
    QByteArray  mess;
    QDataStream mess_r(m_clients[client_id].socket);

    if (m_clients[client_id].client.dataSize() == 0)
    {
        quint16 size;

        if (m_clients[client_id].socket->bytesAvailable() < sizeof(quint16))
            return; // attente des 16 bits
        size = m_clients[client_id].client.dataSize();
        mess_r >> size; // copie des 16 bits (taille)
        m_clients[client_id].client.setDataSize(size);
    }

    if (m_clients[client_id].socket->bytesAvailable() < m_clients[client_id].client.dataSize())
        return;

    mess = m_clients[client_id].client.data();
    mess_r >> mess;
    m_clients[client_id].client.setData(mess);
    m_clients[client_id].client.setDataSize(0);
    processing(m_clients[client_id].client.data(), client_id);
    return;
}

/*
** SLOT un channel (channel_identifier) envoie un message a un client
** ou à l'ensemble de ses clients.
*/
void Application_server::channelSendToClient(QString m, quint64 channel_identifier)
{
    if (channel_identifier == 0)
    {
        QList< quint64> identifierList;
        int             channel_id;
        int             i;

        channel_id = findChannelId(channel_identifier);
        identifierList = m_channels[channel_id]->listClientIdentifier();
        i = 0;

        while (i < identifierList.size())
        {
            this->sendClient(m.toUtf8(), findClientId(identifierList[i]));
            i++;
        }
    }
    else
    {
        this->sendClient(m.toUtf8(), findClientId(channel_identifier));
    }
    return;
}

/*
** PRIVATE PROCESSING : le client tente de joindre le channel. Tester et executer
*/
int Application_server::clientJoinChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelIdAmongClient(client_identifier) > 0)
    {
        qDebug() << "[NOTIC] : CLIENT ALREADY HAS A CHANNEL!";
        return (0xfff1);
    }
    else
    {
        int channel_id;

        channel_id = findChannelId(channel_identifier);
        m_channels[channel_id]->addClient(client_identifier);
        return (0);
    }
    return (0xffff);
}

/*
** PRIVATE PROCESSING : le client tente de quitter le channel. Tester et executer
*/
int Application_server::clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelIdAmongClient(client_identifier) < 0)
    {
        qDebug()<< "ERREUR : CLIENT HAS NO CHANNEL!";
        return (0xfff2);
    }
    else
    {
        int id_channel;

        id_channel = findChannelId(channel_identifier);
        m_channels[id_channel]->delClient(client_identifier);
        return (0);
    }
    return (0xffff);
}

/*
** PRIVATE PROCESSING : renomme le client
*/
int Application_server::clientRename(int client_id, QString pseudo, quint64 channel_identifier)
{
    QString     pseudo_old;
    QByteArray  message_send;

    pseudo_old = m_clients[client_id].client.pseudo();
    m_clients[client_id].client.setPseudo(pseudo);
    message_send = QString("CLIENT RENAME " + pseudo_old + " " + pseudo).toUtf8();
    sendChannel(message_send, channel_identifier);
    qDebug() << message_send;
}

/*
** PRIVATE PROCESSING : renvoie à la liste en QString des params
*/
QString Application_server::listAllChannel()
{
    QList< s_application_channel> list;
    s_application_channel channel;
    QString message;

    for (int i = 0; i < m_channels.size(); i++)
    {
        channel.identifier = m_channels[i]->identifier();
        channel.nclients_max = m_channels[i]->params(1).toInt();
        channel.nclients_connected = m_channels[i]->params(3).toInt();
        channel.title = m_channels[i]->params(5);
        list.append(channel);
    }

    for (int i = 0; i < list.size(); i++)
    {
        message += list[i].identifier + " ";
        message += list[i].nclients_max + " ";
        message += list[i].nclients_connected + " ";
        message += list[i].title + " ";
    }
    return message;
}

/*
** NORMES : mots clef séparés par des espaces
** server
** client
** channel
** play
** quit
** join
** get
** LIST
** parameter
** error (notice ! -> table README.md)
**
** LIST : LIST nb_channels nb_params <listAllChannel()>
 */
int Application_server::processing(QByteArray m, int client_id)
{
    int                 notice;
    int                 channel_id;
    quint64             client_identifier;
    quint64             channel_identifier;
    QByteArray          message_send;
    QList<QByteArray>   message_recv;

    client_identifier   = m_clients[client_id].client.identifier();
    channel_id          = findChannelIdAmongClient(client_identifier);
    channel_identifier  = m_channels[channel_id]->identifier();

    if (m.size() < 2)
    {
        return (1);
    }
    else
    {
        message_recv = m.split(' ');

        if (message_recv[0] == "NAME")
        {
            clientRename(client_id, message_recv[1], channel_identifier);
        }
        else if (message_recv[0] == "JOIN")
        {
            channel_identifier = message_recv[1].toLongLong();
            notice          = clientJoinChannel(client_identifier, channel_identifier);
            message_send    = QString("NOTICE " + QString::number(notice)).toUtf8();
            sendClient(message_send, client_id);
        }
        else if (message_recv[0] == "READY")
        {
            m_channels[channel_id]->clientReady(client_identifier, true);
        }
        else if (message_recv[0] == "NOTREADY")
        {
            m_channels[channel_id]->clientReady(client_identifier, false);
        }
        else if (message_recv[0] == "PLAY")
        {
            if (message_recv.size() >= 2)
            {
                QString cards;

                for (int i = 1; i < message_recv.size(); i++)
                    cards += message_recv[i] + " ";

                m_channels[channel_id]->play(cards, client_identifier);
            }
            else
            {
                message_send = QString("NOTICE " + QString::number(0xff01)).toUtf8();
            }
        }
        else if (message_recv[0] == "LIST")
        {

            message_send += "LIST " +
                            QString::number(m_channels.size()) +
                            QString::number(NB_PARAMS);
            message_send += listAllChannel();
            sendClient(message_send, client_id);
        }
        else
        {
            qDebug()<< "[NOTIC] Erreur dans la lecture du flux de données.";
        }
    }
    return (0);
}
