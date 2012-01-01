#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

void LOG(const QString& text);

QString now (bool utc=false);

QDateTime nextDay();

void save (const QString& fname, const QString& text);

QString load (const QString& fname);

inline QString u8 (const char *pch)
{
    return QString::fromUtf8 (pch);
}


#endif // TOOLS_H
