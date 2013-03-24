#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <QtCore/QCoreApplication>

#include <QDebug>

typedef struct s_channel_client s_channel_client;
struct s_channel_client
{
    quint64 identifier;
    bool ready;
};

/*
** DOCUMENTATION : LES METHODES DE LA CLASSE
**
** listClientIdentifier()
** recuperer la liste des identifiant des clients du channel
**
** howManyClient()
** renvoie au nombre de clients du channel
**
** clientIncluded(quint64 client_identifier)
** tester si le client client_identifier est inclue dans le channel
**
** findClientId(quint64 client_identifier)
** trouve l'id du client client_identifier
**
** SLOT clientsAreReady()
** teste si tous les clients sont prets à jouer.
**
** SLOT start()
** lance la partie : distribution des cartes, tour, ...
**
** play(QString card, quint64 client_identifier)
** client_identifier essaye joue la carte card
**
**
** DOCUMENTATION : ATTRIBUTS DE LA CLASSE
**
** m_p_identifierShortedByRound
** liste des identifiants des joueurs triés dans l'ordre de jeu. (0 commence)
**
** m_round
** incrémenté à chaque fois qu'un joueur joue (gestion du tour)
**
**
** DOCUMENTATION : GESTION DES CARTES
** les cartes sont représentées par un nombre de 8 bits
** octet :    utilisation :
** 1-4   :    valeur (1-13, 11=J, 12=D, 13=K)
** 5-8   :    couleur (1=coeur,2=pic,4=trefle,8=carreau)
*/

class Channel : public QObject
{
    Q_OBJECT

    public:
        Channel();
        QList< quint64> listClientIdentifier();
        quint64 identifier() const;
        int howManyClient() const;

        bool clientIncluded(quint64 client_identifier) const;
        bool clientsAreReady();
        int findClientId(quint64 client_identifier);

        int addClient(quint64 client_identifier);
        int delClient(quint64 client_identifier);

        void clientReady(quint64 client_identifier, bool value);

        int play(QString card, quint64 client_identifier);

    private slots:
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
        QList< quint64> m_p_identifierShortedByRound;
        quint64         m_round;
        /* NOTICE : Faire une fonction capable
        ** d'envoyer à la fin l'id qui vient
        ** de jouer :) L'id en gaut de la
        ** liste joue les autres attendent.
        */

        quint64         m_identifier;
        QList< s_channel_client> m_clients;
        bool            m_started;

};

#endif // CHANNEL_HPP
