#include "application_server.hpp"

Application_server::Application_server()
{
}

/*
** Envoyer m au client [id]
*/
int Application_server::sendClient(QByteArray m, int id_client)
{
    QByteArray paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);

    if (id_client >= 0)
    {
            mess_stream << (quint16)m.size();
            mess_stream << m;
            m_clients[id_client].socket->write(paquet);
            return (0);
    }
    else if (id_client < 0)
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
        std::cout << "ERROR : UNABLE TO DO THAT !\n";
        return (-1);
    }
}

/*
** envoyer un message m a chaque client inclue dans channellist[id]
*/
int Application_server::sendChannel(QByteArray m, int id_channel)
{
    QByteArray paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);

    if (id_channel >= 0)
    {
        int i;
        i = 0;

        mess_stream << (quint16)m.size();
        mess_stream << m;
        QList< quint64> identifier_id = m_channels[id_channel]->listClientIdentifier();

        while (i < identifier_id.size())
        {
            m_clients[findClientId(identifier_id[i])].socket->write(paquet);
            i++;
        }
        return (0);
    }
    else
    {
        std::cout << "ERROR : UNABLE TO DO THAT !\n";
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
** si pas de channel , renvoie -1
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
    return (-1);
}

/*
** ajouter un client au serveur
*/
int Application_server::newClient()
{

    QTcpSocket* socket;
    s_application_client client;
    quint64 client_identifier;
    int id;

    m_clients.append(client);
    client_identifier = client.client.identifier();
    id = findClientId(client_identifier);
    m_clients[id].socket = m_server->nextPendingConnection();
    std::cout << "New connexion" << std::endl;

    std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
    connect(m_clients[id].socket, SIGNAL(disconnected()), this, SLOT(delClient(id)));
    connect(m_clients[id].socket, SIGNAL(readyRead()), this, SLOT(recv(id)));
    return (0);
}

/*
** Enleve le client de la liste
** et en informe les autres clients du channel
*/
int Application_server::delClient(int id_client)
{
    if (id_client < 0)
        return (-1);

    quint64 channel_identifier;

    channel_identifier = m_channels[findChannelIdAmongClient(m_clients[id_client].client.identifier())]->identifier();
    if (channel_identifier != -1)
    {
        sendChannel(QString("scq" + QString::number(channel_identifier)).toUtf8(), findChannelId(channel_identifier));
    }
    m_clients.removeAt(id_client);

    return (0);
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
** TODO : Kicker les clients du channel
*/
int Application_server::delChannel(int id_channel)
{
    if (id_channel < 0)
        return (-1);

    QList< quint64> client_list;
    client_list = m_channels[id_channel]->listClientIdentifier();
    sendChannel("sCq", id_channel);

    while (!client_list.isEmpty())
    {
        m_channels[id_channel]->delClient(client_list[0]);
    }

    m_channels.removeAt(id_channel);

    return (0);
}

/*
** si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
** donc on charge les 16 bits dans la taille du message
** on attend d'avoir tout recu
*/
void Application_server::recv(int id_client)
{
    QByteArray mess;
    QDataStream mess_r(m_clients[id_client].socket);

    if (m_clients[id_client].client.dataSize() == 0)
    {
        quint16 size;

        if (m_clients[id_client].socket->bytesAvailable() < sizeof(quint16))
            return; // attente des 16 bits
        size = m_clients[id_client].client.dataSize();
        mess_r >> size; // copie des 16 bits (taille)
        m_clients[id_client].client.setDataSize(size);
    }

    if (m_clients[id_client].socket->bytesAvailable() < m_clients[id_client].client.dataSize())
        return;

    mess = m_clients[id_client].client.data();
    mess_r >> mess;
    m_clients[id_client].client.setData(mess);
    m_clients[id_client].client.setDataSize(0);
    processing(m_clients[id_client].client.data(), id_client);
    return;
}

/*
** slot permettant a un channel(identifier) d'envoyer un message a un client
** ou à l'ensemble de ses clients
*/
void Application_server::channelSendToClient(QString m, quint64 channel_identifier)
{
    if (channel_identifier == 0)
    {
        QList< quint64> identifierList;
        int i;

        identifierList = m_channels[findChannelId(channel_identifier)]->listClientIdentifier();
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
** le client tente de joindre le channel. Tester et executer
*/
int Application_server::clientJoinChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelIdAmongClient(client_identifier) > -1)
    {
        std::cout << "ERREUR : CLIENT ALREADY HAS A CHANNEL!\n";
        return (0xfff1);
    }
    else
    {
        int id_channel;

        id_channel = findChannelId(channel_identifier);
        m_channels[id_channel]->addClient(client_identifier);
        return (0);
    }
    return (0xffff);
}

/*
** le client tente de quitter le channel. Tester et executer
*/
int Application_server::clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelIdAmongClient(client_identifier) < 0)
    {
        std::cout << "ERREUR : CLIENT HAS NO CHANNEL!\n";
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
**
** TODO : Developper le protocole sortant (comment reépondre)
**
** ******************
** NORMES :
** [ data<var1:var2> ]
** s : server
** c : client
** C : channel
** p : play
** q : quit
** j : join
** g : get
** l : list
** x : parameter
** e : error (notice ! -> table README.md)
**
** ******************
** RECEPTION DE MESSAGES
** mess[0] :
**      n = request name (pseudo)
**      C = request channel
**      g = <before/after> joined channel
**      p = play
**      e = erreur
**
** n : [ n<pseudo> ]
** C : [ C<id> ]
** g : [ gl ] (liste des channels) ou
**     [ gx<info> ] (change les params)
**         <info> -> [ <c/C><params>:<value> ]
**               c -> personnel
**               C -> params du chann
**               params -> nom du params val
** p : [ p<cartes> ]
** e : [ e<num> ]
**
** ******************
** ENVOIE DE MESSAGES
**
** mess[0] :
**      s = server notification (clients, ...)
**      c = channel notification (params)
**      p = game notification (card, ...)
**
** s : [s<type><arg1:arg2:arg3:...>]
**      scq<identifier> : server client quit identifier
**      sCd             : server Channel deconnexion
 */
int Application_server::processing(QByteArray m, int client_id)
{
    int channel_id;
    int err;
    quint64 client_identifier;
    quint64 channel_identifier;

    channel_id = findChannelIdAmongClient(m_clients[client_id].client.identifier());
    client_identifier = m_clients[client_id].client.identifier();
    channel_identifier = m_channels[channel_id]->identifier();

    if (m.size() < 2)
    {
        return (1);
    }
    else if (m[0] == 'n')
    {
        QByteArray pseudo;
        pseudo = m.right(m.size() - 1);

        m_clients[id_client].client.setPseudo(pseudo);
        err = sendChannel("cc:pseudo:" + pseudo, channel_identifier);
        if (!err)
        {
            sendClient("e", id_client); // TODO...
        }
    }
    else if (m[0] == 'C')
    {
        channel_identifier = m.right(m.size() -1).toLongLong();
        err = clientJoinChannel(identifier_client, identifier_channel);
        sendClient(QString("e" + QString::number(err)).toUtf8(), id_client);
        // TODO...
    }
    else if (m[0] == 'g')
    {
        if (m[1] == 'b')
        {
            // TODO : envoyer la liste des chann
        }
        else if (m[1] == 'a')
        {
            // TODO : changer des params
        }
    }
    else if (m[0] == 'p')
    {
        m_channels[channel_id]->play(m.right(m.size() - 1), client_identifier);
    }
    else
    {
        std::cout << "Erreur dans la lecture du flux de donnees.\n";
    }

    return (0);
}
