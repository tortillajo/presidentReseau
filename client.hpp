#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <iostream>
#include "channel.hpp"

class Client
{
    public:
        Client();
        ~Client();
        quint64 identifier() const;
        QString pseudo() const;

        quint16 dataSize() const;
        QByteArray data() const;

        int setPseudo(QString pseudo);
        int setDataSize(quint16 size);
        int setData(QByteArray d);

        bool operator==(Client const& client);
    private:
        quint64 m_identifier;
        QString m_pseudo;
        Channel *m_channel;
        quint16 m_dataSize;
        QByteArray m_data;

        QList< quint16> m_cards;
        /* attribuer un nombre a chaque carte ?
            en fonction de sa valeur ?
            2 quartet :
            1er quartet = valeur
            1-10 , 11 valet , 12 dame , 13 roi ?
            2eme quartet = couleur ( 1 2 4 8 )
            bit 1 trefle
            bit 2 pic
            bit 3 carreau
            bit 4 coeur ?
          */
};
#endif // CLIENT_HPP
