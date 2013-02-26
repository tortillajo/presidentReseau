#include "application_server.hpp"

// CONSTRUCTEURS / DESTRUCT
Application_server::Application_server()
{
}

// PUBLIC
/*
** Envoyer m au client [id]
*/
int Application_server::sendClient(QByteArray m, int id_client)
{
    QByteArray paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);
    int i;

    i = 0;

    if (id_client >= 0)
    {
            mess_stream << (quint16)m.size();
            mess_stream << m;
            m_sockets[id_client]->write(paquet);
            return(0);
    }
    else if (id_client == -1)
    {
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
            return(-1);
    }
}

/*
** envoyer un message m a chaque client inclue dans channellist[id]
*/
int Application_server::sendChannel(QByteArray m, int id_channel)
{
    QByteArray paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);
    int i;

    i = 0;

    if (id_channel >= 0)
    {
            mess_stream << (quint16)m.size();
            mess_stream << m;
            QList< quint64> identifier_id = m_channels[id_channel]->listClientIdentifier();
            while (i < identifier_id.size())
            {
                m_sockets[ findClientId(identifier_id[i]) ]->write(paquet);
                i++;
            }
            return(0);
    }
    else
    {
            std::cout << "ERROR : UNABLE TO DO THAT !\n";
            return(-1);
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
        if (m_clients[i].identifier() == client_identifier)
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
** trouver l'id du channel auqeuel apartient le client
*/
int Application_server::findChannelAmongClient(quint64 client_identifier)
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
void Application_server::newClient()
{

    QTcpSocket* socket;
    Client client;
    int id_socket;
    int id_client;
    int id;

    id_socket = m_sockets.indexOf(socket);
    id_client = m_clients.indexOf(client);
    m_sockets.append(socket);
    m_clients.append(client);
    socket = m_server->nextPendingConnection();
    std::cout << "New connexion" << std::endl;

    if (id_client == id_socket)
    {
        std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
        id = id_client;
        connect(m_sockets[id], SIGNAL(disconnected()), this, SLOT(delClient(id)));
        connect(m_sockets[id], SIGNAL(readyRead()), this, SLOT(recv(id)));
    }
    else
    {
        std::cout << "ERREUR : DONNEES NON-SYNCHRONISEE. DISFONCTIONNEMENT GENERAL!" << std::endl;
        return;
    }
    return;
}

/*
**
*/
void Application_server::delClient(int id_client)
{
    if (id_client < 0)
        return;
    m_sockets.removeAt(id_client);
    m_clients.removeAt(id_client);
    return;
}

/*
**
*/
void Application_server::newChannel()
{
    Channel *channel;
    m_channels.append(channel);
    connect(m_channels.last(), SIGNAL(sendClient(QString,quint64)), this, SLOT(channelSendToClient(QString,quint64)));
    return;
}

/*
**
*/
void Application_server::delChannel(int id_channel)
{
    if (id_channel < 0)
        return;
    m_channels.removeAt(id_channel);
    return;
}

/*
**si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
** donc on charge les 16 bits dans la taille du message
** on attend d'avoir tout recu
*/
void Application_server::recv(int id_client)
{
    QByteArray mess;
    QDataStream mess_r(m_sockets[id_client]);

    if (m_clients[id_client].dataSize() == 0)
    {
        quint16 size;

        if (m_sockets[id_client]->bytesAvailable() < sizeof(quint16))
            return; // attente des 16 bits
        size = m_clients[id_client].dataSize();
        mess_r >> size; // copie des 16 bits (taille)
        m_clients[id_client].setDataSize(size);
    }

    if (m_sockets[id_client]->bytesAvailable() < m_clients[id_client].dataSize())
        return;

    mess = m_clients[id_client].data();
    mess_r >> mess;
    m_clients[id_client].setData(mess);
    m_clients[id_client].setDataSize(0);
    processing(m_clients[id_client].data(), id_client);
    return;
}

/*
** slot permettant a un channel(identifier) d'envoyer un message a un client
** ou à l'ensemble de ses clients
*/
void Application_server::channelSendToClient(QString m, quint64 channel_identifier)
{
    int i;

    i = 0;

    if (channel_identifier == 0)
    {
        QList< quint64> identifierList;
        identifierList = m_channels[findChannelId(channel_identifier)]->listClientIdentifier();
        while (i < identifierList.size())
        {
            this->sendClient(m.toAscii(), findClientId(identifierList[i]));
            i++;
        }
    }
    else
    {
        this->sendClient(m.toAscii(), findClientId(channel_identifier));
    }
    return;
}

/*
** le client tente de joindre le channel. Tester et executer
*/
void Application_server::clientJoinChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelAmongClient(client_identifier) > 0)
    {
        std::cout << "ERREUR : CLIENT ALREADY HAS A CHANNEL!\n";
    }
    else
    {
        int id_channel;

        id_channel = findChannelId(channel_identifier);
        m_channels[id_channel]->addClient(client_identifier);
    }
}

/*
** le client tente de quitter le channel. Tester et executer
*/
void Application_server::clientLeaveChannel(quint64 client_identifier, quint64 channel_identifier)
{
    if (findChannelAmongClient(client_identifier) == -1)
    {
        std::cout << "ERREUR : CLIENT HAS NO CHANNEL!\n";
    }
    else
    {
        int id_channel;

        id_channel = findChannelId(channel_identifier);
        m_channels[id_channel]->delClient(client_identifier);
    }
}

/*
** RECEPTION DE MESSAGES
** mess[0] :
**      p = request pseudo
**      c = request channel
**      g = <before/after> have joined channel
**      p = play
**      e = erreur
**
** i : [ i<pseudo> ]
** c : [ c<id> ]
** g : [ gb ] (liste des channels) ou
**     [ ga<info> ] (change les params)
**         <info> -> [ <p/g><params>:<value> ]
**               p-> personnel
**               g->params du chann
**               params -> nom du params val
** p : [ p<cartes> ]
** e : [ e<num> ]
**
** ENVOIE DE MESSAGES
** mess[0] :
**      c = nouveau parametre de channel
**      s = nouveau parametre de serveur
** c : [ cp<params>:<value> ]
**     [ cc<client>:<params>:<value> ]
** s : [ s<??> ]
**
 */
int Application_server::processing(QByteArray m, int id_client)
{
    int id_channel;
    quint64 identifier_client;
    quint64 identifier_channel;

    id_channel = findChannelAmongClient(m_clients[id_client].identifier());
    identifier_client = m_clients[id_client].identifier();
    identifier_channel = m_channels[id_channel]->identifier();

    if (m.size() < 2)
    {
        return (1);
    }
    else if (m[0] == 'i')
    {
        QByteArray pseudo;

        pseudo = m.right(m.size() - 1);

        m_clients[id_client].setPseudo(pseudo);
        sendChannel("cc:pseudo:" + pseudo, identifier_channel);

    }
    else if (m[0] == 'c')
    {
        clientJoinChannel(identifier_client,identifier_channel);
    }
    else if (m[0] == 'g')
    {
        if (m[1] == 'b')
        {
            // TODO : envoyer la liste des chann
        } else if (m[1] == 'a')
        {
            // TODO : changer des params
        }
    }
    else if (m[0] == 'p')
    {
        /*
        ** TODO :
        ** gestion des cartes.
        ** verifier si la carte est dans la main du joueur
        ** verifier s'il a le droit le la poser (règles + tour )
        ** poser
        ** envoyer aux autres joueurs l'information
        ** tour suivant
         */
    }
    else
    {
        std::cout << "Erreur dans la lecture du flux de donnees.\n";
    }

    return (0);
}
