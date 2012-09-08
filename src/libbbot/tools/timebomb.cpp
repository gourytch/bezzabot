#include <QEventLoop>
#include <QTimer>
#include <QTime>
#include "timebomb.h"
#include "sleeper.h"
#include "tools.h"

Timebomb *Timebomb::_instance = NULL;


Timebomb::Timebomb(QObject *parent) :
    QObject(parent),
    _bar(NULL),
    _receiver(NULL),
    _member(NULL)
{
    _ticksTotal = 100;
    _timer_id = -1;
    _active = false;
}


Timebomb::~Timebomb() {
    if (isTimerActive()) {
        defuse();
    }
    if (this == _instance) {
        qDebug("remove Timebomb::instance");
        _instance = NULL;
    }
}


void Timebomb::bind(QProgressBar *bar) {
    if (!_instance) {
        _instance = new Timebomb(bar);
    }
    _instance->_bar = bar;
}


void Timebomb::unbind() {
    if (isTimerActive()) {
        defuse();
    }
    _bar = NULL;
}


void Timebomb::launch(int ms, QObject *receiver, const char *member) {
//    qDebug("Timebomb::launch(%d <%s>)", ms, member);
    Q_CHECK_PTR(_bar);
    if (isTimerActive()) {
        defuse();
    }
    _receiver = receiver;
    _member   = member;
    setupBar(Qt::red, Qt::darkRed, "%v", 0, ms, ms);
    makeActive();

    _msecs = ms;
    _ticksLeft = _ticksTotal;
    int ival = _msecs / _ticksTotal;
    if (ival == 0) ival = 1;
    _timer_id = startTimer(ival);
    Q_ASSERT(_timer_id > 0);
//    qDebug(u8("timer #%1, ival=%2 for object {%3}, member {%4} started")
//           .arg(_timer_id)
//           .arg(ival)
//           .arg(receiver->objectName())
//           .arg(member));
}


void Timebomb::dedicatedWait(int ms) {
//    qDebug("Timebomb::dedicatedWait(%d)", ms);
    Q_CHECK_PTR(_bar);
    if (isTimerActive()) {
        defuse();
    }

    int mchunk = ms / 100;
    if (mchunk == 0) mchunk = 1;

    setupBar(Qt::magenta, Qt::darkMagenta, "%v", 0, ms, 0);
    makeActive();

    QTime time;
    QEventLoop loop;
    time.start();
    while (time.elapsed() < ms) {
        _bar->setValue(time.elapsed());
        loop.processEvents(QEventLoop::ExcludeUserInputEvents);
        Sleeper::msleep(mchunk);
    }
    cancel();
//    qDebug("Timebomb::dedicatedWait() finished");
}


void Timebomb::startWaiter() {
//    qDebug("Timebomb::startWaiter()");
    Q_CHECK_PTR(_bar);
    if (isTimerActive()) {
        defuse();
    }
    setupBar(Qt::magenta, Qt::darkMagenta, "", 0, 0, 0);
    makeActive();
}


void Timebomb::startDownloader() {
//    qDebug("Timebomb::startDownloader()");
    Q_CHECK_PTR(_bar);
    if (isTimerActive()) {
        defuse();
    }
    setupBar();
    makeActive();
}


void Timebomb::updateDownloader(int percents) {
//    qDebug("Timebomb::updateDownloader(%d)", percents);
    Q_CHECK_PTR(_bar);
    _bar->setValue(percents);
}


void Timebomb::finishDownloader() {
//    qDebug("Timebomb::finishDownloader()");
    if (!isTimerActive()) {
        cancel(); // avoid defusing
    }
}


void Timebomb::timerEvent(QTimerEvent*) {
    Q_CHECK_PTR(_bar);
    if (--_ticksLeft <= 0) {
//        qDebug("ka-boom!");
        QObject *p = _receiver;
        const char *s = _member;
        cancel();
        QTimer::singleShot(0, p, s);
    } else {
        int n = _ticksLeft * _msecs / _ticksTotal;
        if (_ticksLeft == _ticksTotal - 1) {
//            qDebug("tick-tock..");
        }
        _bar->setValue(n);
    }
}

void Timebomb::defuse() {
    if (!isTimerActive()) return;
    if (_receiver && _member) {
//        qDebug(u8("defuse timer #%1 (object {%2}, member {%3})")
//               .arg(_timer_id)
//               .arg(_receiver->objectName())
//               .arg(_member));
    } else {
        qDebug("defuse bronken timer");
    }
    killTimer(_timer_id);
    _timer_id = -1;
    _receiver = NULL;
    _member = NULL;
}


void Timebomb::cancel() {
    if (!isActive()) {
//        qDebug("skip cancel dup");
        return;
    }
//    qDebug("cancel timebomb");
    defuse();
    makeInactive();
}


void Timebomb::makeActive() {
    if (isActive()) {
//        qDebug("skip activation dup");
        return;
    }
    _active = true;
    _bar->setEnabled(true);
}


void Timebomb::makeInactive() {
    if (!isActive()) {
//        qDebug("skip deactivation dup");
        return;
    }
    Q_CHECK_PTR(_bar);
    _active = false;
    setupBar(Qt::gray, Qt::gray, "", 0, 1, 1);
//    _bar->reset();
    _bar->setEnabled(false);
}


void Timebomb::setupBar(QColor c1, QColor c2,
                        QString fmt,
                        int min, int max, int val) {
    Q_CHECK_PTR(_bar);
    _palette  = _bar->palette();
    QPalette newPalette(_palette);
    newPalette.setColor(QPalette::Active, QPalette::Highlight, c1);
    newPalette.setColor(QPalette::Inactive, QPalette::Highlight, c2);
    _bar->setPalette(newPalette);
    _bar->setFormat(fmt);
    _bar->setRange(min, max);
    _bar->setValue(val);
}


