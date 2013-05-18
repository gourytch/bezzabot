#ifndef PAGE_GAME_ALCHEMY_LAB_H
#define PAGE_GAME_ALCHEMY_LAB_H

#include "page_game.h"

class Page_Game_Alchemy_Lab : public Page_Game
{
    Q_OBJECT
public:
    explicit Page_Game_Alchemy_Lab(QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    static bool fit(const QWebElement& doc);

    bool parse();

    bool doSmelting();

    bool doFreezing();

    int bubbles_cur;
    int bubbles_max;
    int bottles_cur;
    int bottles_max;

    bool can_smelting;
    bool can_freezing;

    PageTimer smelting_timer;
    PageTimer freezing_timer;

    QWebElement submit_smelting;
    QWebElement submit_freezing;

signals:

public slots:

private:
    bool doPush(QWebElement e);
    bool gotSignal;

private slots:
    virtual void slot_update_finished();

};

#endif // PAGE_GAME_ALCHEMY_FURNACES_H
