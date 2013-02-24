#include "channel.hpp"

/*
** m_id est un nombre sur 64 bits
** qrand() genere un nombre sur 32 bits.
** on genere un nombre sur 32 bits -> bits faibles.
** On roll << gauche pour avoir les 32 bits forts occupes
** on ajoute un deuxieme nombre sur les 32 bits faibles
 */
Channel::Channel()
{
    m_identifier = (qrand() << (32)) + qrand() ;
    connect(this, SIGNAL(readyToBegin()), this, SLOT(start()));
    m_params << "nclients";
    m_params << "4";
    // TODO : Ajouter des params
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
    for (int i=0; i < m_client_identifier.size(); i++)
        {
            if (client_identifier == m_client_identifier[i] )
                return (true);
        }
    return (false);
}

/*
** trouver l'id(dans le channel) du client a partir de son identifiant
*/
int Channel::findClientId(quint64 client_identifier)
{
    int i(0);
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
    }
}

/*
** dit si tous les clients sont prets ou non.
*/
bool Channel::clientAreReady()
{
    for (int i=0; i < m_client_ready.size(); i++)
    {
        if (m_client_ready[i] == false)
            return (false);
    }
    return (true);
}

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
            m_client_identifier.append(client_identifier);
            m_client_ready.append(false);
            emit sendClient("",client_identifier); // TODO : envoyer un message
            return (true);
        }
}

/*
** Supprimer le client uniquement s'il est présent, sinon erreur et envoyer le message eau client.
*/
bool Channel::delClient(quint64 client_identifier)
{
    if (clientIncluded(client_identifier))
        {
            int id = m_client_identifier.indexOf(client_identifier);
            m_client_identifier.removeAt(id);
            m_client_ready.removeAt(id);
            return (true);
        }
    else
        {
            std::cout << "ERROR : CLIENT IS NOT PRESENT ON THIS CHANNEL.\n";
            emit sendClient("0xff12",client_identifier);
            return (false);
        }
    return (false);
}

/*
** lancement de la partie. Gestion des coups etc..
*/
void Channel::start()
{

}
