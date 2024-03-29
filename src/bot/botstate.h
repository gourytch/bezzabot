#ifndef BOTSTATE_H
#define BOTSTATE_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include "farmersgroupsprices.h"
#include "parsers/page_game.h"

enum WorkType { // основная работа
    Work_None,
    Work_Sleeping,
    Work_Resting,
    Work_Watching,
    Work_Mining,
    Work_MineShopping,
    Work_FieldsOpening,
    Work_Farming,
    Work_Training,
    Work_Dressing,
    Work_Fighting,
    Work_ScaryFighting,
    Work_CaveExploring,
    Work_Guarding,
     // неосновная работа
    Work_Fishing,
    Work_Healing,
    Work_Gambling,
    Work_ClanGiving,
    Work_Shopping,
    Work_Reading,
    Work_Trading,
    Work_LuckySquare,
    Work_FlyingBreeding
};

QString toString(WorkType v);


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

    WorkGuild   work_guild; // в какой рабочей гильдии состоит
    FGPRecord   fgp;

    int         gold;
    int         free_gold;
    int         crystal;
    int         free_crystal;

    int         fish;
    int         green;

    int         hp_cur;
    int         hp_max;
    int         hp_spd;

    int         xp_left;

    int         dozors_remains; // осталось на сегодня десятиминуток дозоров
    int         fishraids_remains; // осталось на сегодня заплывов за пирашками
    int         pickaxes_remains; // сколько осталось зарядов у кирок
    int         bigtickets_remains; // сколько есть больших билетов
    int         smalltickets_remains; // сколько есть маленьких билетиков

    int         dozor_price; // стоимость дозора

    int         plant_income;
    int         plant_capacity;
    int         plant_slaves;

    WorkType    primary_work; // текущая работа

    QDateTime   hardminer_effect; // время истечения УШ

    QDateTime   atime; // pit последнего доступа к серверу

signals:

public slots:

};

#endif // BOTSTATE_H
