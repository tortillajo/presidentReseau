#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <QtCore/QCoreApplication>
#include <iostream>

class Channel : QObject
{
    Q_OBJECT

    public:
        Channel();

    private:
        QList< quint16> m_cards;
        /* attribuer un nombre a chaque carte ?
            en fonction de sa valeur ?
            2 octets :
            1er octet = valeur
            1-10 , 11 valet , 12 dame , 13 roi ?
            2eme octet = couleur
            bit 1 trefle
            bit 2 pic
            bit 3 carreau
            bit 4 coeur ?
          */

        //[PARAMETRES DIVERS]
        //QVector< QString> m_params;
        /*  n est impaire
            params[n] = cle (nom du params)
            params[n+1] = valeure de la cle

          */

        QList< QString> m_client_id;
        QString m_id;
        bool m_started; // -1 si termine, 0 si pas encore lance, 1 si en cours

};

#endif // CHANNEL_HPP
