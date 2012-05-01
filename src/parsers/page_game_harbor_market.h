#ifndef PAGE_GAME_HARBOR_MARKET_H
#define PAGE_GAME_HARBOR_MARKET_H

#include <QMap>
#include <QVector>
#include "page_game.h"

class Page_Game_Harbor_Market : public Page_Game
{
    Q_OBJECT

    struct Item {
        QString id;
        QString name;
        QString icon;
        double  price;
        int     price_type;
        int     min;
        int     max;
        int     step;
        QString toString() const;
    };

    QVector<Item> _items;

public:

    explicit Page_Game_Harbor_Market(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parseGoods();

    bool parseJSItems();

    bool parseMessage();

    bool doSelectItem(QString id);

    bool doSelectQuantity(int quantity);

    bool doBuy();

    bool doSelectAndBuy(QString name, int quantity);

    QMap<QString, QString> lotsIds; // текст -> id

    QString selectedLotId;

    QString selectedLotOptValue;

    QString lotIcon;

    int lotQuantity;

    int lotPrice;

    QString lotCurrency;

    int previousBuyResult;

};

#endif // PAGE_GAME_HARBOR_MARKET_H
