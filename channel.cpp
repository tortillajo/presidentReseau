#include "channel.hpp"

/*
** m_id est un nombre sur 64 bits
** qrand() genere un nombre sur 32 bits.
** on genere un nombre sur 32 bits -> bits faibles.
** On roll << gauche pour avoir les 32 bits forts occupes
** on ajoute un deuxieme nombre sur les 32 bits faibles
**
** m_params contient les valeurs :
** [1] nclientsmax
** [3] nclientsconnected
** [3] title
 */
Channel::Channel()
{
    m_identifier    = ((qrand() << (32)) + qrand()) | 1;
    m_round         = 0;
    m_params << "nclientsmax";
    m_params << "4";
    m_params << "nclientsconnected";
    m_params << "0";
    m_params << "title";
    m_params << "Unknow";
    // TODO : Ajouter des params
    connect(this, SIGNAL(readyToBegin()), this, SLOT(play()));
}

/*
** renvoie la liste des identifiants des clients du channel
*/
QList< quint64> Channel::listClientIdentifier()
{
    QList< quint64> list_client_identifier;

    for (int i = 0; i < m_clients.size(); i++)
    {
        list_client_identifier.append(m_clients[i].identifier);
    }
    return (list_client_identifier);
}

/*
** identifiant du channel
*/
quint64 Channel::identifier() const
{
    return (m_identifier);
}

/*
** combien le channel possede-t'il de client ?
*/
int Channel::howManyClient() const
{
    return (m_clients.size());
}

/*
** le client est-il déjà présent dans le chann ?
*/
bool Channel::clientIncluded(quint64 client_identifier) const
{
    int i;
    i = 0;

    while (i < m_clients.size())
    {
        if (client_identifier == m_clients[i].identifier )
            return (true);
        i++;
    }
    return (false);
}

/*
** trouver l'id(dans le channel) du client a partir de son identifiant
*/
int Channel::findClientId(quint64 client_identifier)
{
    int i;
    i = 0;

    while ( i < m_clients.size())
    {
        if (m_clients[i].identifier == client_identifier)
        {
            return (i);
        }
        else
        {
            i++;
            return (-1);
        }
        i++;
    }
    return (-1);
}

/*
** dit si tous les clients sont prets ou non.
*/
bool Channel::clientsAreReady()
{
    int i;
    i = 0;

    while (i < m_clients.size())
    {
        if (m_clients[i].ready == false)
            return (false);
        i++;
    }
    return (true);
}

/*
** ajouter un client s'il n'est pas déjà là.
*/
int Channel::addClient(quint64 client_identifier)
{
    if (!clientIncluded(client_identifier))
    {
        qDebug()<< "ERROR : CLIENT IS ALREADY PRESENT ON THIS CHANNEL!";
        return (0xff11);
    }
    else
    {
        if (m_params[1].toInt() > m_params[3].toInt())
        {
            if (m_started == false)
            {
                s_channel_client client;

                client.identifier   = client_identifier;
                client.ready        = false;
                m_clients.append(client);
                m_params[1]         = QString(m_params[1].toInt() + 1);
                return (0);
            }
            else
            {
                qDebug() << "ERROR : GAME ALREADY STARTED!";
            }
        }
        else
        {
            qDebug() << "ERROR : MAX NUMBER OF CLIENT CHANNEL HAS ALREADY BEEN"
                     << "REACHED.!";
            return (0xff13);
        }
    }
    return (0xffff);
}

/*
** Supprimer le client uniquement s'il est présent
*/
int Channel::delClient(quint64 client_identifier)
{
    if (clientIncluded(client_identifier))
    {
        int id;
        id          = findClientId(client_identifier);
        m_params[1] = QString(m_params[1].toInt() - 1);
        m_clients.removeAt(id);
        return (0);
    }
    else
    {
        qDebug()<< "ERROR : CLIENT IS NOT PRESENT ON THIS CHANNEL!";
        return (0xff12);
    }
    return (0xffff);
}

/*
** Le client devient ou non pret
*/
void Channel::clientReady(quint64 client_identifier, bool value)
{
    m_clients[findClientId(client_identifier)].ready = value;
    if (clientsAreReady())
    {
        emit readyToBegin();
    }
    return;
}

/*
** lancement de la partie
*/
void Channel::start()
{
}
/*
** Gestion de la partie
*/
int Channel::play(QString card, quint64 client_identifier)
{
    return (0);
}
