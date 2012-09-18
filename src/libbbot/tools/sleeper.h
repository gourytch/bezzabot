#ifndef SLEEPER_H
#define SLEEPER_H

#include <QThread>

//
// http://stackoverflow.com/questions/3831439/qthow-to-give-a-delay-in-loop-execution
//

class Sleeper : public QThread
{
public:
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

#endif // SLEEPER_H
