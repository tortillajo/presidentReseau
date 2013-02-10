#include "client.hpp"

Client::Client()
{
    /*
     *m_id est un nombre sur 64 bits
     *qrand() genere un nombre sur 32 bits.
     *on genere un nombre sur 32 bits -> bits faibles.
     *On roll << gauche pour avoir les 32 bits forts occupes
     *on ajoute un deuxieme nombre sur les 32 bits faibles
     */
    m_id = (qrand() << (32)) + qrand() ; //rand
}

Client::~Client()
{
}

quint64 Client::id() const
{
    return m_id;
}

QString Client::pseudo() const
{
    return m_pseudo;
}

int Client::setPseudo(QString pseudo)
{
    m_pseudo = pseudo;
    return true;
}

bool Client::operator==(Client const& client) // utile ?
{
    if (m_id == client.id())
        return true;
    else
        return false;
}
