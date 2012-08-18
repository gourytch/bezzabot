#include <QRegExp>
#include <QTime>
#include <QString>
#include <QStringList>
#include "activityhours.h"
#include "tools.h"

ActivityHours::ActivityHours(const QString defstr) {
    assign(defstr);
}

void ActivityHours::assign(int hour, bool active) {
    _active[hour % 24] = active;
}

void ActivityHours::assign(const QString defstr) {
    for (int i = 0; i < 24; ++i) {
        _active[i] = false;
    }
    if (defstr.isEmpty()) {
        return;
    }
//    qDebug("defstr={" + defstr + "}");
    QRegExp rx1("^(\\d+)$");
    QRegExp rxM("^(\\d+)-(\\d+)$");
    foreach (QString defchunk, defstr.split(' ')) {
        QString s = defchunk.trimmed();
//        qDebug("chunk={" + s + "}");
        if (s.isEmpty()) continue;

        if (rx1.indexIn(s) != -1) {
            int h = rx1.cap(1).toInt();
//            qDebug("assign %d", h);
            assign(h, true);
            continue;
        }
        if (rxM.indexIn(s) != -1) {
            int h1 = rxM.cap(1).toInt();
            int h2 = rxM.cap(2).toInt();
//            qDebug("assign %d..%d", h1, h2);
            if (h1 <= h2) {
                for (int h = h1; h <= h2; ++h) {
                    assign(h, true);
                }
            }  else {
                for (int h = h1; h < 24; ++h) {
                    assign(h, true);
                }
                for (int h = 0; h <= h2; ++h) {
                    assign(h, true);
                }
            }
            continue;
        }
        qCritical("bad activity hour chunk {" + s + "} in activity def {" +
                  defstr + "}");
    }
}

bool ActivityHours::isActive() const {
    return isActive(QTime::currentTime().hour());
}

bool ActivityHours::isActive(int hour) const {
    return _active[hour % 24];
}

QString ActivityHours::toString() const {
    QString s;
    int t0 = -1;
    for (int i = 0; i < 24; ++i) {
        if (_active[i]) {
            if (t0 == -1) {
                t0 = i;
                continue;
            }
        } else { // ! active
            if (t0 != -1) {
                if (!s.isEmpty()) {
                    s += ",";
                }
                if (i - t0 == 1) {
                    s += QString::number(t0);
                } else {
                    s += QString("%1-%2").arg(t0).arg(i - 1);
                }
                t0 = -1;
            }
        }
    }
    if (t0 != -1) {
        if (!s.isEmpty()) {
            s += ",";
        }
        if (24 - t0 == 1) {
            s += QString::number(t0);
        } else {
            s += QString("%1-%2").arg(t0).arg(23);
        }
    }
    return s;
}

QString ActivityHours::timeline() const {
    QString s;
    for (int i = 0; i < 24; ++i) {
        s += _active[i] ? "#" : ".";
    }
    return s;
}

int ActivityHours::seg_length(int hour) const {
    int len = 0;
    for (int i = hour; i < (hour + 24); ++i) {
        int ix = i % 24;
        if (!_active[ix]) {
            return len;
        }
        ++len;
    }
    if (len > 24) {
        return 24;
    }
    return len;
}

int ActivityHours::seg_length() const {
    return seg_length(QTime::currentTime().hour());
}
