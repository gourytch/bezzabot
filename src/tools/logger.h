#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

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
    void signalError(const QString& msg);
    void signalXError(const QString& msg);

public slots:
    void slotDebug(const QString& msg);
    void slotWarning (const QString& msg);
    void slotError(const QString& msg);
    void slotXError(const QString& msg);

};

#endif // LOGGER_H