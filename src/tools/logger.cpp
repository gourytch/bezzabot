#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "config.h"
#include "logger.h"
#include "tools.h"

using namespace std;

int Logger::_count = 0;
Logger *Logger::_instance = NULL;
QtMsgHandler Logger::_prev_handler = NULL;

Logger::Logger(QObject *parent) :
    QObject(parent) {
    clog << "LOG[count=" << (++_count) << "]" << endl;
    Config& cfg = Config::global();
    setLevel(QtDebugMsg, QtDebugMsg);
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
    if (_instance == this) {
        log(QtDebugMsg, "end logging");
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

    QDateTime ts = QDateTime::currentDateTime();
    QString m = ts.toString("yyyy-MM-dd hh:mm:ss");
    m += " -";
    m += QString("DWEX???"[(int)mtype]);
    m += "-  ";
    QString t = u8(text);
    m += t.replace(QString("\n"), QString("\n    "));
    if (_lvl_file <= mtype && _stream) {
        (*_stream) << m << "\n";
        _stream->flush();
    }
    if (_lvl_cout <= mtype) {
//        if (_prev_handler) {
//            _prev_handler(mtype, text);
//        } else {
            std::clog << qPrintable(m) << std::endl;
//        }
    }
}
