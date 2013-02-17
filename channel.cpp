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
}
bool Channel::clientIncluded(quint64 identifier)
{
    for (int i=0; i < m_client_identifier.size(); i++)
        {
            if (identifier == m_client_identifier[i] )
                return (true);
        }
    return (false);
}

void Channel::clientReady(quint64 identifier, bool value)
{
    m_client_ready[identifier] = value;
    return;
}

bool Channel::addClient(quint64 identifier)
{
    if (!clientIncluded(identifier))
        {
            std::cout << "ERROR : CLIENT ALREADY PRESENT ON THIS CHANNEL.\n";
            emit clientAlreadyHere();
            return (false);
        }
    else
        {
            m_client_identifier.append(identifier);
            m_client_ready.append(false);
            return (true);
        }
    return (false);
}

bool Channel::delClient(quint64 identifier)
{
    if (!clientIncluded(identifier))
        {
            int id = m_client_identifier.indexOf(identifier);
            m_client_identifier.removeAt(id);
            m_client_ready.removeAt(id);
            return (true);
        }
    else
        {
            std::cout << "ERROR : CLIENT NOT PRESENT ON THIS CHANNEL.\n";
            emit clientNotHere();
            return (false);
        }
    return (false);
}

void Channel::start()
{

}
