#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "config.h"
#include "logger.h"
#include "tools.h"

Logger *Logger::_instance = NULL;
QtMsgHandler Logger::_prev_handler = NULL;

Logger::Logger(QObject *parent) :
    QObject(parent) {
    setLevel(QtDebugMsg, QtDebugMsg);
    Config& cfg = Config::global();
    _fname = cfg.dataPath() + "/"
            + cfg.get("logname", false, Config::appName()).toString()
            + ".log";
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
    log(QtDebugMsg, "end logging");
}

Logger& Logger::global() {
    if (!_instance) {
        _instance = new Logger(NULL);
    }
    _prev_handler = qInstallMsgHandler (_handler);
    _instance->log(QtDebugMsg, "start logging");
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
    if (mtype < _lvl_cout && mtype < _lvl_file) return;

    QDateTime t = QDateTime::currentDateTime();
    QString m = QString("%1 -%2-  %3")
            .arg(t.toString("yyyy-MM-dd hh:mm:ss"))
            .arg("DWEX???"[(int)mtype])
            .arg(u8(text).replace("\n", "\n    "));
    if (_lvl_file <= mtype && _stream) {
        (*_stream) << m;
        _stream->flush();
    }
    if (_lvl_cout <= mtype) {
        if (_prev_handler) {
            _prev_handler(mtype, text);
        } else {
            std::clog << qPrintable(m) << std::endl;
        }
    }
}
