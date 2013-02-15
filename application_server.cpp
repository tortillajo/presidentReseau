#include "application_server.hpp"

Application_server::Application_server()
{
}

int Application_server::send(QByteArray m, int id)
{
    QByteArray paquet;
    QDataStream mess_stream(&paquet, QIODevice::WriteOnly);
    mess_stream << (quint16)m.size();
    mess_stream << m;
    m_sockets[id]->write(paquet);
    return (0);
}

// on accepte blabla on ajoute un client
// ensuite on lie le signal du client (recv) avec le slot Application (proccessing)
// TODO : gestion du probleme
int Application_server::newClient()
{

    std::cout << "New connexion" << std::endl;
    QTcpSocket* socket = m_server->nextPendingConnection();
    Client client;

    m_sockets.append(socket);
    m_clients.append(client);

    int id_socket = m_sockets.indexOf(socket);
    int id_client = m_clients.indexOf(client);
    int id = -1;

    if (id_client == id_socket)
        {
            std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
            id = id_client;
        }
    else
        {
            std::cout << "ERREUR : DONNEES NON-SYNCHRONISEE. DISFONCTIONNEMENT GENERAL!" << std::endl;
            return(-1);
        }

    connect(m_sockets[id], SIGNAL(disconnected()), this, SLOT(delClient(id)));
    connect(m_sockets[id], SIGNAL(readyRead()), this, SLOT(recv(id)));
    return (0);
}

/*
** on supprime dans les deux QList le numero [id]
*/
int Application_server::delClient(int id)
{
    if (id < 0)
        return (-1);
    m_sockets.removeAt(id);
    m_clients.removeAt(id);
    return (0);
}

//si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
//donc on charge les 16 bits dans la taille du message
// on attend d'avoir tout recu
int Application_server::recv(int id)
{
    QDataStream mess_r(m_sockets[id]);
    if (m_clients[id].dataSize() == 0)
        {
            if (m_sockets[id]->bytesAvailable() < sizeof(quint16))
                return (1); // attente des 16 bits
            quint16 size = m_clients[id].dataSize();
            mess_r >> size; // copie des 16 bits (taille)
            m_clients[id].setDataSize(size);
        }

    if (m_sockets[id]->bytesAvailable() < m_clients[id].dataSize())
        return (2);

    QByteArray mess = m_clients[id].data();
    mess_r >> mess;
    m_clients[id].setData(mess);
    m_clients[id].setDataSize(0);
    processing(m_clients[id].data(), id);
    return 0;
}

/*
** mess[0] :
**      p = request pseudo
**      c = request channel
**      g = <before/after> have joined channel
**      p = play
**
** p : [ p<pseudo> ]
** c : [ c<id> ]
** g : [ gb ] (liste des channels) ou [ ga<info> ] (change les params)
** <info> -> [ <p/g><params>:<value> ] p-> personnel g->params du chann
**                                    params -> nom du params val
** p : [ <cartes> ]
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
