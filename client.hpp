#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <QDebug>
#include "channel.hpp"

/*
** DOCUMENTATION : GESTION DES CARTES
** les cartes sont représentées par un nombre de 8 bits
** octet :    utilisation :
** 1-4   :    valeur (1-13, 11=J, 12=D, 13=K)
** 5-8   :    couleur (1=coeur,2=pic,4=trefle,8=carreau)
*/
class Client
{
    public:
        Client();
        ~Client();
        quint64 identifier() const;
        QString pseudo() const;

        quint16 dataSize() const;
        QByteArray data() const;

        void setPseudo(QString pseudo);
        void setDataSize(quint16 size);
        void setData(QByteArray d);

        bool operator==(Client const& client);
    private:
        quint64 m_identifier;
        QString m_pseudo;
        quint64 m_channel_identifier;
        quint16 m_dataSize;
        QByteArray m_data;

        QList< quint16> m_cards;
};
#endif // CLIENT_HPP
