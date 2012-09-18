#ifndef TIMEBOMB_H
#define TIMEBOMB_H

#include <QObject>
#include <QProgressBar>
#include <QPalette>


class Timebomb : public QObject
{
    Q_OBJECT

protected:
    explicit Timebomb(QObject *parent = 0);

    QProgressBar *_bar;
    QObject      *_receiver;
    const char   *_member;

    static Timebomb *_instance;

    QPalette    _palette;
    int         _msecs;
    int         _ticksTotal;
    int         _ticksLeft;
    int         _timer_id;
    bool        _active;

    void makeActive();
    void makeInactive();
    void setupBar(QColor c1 = Qt::blue, QColor c2 = Qt::darkBlue,
                  QString fmt = "%p%",
                  int min = 0, int max = 100, int val = 0);

public:
    ~Timebomb();

    static void bind(QProgressBar *bar);
    static Timebomb *global() {return _instance;}

    void unbind();
    void launch(int ms, QObject *receiver, const char *member);
    void dedicatedWait(int ms);
    void startWaiter();
    void startDownloader();
    void updateDownloader(int percents);
    void finishDownloader();
    void cancel();
    void defuse();

    bool isTimerActive() const {
        return (_timer_id > 0);
    }

    bool isActive() const {
        return _active;
    }

    QProgressBar *bar() {
        return _bar;
    }

    void timerEvent(QTimerEvent *);

signals:

protected slots:

public slots:


};

#endif // TIMEBOMB_H
