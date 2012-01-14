#include <stdio.h>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "config.h"
#include "logger.h"
#include "tools.h"

#include <iostream>

int Logger::_count = 0;
Logger *Logger::_instance = NULL;
QtMsgHandler Logger::_prev_handler = NULL;

Logger::Logger(QObject *parent) :
    QObject(parent) {
    ++_count;
//    clog << "LOG[count=" << _count << "]" << endl;
    Config& cfg = Config::global();

    setLevel((QtMsgType)cfg.get("log/level_out", (int)QtDebugMsg).toInt(),
             (QtMsgType)cfg.get("log/level_log", (int)QtDebugMsg).toInt());

    _fname = cfg.get("log/name", false, Config::appName()).toString();
    if (!_fname.contains('.')) {
        _fname += ".log";
    }
    if (!(_fname.startsWith("/") || _fname.startsWith("./"))) {
        _fname = cfg.dataPath() + "/" + _fname;
    }
    QDateTime t = QDateTime::currentDateTime();
    _fname = _fname.replace("{TS}", t.toString("yyyyMMdd_hhmmss"))
            .replace("{DATE}", t.toString("yyyyMMdd"));
    _file = new QFile (_fname, this);
    if (!_file->open(QFile::WriteOnly)) {
        delete _file;
        _file = NULL;
        _stream = NULL;
    } else {
        _stream = new QTextStream(_file);
    }
}

Logger::~Logger() {
    if (--_count <= 0) {
        if (_instance == this) {
            log(QtDebugMsg, "end logging");
        }
        if (_prev_handler) {
            qInstallMsgHandler(_prev_handler);
            _prev_handler = NULL;
        }
    }
}

Logger& Logger::global() {
    if (!_instance) {
        _instance = new Logger(NULL);
        _prev_handler = qInstallMsgHandler (_handler);
        _instance->log(QtDebugMsg, "start logging");
    }
    return *_instance;
}

//static
void Logger::_handler(QtMsgType mtype, const char *text) {
    global().log(mtype, text);
}

void Logger::setLevel(QtMsgType to_out, QtMsgType to_file) {
    _lvl_cout = to_out;
    _lvl_file = to_file;
}

void Logger::log(QtMsgType mtype, const char *text) {
    if ((int)mtype > 3) mtype = (QtMsgType)3;
    if ((int)mtype < 0) mtype = (QtMsgType)0;

    if (mtype < _lvl_cout && mtype < _lvl_file) return;

    QString t = u8(text);

    QDateTime ts = QDateTime::currentDateTime();
    QString tss_file = ts.toString("yyyy-MM-dd hh:mm:ss");
    QString tss_cout = ts.toString("hh:mm:ss");
    QString m_text = t.replace(QString("\n"), QString("\n    "));
    QString m_lvl = " -" + QString("DWCF???"[(int)mtype]) + "-  ";

    if (_lvl_file <= mtype && _stream) {
        (*_stream) << tss_file + m_lvl + m_text << "\n";
        _stream->flush();
    }
    if (_lvl_cout <= mtype) {
        QString m = tss_cout + m_lvl + m_text;
//        fprintf(stderr, "%s\n", qPrintable(m));
        std::clog << m.toUtf8().constBegin() << std::endl;
    }

    switch (mtype) {
    case QtDebugMsg:
        emit signalDebug(t);
        break;
    case QtWarningMsg:
        emit signalWarning(t);
        break;
    case QtCriticalMsg:
        emit signalCritical(t);
        break;
    default:
        emit signalFatal(t);
        break;
    }
}

void Logger::slotDebug(const QString& msg) {
    log(QtDebugMsg, msg.toUtf8().constData());
}

void Logger::slotWarning (const QString& msg) {
    log(QtWarningMsg, msg.toUtf8().constData());
}

void Logger::slotCritical(const QString& msg) {
    log(QtCriticalMsg, msg.toUtf8().constData());
}

void Logger::slotFatal(const QString& msg) {
    log(QtFatalMsg, msg.toUtf8().constData());
}
