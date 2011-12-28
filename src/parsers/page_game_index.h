#ifndef PAGE_GAME_INDEX_H
#define PAGE_GAME_INDEX_H

#include "page_game.h"

enum SafeKind
{
    NoSafe,
    SimpleSafe,
    GoldenSafe,
    PermanentSafe
};


struct Stat
{
    float base;
    float total;
    float offense;
    float defence;
};


class Page_Game_Index : public Page_Game
{
    Q_OBJECT
public:
    // сейф
    // кукла вуду
    // знак вуду
    // бронзовый тотем
    // золотой тотем

    // шлем
    // кулон
    // оружие
    // щит
    // броня

    // зелья
    // одевалка

    // здоровье
    int hp_cur;
    int hp_max;

    // опыт
    int xp_cur;
    int xp_left;

    int level; // уровень

    Stat power;
    Stat block;
    Stat dexterity;
    Stat endurance;
    Stat charisma;

    int glory_base;
    int glory_penalty;
    int glory_scare;
    int glory_medals;
    int glory_casts;
    int glory_total_mod;

    // PET

    int player_id; // из урла статистики

    Page_Game_Index (QWebElement& doc);
    virtual QString toString (const QString& pfx = QString ()) const;
};


#endif // PAGE_GAME_INDEX_H
