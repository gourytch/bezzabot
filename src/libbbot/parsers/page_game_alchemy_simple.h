#ifndef PAGE_GAME_ALCHEMY_SIMPLE_H
#define PAGE_GAME_ALCHEMY_SIMPLE_H

#include "page_game.h"
#include <QMap>


class Page_Game_Alchemy_Simple : public Page_Game
{
    Q_OBJECT
public:

    explicit Page_Game_Alchemy_Simple(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse();

    int mastery;    // мастерство по изготовлению
    int accuracy;   // аккуратность изготовления
    int quality;    // качество зелья
    int brewsec;    // время изготовления зелья в секундах

    typedef QMap<QString, int> CountMap;
    CountMap resources; // доступные ингридиенты и их количество
    CountMap potions; // варианты зелий

signals:
    
public slots:
    
};

#endif // PAGE_GAME_ALCHEMY_SIMPLE_H
