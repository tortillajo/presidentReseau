#include "client.hpp"

/*
** m_id est un nombre sur 64 bits
** qrand() genere un nombre sur 32 bits.
** on genere un nombre sur 32 bits -> bits faibles.
** On roll << gauche pour avoir les 32 bits forts occupes
** on ajoute un deuxieme nombre sur les 32 bits faibles
** lui demander son pseudo :)
 */
Client::Client()
{
    m_identifier = ((qrand() << (32)) + qrand()) | 1;
}

Client::~Client()
{
}

quint64 Client::identifier() const
{
    return (m_identifier);
}

QString Client::pseudo() const
{
    return (m_pseudo);
}

quint16 Client::dataSize() const
{
    return (m_dataSize);
}

QByteArray Client::data() const
{
    return (m_data);
}

void Client::setPseudo(QString pseudo)
{
    m_pseudo = pseudo;
    return;
}

void Client::setDataSize(quint16 size)
{
    m_dataSize = size;
    return;
}

void Client::setData(QByteArray d)
{
    m_data = d;
    return;
}

bool Client::operator==(Client const& client) // utile ?
{
    if (m_identifier == client.identifier())
        return (true);
    else
        return (false);
}
