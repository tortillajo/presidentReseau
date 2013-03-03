#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <QtCore/QCoreApplication>
#include <iostream>

struct s_client
{
    quint64 identifier;
    bool ready;
};

class Channel : public QObject
{
    Q_OBJECT

    public:
        Channel();
        QList< quint64> listClientIdentifier();
        quint64 identifier() const;
        int howManyClient() const;
        bool clientIncluded(quint64 client_identifier) const;
        int findClientId(quint64 client_identifier);
        bool clientAreReady();
        int addClient(quint64 client_identifier);
        int delClient(quint64 client_identifier);

    private slots:
        void clientReady(quint64 client_identifier, bool value);
        void start();

    signals:
        void readyToBegin();

    private:
        //[PARAMETRES DIVERS]
        /*  n est impair
        **  params[n] = cle (nom du params)
        **  params[n+1] = valeur de la cle
        */
        QList< QString> m_params;
        QList< quint64> m_p_identifierShortedByRound; /* NOTICE : Faire une fonction capable
                                          ** d'envoyer à la fin l'id qui vient
                                          ** de jouer :) L'id en gaut de la
                                          ** liste joue les autres attendent.
                                          */

        quint64 m_identifier;
        QList< s_client> m_clients;
        bool m_started; // -1 si termine, 0 si pas encore lance, 1 si en cours

};

#endif // CHANNEL_HPP
