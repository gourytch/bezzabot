#ifndef TIMEBOMB_H
#define TIMEBOMB_H

#include <QObject>
#include <QProgressBar>
#include <QPalette>
#include <QTimer>


class Timebomb : public QObject
{
    Q_OBJECT

protected:
    explicit Timebomb(QObject *parent = 0);

    QProgressBar *_bar;
    QTimer       *_timer;
    QObject      *_receiver;
    const char   *_member;

    static Timebomb *_instance;

    QPalette    _palette;
    int         _msecs;
    int         _ticksTotal;
    int         _ticksLeft;

public:

    static void bind(QProgressBar *bar);
    static Timebomb *global() {return _instance;}

    void unbind();
    void launch(int ms, QObject *receiver, const char *member);
    void cancel();
    bool isActive() const {
        return (_timer != NULL);
    }

    QProgressBar *bar() {
        return _bar;
    }

signals:

protected slots:

    void tick();

public slots:


};

#endif // TIMEBOMB_H
