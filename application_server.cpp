#include "application_server.hpp"

Application_server::Application_server()
{
}

int Application_server::newClient()
{
    // on accepte blabla on ajoute un client
    // ensuite on lie le signal du client (recv) avec le slot Application (proccessing)

    std::cout << "New connexion" << std::endl;

    QTcpSocket* socket = m_server->nextPendingConnection();
    Client client = Client();
    m_sockets.append(socket);// << socket (lié au client);
    m_clients.append(client);// << client (lié au socket);

    int id_socket = m_sockets.indexOf(socket);
    int id_client = m_clients.indexOf(client);
    int id = -1;
    if (id_client =! id_socket)
    {
        // TODO : gestion du probleme
        std::cout << "ERREUR : DONNEES NON-SYNCHRONISEE. DISFONCTIONNEMENT GENERAL!" << std::endl;
    } else
    {
        id = id_client;
        std::cout << "AJOUT DE CLIENT DANS LES DONNES DU SERVEUR. SYNCHRONISATION OK !" << std::endl;
    }

    connect(m_sockets[id], SIGNAL(disconnected()), this, SLOT(delClient(id)));
    connect(m_sockets[id], SIGNAL(readyRead()), this, SLOT(recv(id)));
}

int Application_server::delClient(int id)
{
    // on supprime dans les deux QList le numero [id]
}

int Application_server::recv(int id)
{

}

int Application_server::processing(QString m, Client client)
{
}
