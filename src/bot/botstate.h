#ifndef BOTSTATE_H
#define BOTSTATE_H

#include <QObject>
#include <QString>
#include "parsers/page_game.h"

enum WorkType { // основная работа
    Work_None,
    Work_Sleeping,
    Work_Resting,
    Work_Watching,
    Work_Mining,
    Work_MineShopping,
    Work_BigOpening,
    Work_SmallOpening,
    Work_Farming,
    Work_Training,
    Work_Dressing,
    Work_Fighting,
    Work_ScaryFighting,
    Work_CaveExploring,
    Work_Guarding
};

QString toString(WorkType v);

enum SecWorkType { // неосновная работа
    SecWork_None,
    SecWork_Fishing,
    SecWork_Healing,
    SecWork_Gambling,
    SecWork_ClanGiving,
    SecWork_Shopping,
    SecWork_Reading,
    SecWork_Trading
};

QString toString(SecWorkType v);


class BotState : public QObject
{
    Q_OBJECT
public:
    explicit BotState(QObject *parent = 0);

    void reset();

    void update_from_page(const Page_Game *p);

public: // всё, описывающее текущее состояние бота
    int         level;
    QString     charname;
    QString     chartitle;

    int         gold;
    int         free_gold;
    int         crystal;
    int         free_crystal;

    int         fish;
    int         green;

    int         hp_cur;
    int         hp_max;
    int         hp_spd;

    int         dozors_remains; // осталось на сегодня десятиминуток дозоров
    int         fishraids_remains; // осталось на сегодня заплывов за пирашками

    int         dozor_price; // стоимость дозора

    WorkType    primary_work; // текущая работа
    SecWorkType secondary_work; // дополнительная работа

signals:

public slots:

};

#endif // BOTSTATE_H
