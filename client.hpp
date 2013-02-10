#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <QtCore/QCoreApplication>
#include <QTcpSocket>
#include <iostream>

class Client
{

public:
    //construc/destr
    Client();
    ~Client();
    //get methods
    quint64 id() const;
    QString pseudo() const;

    //set methods
    int setPseudo(QString pseudo);

    //operator
    bool operator==(Client const& client);
private:
    quint64 m_id;
    QString m_pseudo;
};
#endif // CLIENT_HPP
