#ifndef ACTIVITYHOURS_H
#define ACTIVITYHOURS_H

#include <QString>

class ActivityHours
{
    bool _active[24];
public:
    ActivityHours(const QString defstr = "");
    void assign(const QString defstr);
    void assign(int hour, bool active);
    bool isActive() const;
    bool isActive(int hour) const;
    QString toString() const;
    QString timeline() const;
    int seg_length(int hour) const;
    int seg_length() const;
};

#endif // ACTIVITYHOURS_H
