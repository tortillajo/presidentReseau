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
 */
Channel::Channel()
{
    m_identifier = (qrand() << (32)) + qrand() ;
    m_params << "nclientsmax";
    m_params << "4";
    m_params << "nclientsconnected";
    m_params << "0";
    // TODO : Ajouter des params
    connect(this, SIGNAL(readyToBegin()), this, SLOT(start()));
}

/*
** renvoie la liste des identifiants des clients du channel
*/
QList< quint64> Channel::listClientIdentifier()
{
    return (m_client_identifier);
}

/*
** identifiant du channel
*/
quint64 Channel::identifier() const
{
    return (m_identifier);
}

/*
** titre
*/
QString Channel::title() const
{
    return (m_title);
}

/*
** combien le channel possede-t'il de client ?
*/
int Channel::howManyClient() const
{
    return (m_client_identifier.size());
}

/*
** le client est-il déjà présent dans le chann ?
*/
bool Channel::clientIncluded(quint64 client_identifier) const
{
    int i;

    i = 0;

    while (i < m_client_identifier.size())
    {
        if (client_identifier == m_client_identifier[i] )
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

    while ( i < m_client_identifier.size())
    {
        if (m_client_identifier[i] == client_identifier)
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
bool Channel::clientAreReady()
{
    int i;

    i = 0;

    while (i < m_client_ready.size())
    {
        if (m_client_ready[i] == false)
            return (false);
        i++;
    }
    return (true);
}

/*
** ajouter un client s'il n'est pas déjà là. Sinon envoyer une erreur au client et ne pas ajouter.
*/
bool Channel::addClient(quint64 client_identifier)
{
    if (!clientIncluded(client_identifier))
    {
        std::cout << "ERROR : CLIENT IS ALREADY PRESENT ON THIS CHANNEL.\n";
        emit sendClient("0xff11",client_identifier);
        return (false);
    }
    else
    {
        if (m_params[1].toInt() > m_params[3].toInt())
        {
            m_client_identifier.append(client_identifier);
            m_client_ready.append(false);
            emit sendClient("",client_identifier); // TODO : envoyer un message
            m_params[1] = QString(m_params[1].toInt() + 1);
            return (true);
        }
        else
        {
            std::cout << "ERROR : MAX NUMBER OF CLIENT CHANNEL HAS ALREADY BEEN"
                      << "REACHED.\n";
            emit sendClient("0xff13",client_identifier);
            return (false);
        }
    }
}

/*
** Supprimer le client uniquement s'il est présent, sinon erreur et envoyer
**le message eau client.
*/
bool Channel::delClient(quint64 client_identifier)
{
    if (clientIncluded(client_identifier))
    {
        int id;

        id = m_client_identifier.indexOf(client_identifier);
        m_client_identifier.removeAt(id);
        m_client_ready.removeAt(id);
        emit sendClient("",client_identifier); // TODO : envoyer un message
        m_params[1] = QString(m_params[1].toInt() - 1);
        return (true);
    }
    else
    {
        std::cout << "ERROR : CLIENT IS NOT PRESENT ON THIS CHANNEL.\n";
        return (false);
    }
    return (false);
}

/*
** Le client devient ou non pr^et
*/
void Channel::clientReady(quint64 client_identifier, bool value)
{
    m_client_ready[findClientId(client_identifier)] = value;

    if (clientAreReady())
    {
        emit readyToBegin();
    }
    return;
}

/*
** lancement de la partie. Gestion des coups etc..
*/
void Channel::start()
{

}
