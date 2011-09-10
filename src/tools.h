#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

QString now (bool utc=false);
void save (const QString& fname, const QString& text);
QString load (const QString& fname);

#endif // TOOLS_H
