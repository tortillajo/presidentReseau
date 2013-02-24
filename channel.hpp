#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <QtCore/QCoreApplication>
#include <iostream>

class Channel : public QObject
{
    Q_OBJECT

    public:
        Channel();
        QList< quint64> listClientIdentifier();
        quint64 identifier() const;
        QString title() const;
        int howManyClient() const;
        bool clientIncluded(quint64 client_identifier) const;
        int findClientId(quint64 client_identifier);
        bool clientAreReady();

    private slots:
        void clientReady(quint64 client_identifier, bool value);
        bool addClient(quint64 client_identifier);
        bool delClient(quint64 client_identifier);
        void start();

    signals:
        void sendClient(QString,quint64);
        void readyToBegin();
        void sendToClient(QString,quint64);
        // TODO : SLOT = channelSendToClient

    private:
        //[PARAMETRES DIVERS]
        QList< QString> m_params;
        /*  n est impair
            params[n] = cle (nom du params)
            params[n+1] = valeur de la cle

          */

        quint64 m_identifier;
        QString m_title;
        QList< quint64> m_client_identifier;
        QList< bool> m_client_ready;
        bool m_started; // -1 si termine, 0 si pas encore lance, 1 si en cours

};

#endif // CHANNEL_HPP
