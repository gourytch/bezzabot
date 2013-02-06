#ifndef PAGE_GAME_ATLANTIS_H
#define PAGE_GAME_ATLANTIS_H

#include <QWebElementCollection>
#include <QWebElement>
#include <QVector>
#include "page_game.h"
#include "tools/currency.h"

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
        Currency        currency;
        PageTimer       cooldown;

        bool parse(QWebElement& info);
        QString toString(const QString &pfx) const;
    };

    QWebElement button_build_bathyscaphe;
    QWebElement button_launch_bathyscaphe;
    QWebElementCollection link_chests;

    PageTimer atlantis_reachable_cooldown;
    PageTimer bathyscaphe_build_timer;
    int total_bathyscaphes_count;
    int atlant_amount;
    int hangar_count;
    int hangar_max_count;
    int working_count;
    int next_bathyscaphe_price;
    Currency next_bathyscaphe_currency;
    bool boxgame;

    QVector<Bathyscaphe> bathyscaphes;


    explicit Page_Game_Atlantis(QWebElement& doc);

    virtual QString toString(const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse();

    bool parseHead();

    bool parsePier();

    bool parseSecret();

    bool parseSea();

    bool canBuyBathyscaphe();

    bool doBuyBathyscaphe();

    bool canLaunchBathyscaphe();

    bool doLaunchBathyscaphe();

    QDateTime findMinDivingCooldown();

signals:

public slots:

};

#endif // PAGE_GAME_ATLANTIS_H
