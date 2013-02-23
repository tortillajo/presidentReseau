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

}

QList< quint64> Channel::listClientIdentifier()
{
    return (m_client_identifier);
}

quint64 Channel::identifier() const
{
    return (m_identifier);
}

QString Channel::title() const
{
    return (m_title);
}

int Channel::howManyClient() const
{
    return (m_client_identifier.size());
}

bool Channel::clientIncluded(quint64 identifier) const
{
    for (int i=0; i < m_client_identifier.size(); i++)
        {
            if (identifier == m_client_identifier[i] )
                return (true);
        }
    return (false);
}

int Channel::findClientId(quint64 identifier)
{
    int i(0);
    while ( i < m_client_identifier.size())
    {
        if (m_client_identifier[i] == identifier)
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
void Channel::clientReady(quint64 identifier, bool value)
{
    m_client_ready[findClientId(identifier)] = value;
    return;
}

bool Channel::addClient(quint64 identifier)
{
    if (!clientIncluded(identifier))
        {
            std::cout << "ERROR : CLIENT IS ALREADY PRESENT ON THIS CHANNEL.\n";
            emit sendClient("0xff11",identifier);
            return (false);
        }
    else
        {
            m_client_identifier.append(identifier);
            m_client_ready.append(false);
            emit sendClient("",identifier); // TODO : envoyer un message
            return (true);
        }
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
            std::cout << "ERROR : CLIENT IS NOT PRESENT ON THIS CHANNEL.\n";
            emit sendClient("0xff12",identifier);
            return (false);
        }
    return (false);
}

void Channel::start()
{

}
