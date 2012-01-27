#include "timebomb.h"

Timebomb *Timebomb::_instance = NULL;

Timebomb::Timebomb(QObject *parent) :
    QObject(parent),
    _bar(NULL),
    _timer(NULL),
    _receiver(NULL),
    _member(NULL)
{
    _ticksTotal = 100;
}

void Timebomb::bind(QProgressBar *bar) {
    if (!_instance) {
        _instance = new Timebomb(bar);
    }
    _instance->_bar = bar;
}

void Timebomb::unbind() {
    _bar = NULL;
}

void Timebomb::launch(int ms, QObject *receiver, const char *member) {
    cancel();
    _receiver = receiver;
    _member   = member;
    if (_bar) {
        _bar->setEnabled(true);
        _palette  = _bar->palette();
        QPalette newPalette(_palette);
        newPalette.setColor(QPalette::Active, QPalette::Highlight, Qt::red);
        _bar->setPalette(newPalette);
        _bar->setFormat("%v");
        _bar->setRange(0, ms);
        _bar->setValue(ms);
    }
    _timer = new QTimer(this);
    _msecs = ms;
    _ticksLeft = _ticksTotal;
    _timer->setInterval(_msecs / _ticksTotal);
    connect(_timer, SIGNAL(timeout()), this, SLOT(tick()));
    _timer->setSingleShot(false);
    _timer->start();
}

void Timebomb::cancel() {
    if (!_timer) return;
    delete _timer;
    _timer = NULL;

    _receiver = NULL;
    _member = NULL;
    if (_bar) {
        _bar->setPalette(_palette);
        _bar->setFormat("%p%");
        _bar->setRange(0, 100);
        _bar->setValue(0);
        _bar->reset();
        _bar->setEnabled(false);
    }
}

void Timebomb::tick() {
    if (--_ticksLeft <= 0) {
        QObject *p = _receiver;
        const char *s = _member;
        cancel();
        QTimer::singleShot(0, p, s);
    } else {
        if (_bar) {
            int n = _ticksLeft * _msecs / _ticksTotal;
            _bar->setValue(n);
        }
    }
}
