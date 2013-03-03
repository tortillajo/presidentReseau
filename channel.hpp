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
        bool addClient(quint64 client_identifier);
        bool delClient(quint64 client_identifier);

    private slots:
        void clientReady(quint64 client_identifier, bool value);
        void start();

    signals:
        void readyToBegin();
        void sendToClient(QString,quint64);

    private:
        //[PARAMETRES DIVERS]
        /*  n est impair
        **  params[n] = cle (nom du params)
        **  params[n+1] = valeur de la cle
        */
        QList< QString> m_params;
        QList< int> m_p_idShortedByRound; /* NOTICE : Faire une fonction capable
                                          ** d'envoyer à la fin l'id qui vient
                                          ** de jouer :) L'id en gaut de la
                                          ** liste joue les autres attendent.
                                          */

        quint64 m_identifier;
        QString m_title;
        QList< quint64> m_client_identifier;
        QList< bool> m_client_ready;
        bool m_started; // -1 si termine, 0 si pas encore lance, 1 si en cours

};

#endif // CHANNEL_HPP
