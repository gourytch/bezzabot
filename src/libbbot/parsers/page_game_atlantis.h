#ifndef PAGE_GAME_ATLANTIS_H
#define PAGE_GAME_ATLANTIS_H

#include <QVector>
#include "page_game.h"

enum BathyState {
    Unpaid,
    Awaiting,
    Building,
    Diving
};

extern QString toString(BathyState v);

class Page_Game_Atlantis : public Page_Game
{
    Q_OBJECT

public:

    struct Bathyscaphe {
        bool            enabled;
        BathyState      state;
        int             price;
        QString         currency;
        QVector<int>    modules;
        PageTimer       cooldown;

        bool parse(QWebElement& info);
        QString toString(const QString &pfx) const;
    };

    PageTimer atlantis_reachable_cooldown;
    int total_bathyscaphes_count;
    int atlant_amount;
    int hangar_count;
    int hangar_max_count;
    int working_count;
    bool boxgame;

    QVector<Bathyscaphe> bathyscaphes;


    explicit Page_Game_Atlantis(QWebElement& doc);

    virtual QString toString(const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse();

    bool canBuyBathyscaphe();

    bool doBuyBathyscaphe();

    bool canLaunchBathyscaphe();

    bool doLaunchBathyscaphe();

signals:

public slots:

};

#endif // PAGE_GAME_ATLANTIS_H
