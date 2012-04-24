#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include "tools.h"
#include "varmap.h"


VarMap::VarMap(const QString& fname, QObject *parent): QObject(parent) {
    fname_ = fname;
    load();
}


VarMap::~VarMap() {
    sync();
}


void VarMap::clear() {
    if (container_.isEmpty()) return;
    container_.clear();
    dirty_ = true;
}


void VarMap::load() {
    QFile fin(fname_);
    if (!fin.exists())
    {
        qWarning("file " + fname_ + " not exists");
        clear();
        return;
    }

    if (!fin.open(QFile::ReadOnly | QFile::Text))
    {
        qCritical("FILE " + fname_ + " OPEN ERROR FOR READING");
        clear();
        return;
    }
    QTextStream in(&fin);
    QRegExp rx("^([^=]+)=(.*)$");
    int lineNo = 0;

    while (!in.atEnd()) {
        QString buf = in.readLine();
        ++lineNo;
        if (rx.indexIn(buf) == -1) {
            qWarning(u8("varmap[%1]: bad line: {%2}").arg(lineNo).arg(buf));
            continue;
        }
//        container_[unescape(rx.cap(1))] = unescape(rx.cap(2));
        container_[unescape(rx.cap(1))] = QByteArray::fromBase64(rx.cap(2).toAscii());
    }
    fin.close ();
    dirty_ = false;
}

void VarMap::save() {

    QFile fout (fname_);
    if (!fout.open (QFile::WriteOnly | QFile::Truncate))
    {
        qCritical("FILE " + fname_ + " OPEN ERROR FOR WRITING");
        return;
    }
    QTextStream out (&fout);

    Iterator i(container_);
    while (i.hasNext()) {
        i.next();
//        out << escape(i.key()) + "=" + escape(i.value().toString()) + "\n";
        out << escape(i.key()) + "=" + i.value().toByteArray().toBase64() + "\n";
    }
    out.flush ();
    fout.flush();
    fout.close();

    dirty_ = false;
}


void VarMap::sync() {
    if (dirty_) save();
}


void VarMap::set(const QString& key, const QVariant& value) {
    container_[key] = value;
    dirty_ = true;
    sync();
}


bool VarMap::contains(const QString& key) const {
    return container_.contains(key);
}


QVariant VarMap::get(const QString& key, const QVariant& defval) const {
    if (!contains(key)) return defval;
    return container_.value(key);
}
