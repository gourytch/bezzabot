#ifndef BOTTHREAD_H
#define BOTTHREAD_H

#include <QThread>

class BotThread : public QThread
{
    Q_OBJECT
public:
    explicit BotThread(QObject *parent = 0);

    virtual void run();
signals:

public slots:

};

#endif // BOTTHREAD_H
