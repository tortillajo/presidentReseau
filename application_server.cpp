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
    return 0;
}

int Application_server::newClient()
{
    // on accepte blabla on ajoute un client
    // ensuite on lie le signal du client (recv) avec le slot Application (proccessing)

    std::cout << "New connexion" << std::endl;

    QTcpSocket* socket = m_server->nextPendingConnection();
    Client client;
    m_sockets.append(socket);// << socket (lié au client);
    m_clients.append(client);// << client (lié au socket);

    int id_socket = m_sockets.indexOf(socket);
    int id_client = m_clients.indexOf(client);
    int id = -1;
    if (id_client =! id_socket)
    {
        // TODO : gestion du probleme
        std::cout << "ERREUR : DONNEES NON-SYNCHRONISEE. DISFONCTIONNEMENT GENERAL!" << std::endl;
        return (-1);
    } else
    {
        id = id_client;
        std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
    }

    connect(m_sockets[id], SIGNAL(disconnected()), this, SLOT(delClient(id)));
    connect(m_sockets[id], SIGNAL(readyRead()), this, SLOT(recv(id)));
    return 0;
}

int Application_server::delClient(int id)
{
    if (id < 0) return (-1);
    // on supprime dans les deux QList le numero [id]
    m_sockets.removeAt(id);
    m_clients.removeAt(id);
    return 0;
}

int Application_server::recv(int id)
{
    /*
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    int id = m_sockets.indexOf(client);


    if (client==0) {
        return (-404);
    }
     *  theoriquement on doit retrouver le meme id que indique.
     *  car le socket[id] a emis le signal readyRead linked au slot recv avec le params"id"
     */

    //si on a 0 donnes a recup, alors ca veut dire que c'est un nouveau paquet
    //donc on charge les 16 bits dans la taille du message
    QDataStream mess_r(m_sockets[id]);
    if (m_clients[id].dataSize() == 0) {
        if (m_sockets[id]->bytesAvailable() < sizeof(quint16)) return (1); // attente des 16 bits
        quint16 size = m_clients[id].dataSize();
        mess_r >> size; // copie des 16 bits (taille)
        m_clients[id].setDataSize(size);
    }

    // on attend d'avoir tout recu
    if (m_sockets[id]->bytesAvailable() < m_clients[id].dataSize()) return (2);

    QByteArray mess = m_clients[id].data(); // on recupere les datas stockés ? rien ?
    mess_r >> mess; // on place le stream dans les datas
    m_clients[id].setData(mess); // on sauvegarde le message
    m_clients[id].setDataSize(0); // remise a 0 du compteur
    processing(m_clients[id].data(), id); // on gere
    return 0;
}

int Application_server::processing(QByteArray m, int id)
{
    return 0;
}
