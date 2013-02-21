#include "application_server.hpp"

// CONSTRUCTEURS / DESTRUCT
Application_server::Application_server()
{
}

// PUBLIC
int Application_server::findClientIdentifier(quint64 identifier)
{
    int i(0);
    while ( i < m_clients.size())
    {
        if (m_clients[i].identifier() == identifier)
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

int Application_server::findChannelIdentifier(quint64 identifier)
{
    int i(0);
    while ( i < m_channels.size())
    {
        if (m_channels[i]->identifier() == identifier)
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

// SLOTS

int Application_server::send(QByteArray m, int id)
{
    if (id > 0)
        {
            QByteArray paquet;
            QDataStream mess_stream(&paquet, QIODevice::WriteOnly);
            mess_stream << (quint16)m.size();
            mess_stream << m;
            m_sockets[id]->write(paquet);
            return(0);
        }
    else if (id == 0)
        {
            int i(0);
            while (i < m_clients.size())
            {
                send(m,i);
                i++;
            }
            return (0);
        }
    else
        {
            std::cout << "ERROR : ID TO SEND < 0 !!!! UNABLE TO DO THAT !\n";
            return(-1);
        }
}

void Application_server::newClient()
{

    std::cout << "New connexion" << std::endl;
    QTcpSocket* socket = m_server->nextPendingConnection();
    Client client;

    m_sockets.append(socket);
    m_clients.append(client);

    int id_socket = m_sockets.indexOf(socket);
    int id_client = m_clients.indexOf(client);
    int id = (-1);

    if (id_client == id_socket)
        {
            std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
            id = id_client;
        }
    else
        {
            std::cout << "ERREUR : DONNEES NON-SYNCHRONISEE. DISFONCTIONNEMENT GENERAL!" << std::endl;
            return;
        }

    connect(m_sockets[id], SIGNAL(disconnected()), this, SLOT(delClient(id)));
    connect(m_sockets[id], SIGNAL(readyRead()), this, SLOT(recv(id)));
    return;
}


void Application_server::delClient(int id)
{
    if (id < 0)
        return;
    m_sockets.removeAt(id);
    m_clients.removeAt(id);
    return;
}

void Application_server::newChannel()
{
    Channel *channel;
    m_channels.append(channel);
    connect(m_channels.last(), SIGNAL(channelFilled(QString,quint64)), this, SLOT(channelSendToClient(QString,quint64)));
    return;
}

void Application_server::delChannel(int id)
{
    if (id < 0)
        return;
    m_channels.removeAt(id);
    return;
}

/*
**si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
** donc on charge les 16 bits dans la taille du message
** on attend d'avoir tout recu
*/
void Application_server::recv(int id)
{
    QDataStream mess_r(m_sockets[id]);
    if (m_clients[id].dataSize() == 0)
        {
            if (m_sockets[id]->bytesAvailable() < sizeof(quint16))
                return; // attente des 16 bits
            quint16 size = m_clients[id].dataSize();
            mess_r >> size; // copie des 16 bits (taille)
            m_clients[id].setDataSize(size);
        }

    if (m_sockets[id]->bytesAvailable() < m_clients[id].dataSize())
        return;

    QByteArray mess = m_clients[id].data();
    mess_r >> mess;
    m_clients[id].setData(mess);
    m_clients[id].setDataSize(0);
    processing(m_clients[id].data(), id);
    return;
}

void Application_server::channelSendToClient(QString m, quint64 identifier)
{
    int i(0);
    if (identifier == 0)
        {
            while (i < m_clients.size())
            {
                this->send(m.toAscii(), i);
                i++;
            }
        }
    else
        {
            this->send(m.toAscii(), findClientIdentifier(identifier));
        }
    return;
}

// PRIVATE

/*
** mess[0] :
**      p = request pseudo
**      c = request channel
**      g = <before/after> have joined channel
**      p = play
**      e = erreur
**
** p : [ p<pseudo> ]
** c : [ c<id> ]
** g : [ gb ] (liste des channels) ou [ ga<info> ] (change les params)
** <info> -> [ <p/g><params>:<value> ] p-> personnel g->params du chann
**                                    params -> nom du params val
** p : [ p<cartes> ]
** e : [ e<num> ]
 */
int Application_server::processing(QByteArray m, int id)
{
    if (m.size() < 2) {
        return 1;
    } else if (m[0] == 'p') {
        m_clients[id].setPseudo( m.right(m.size() - 1));
        // envoyer une reponse ?
    } else if (m[0] == 'c') {
        //tenter de joindre le channel m.right(m.size() -1)
    } else if (m[0] == 'g') {
        if (m[1] == 'b') {
            // envoyer la liste des chann
        } else if (m[1] == 'a') {
            // changer des params
        }
    } else if (m[0] == 'p') {
        /*
        ** TODO :
        ** gestion des cartes.
        ** verifier si la carte est dans la main du joueur
        ** verifier s'il a le droit le la poser (règles + tour )
        ** poser
        ** envoyer aux autres joueurs l'information
        ** tour suivant
         */
    } else {
        std::cout << "Erreur dans la lecture du flux de donnees.\n";
    }

    return (0);
}
