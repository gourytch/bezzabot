#ifndef PAGE_GAME_HARBOR_MARKET_H
#define PAGE_GAME_HARBOR_MARKET_H

#include <QMap>
#include "page_game.h"

class Page_Game_Harbor_Market : public Page_Game
{
    Q_OBJECT

public:


    explicit Page_Game_Harbor_Market(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parseGoods();

    bool doSelectLotByName(QString name);

    bool doSelectLotById(QString id);

    bool doSelectLotQuantity(int quantity);

    bool doBuy();

    bool doSelectAndBuy(QString name, int quantity);

    QMap<QString, QString> lotsIds; // текст -> id

    QString selectedLotId;

    QString lotIcon;

    int lotQuantity;

    int lotPrice;

    QString lotCurrency;

};

#endif // PAGE_GAME_HARBOR_MARKET_H
