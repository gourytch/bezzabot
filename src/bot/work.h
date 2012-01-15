#ifndef WORK_H
#define WORK_H

#include <QObject>
#include <QString>

#include "botstate.h"
#include "parsers/page_game.h"

class Bot;

class Work : public QObject
{

    Q_OBJECT
protected:

    Bot *_bot;

    QString _workLink;

    void gotoWork();

    bool hasWork() const;

    bool isWorkReady() const;

    bool isMyWork() const;

    bool isNotMyWork() const;

    void setAwaiting();

public:

    explicit Work(Bot *bot);

    enum Query {
        DummyQuery, // пустой запрос
        CanStartWork,    // можно ли начать сейчас эту работу?
        CanStartSecondaryWork, // можно ли сейчас переключиться на другую работу?
        CanCancelWork // можно ли досрочно завершить работу?
    };

    enum Command {
        DummyCommand,
        Reset,               // сбросить внутреннее состояние
        StartWork,           // необходимо начать работу
        StartSecondaryWork,  // переключаемся на побочную работу
        FinishSecondaryWork, //  побочная работа завершена
        CancelWork           // необходимо досрочно завершить работу
    };

    virtual bool isEnabled () const;

    virtual QString toString() const;

    virtual bool isPrimaryWork() const = 0; // true ::= основная работа

    virtual WorkType getWorkType() const = 0; // тип выполняемой работы

    virtual QString getWorkName() const; // уникальное название работы

    virtual QString getWorkStage() const = 0; // состояние работы

    // вызов при возможности нового шага
    // возврат: true ::= работа не завершена и должна быть продолжена
    virtual bool nextStep() = 0;

    // вызов при наличии новой страницы
    // возврат: true ::= работа не завершена и должна быть продолжена
    virtual bool processPage(const Page_Game *gpage) = 0;

    // получение информации об возможности изменения состояния работы
    // возврат: ответ на запрос
    virtual bool processQuery(Query query) = 0;

    // команда/извещение изменения состояния работы
    // возврат: true ::= работа будет продолжена
    virtual bool processCommand(Command command) = 0;

};

#endif // WORK_H
