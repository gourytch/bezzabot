#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "build_id.h"

class Logger : public QObject
{
    Q_OBJECT
protected:
    static int      _count;
    static Logger  *_instance;
    static QtMsgHandler _prev_handler;
    QString         _fname;
    QFile          *_file;
    QTextStream    *_stream;
    QtMsgType       _lvl_cout;
    QtMsgType       _lvl_file;

    static void _handler(QtMsgType mtype, const char *text);
public:
    explicit Logger(QObject *parent = 0);
    virtual ~Logger();

    static Logger& global();
    void log(QtMsgType mtype, const char *text);
    void setLevel(QtMsgType to_out, QtMsgType to_file);

signals:
    void signalDebug(const QString& msg);
    void signalWarning (const QString& msg);
    void signalCritical(const QString& msg);
    void signalFatal(const QString& msg);

public slots:
    void slotDebug(const QString& msg);
    void slotWarning (const QString& msg);
    void slotCritical(const QString& msg);
    void slotFatal(const QString& msg);

};

#endif // LOGGER_H
