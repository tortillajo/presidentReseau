#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <iostream>
#include "channel.hpp"

class Client
{
    public:
        //construc/destr
        Client();
        ~Client();
        //get methods
        quint64 identifier() const;
        QString pseudo() const;

        quint16 dataSize() const;
        QByteArray data() const;

        //set methods
        int setPseudo(QString pseudo);
        int setDataSize(quint16 size);
        int setData(QByteArray d);

        //operator
        bool operator==(Client const& client);
    private:
        quint64 m_identifier;
        QString m_pseudo;

        Channel *m_channel;

        quint16 m_dataSize; // contient la taille du message
        QByteArray m_data; // contient le message
};
#endif // CLIENT_HPP
