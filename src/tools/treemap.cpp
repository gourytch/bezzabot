#include <QDebug>
#include <QDir>
#include <QStringList>
#include <QStringListIterator>
#include <QMap>
#include <QMapIterator>
#include "treemap.h"
#include "tools.h"

QStringList splitPath(const QString& path) {
    return path.split('/', QString::SkipEmptyParts);
}

QString dirname(const QString& path) {
    QStringList entries = splitPath(path);
    if (entries.count() <= 1) return "/";
    QString r;
    for (int i = 0; i < entries.count() - 1; ++i) {
        r += "/" + entries[i];
    }
    return r;
}

QString basename(const QString& path) {
    QStringList entries = splitPath(path);
    if (entries.isEmpty()) return "";
    return entries.back();
}


TreeMap::Directory::Directory(const QString& _name, Directory *_base) :
    name(_name), base(_base) {
}


TreeMap::Directory::~Directory() {
    for (QMapIterator<QString, Directory*> i(directories);
         i.hasNext(); ) {
        Directory *p = i.next().value();
        if (p) delete p;
    }
}


QString TreeMap::Directory::getPath() const {
    if (base == NULL) return "/";
    return base->getPath() + name + "/";
}


bool TreeMap::Directory::hasValue(const QString &name) const {
    return values.contains(name);
}


bool TreeMap::Directory::hasDirectory(const QString &name) const {
    return directories.contains(name);
}


TreeMap::Directory* TreeMap::Directory::getDirectory(const QString &name) {
    if (!hasDirectory(name)) {
//        qDebug() << "DIR[" << this->name << "] MAKE SUBDIR [" << name << "]";
        directories.insert(name, new Directory(name, this));
    }
    return directories.value(name);
}


const TreeMap::Directory* TreeMap::Directory::getConstDirectory(const QString &name) const {
    if (!hasDirectory(name)) {
        return NULL;
    }
    return directories.value(name);
}

QVariant TreeMap::Directory::getValue(const QString &name, const QVariant& defval) const {
    return values.value(name, defval);
}


void TreeMap::Directory::setValue(const QString &name, const QVariant& value) {
//    if (!values.contains(name)) {
//        qDebug() << "DIR[" << this->name << "] MAKE VALUE [" << name << "]";
//    }
    values.insert(name, value);
}


QString TreeMap::Directory::toXml() const {
    QString xml = QString("<directory name='%1'>").arg(name);
    if (directories.isEmpty()) {
        xml += "<directories />";
    } else {
        xml += "<directories>";
        for (QMapIterator<QString, Directory*> i(directories); i.hasNext(); ) {
            xml += i.next().value()->toXml();
        }
        xml += "</directories>";
    }

    if (values.isEmpty()) {
        xml += "<values />";
    } else {
        xml += "<values>";
        for (QMapIterator<QString, QVariant> i(values); i.hasNext(); ) {
            i.next();
            xml += QString("<value name='%1'>%2</value>")
                .arg(i.key(), i.value().toString());
        }
        xml += "</values>";
    }
    return xml + "</directory>";
}


QString TreeMap::Directory::saveSection() const {
    QString txt = "";
    if (!values.isEmpty()) {
        txt.append(QString("[%1]\n").arg(getPath()));
        for (QMapIterator<QString, QVariant> i(values); i.hasNext(); ) {
            i.next();
            txt.append(QString("%1 = %2\n").arg(i.key(), i.value().toString()));
        }
        txt.append("\n");
    }

    for (QMapIterator<QString, Directory*> i(directories); i.hasNext(); ) {
        i.next();
        txt.append(i.value()->saveSection());
    }

    return txt;
}


TreeMap::TreeMap(QObject *parent) : QObject(parent), root("#") {
}


const TreeMap::Directory *TreeMap::getConstDir(const QString& path) const {
    const Directory *p = &root;
    QStringList tokens = path.split('/', QString::SkipEmptyParts);
    for (QStringListIterator i(tokens); i.hasNext();) {
        if (p == NULL) return NULL;
        p = p->getConstDirectory(i.next());
    }
    return p;
}


TreeMap::Directory *TreeMap::getDir(const QString& path) {
    Directory *p = &root;
//    qDebug() << "getDir(" << path << ") {";
    QStringList tokens = path.split('/', QString::SkipEmptyParts);
    for (QStringListIterator i(tokens); i.hasNext(); ) {
        QString token = i.next();
//        qDebug() << "  ... token {" << token << "}";
        p = p->getDirectory(token);
    }
//    qDebug() << "} // getDir(" << path << ")";
    return p;
}


void TreeMap::mkdir(const QString& path) {
    (void)getDir(path);
}


bool TreeMap::hasValue(const QString& path) const {
    const Directory *p = getConstDir(dirname(path));
    if (!p) return NULL;
    return p->hasValue(basename(path));
}


bool TreeMap::hasDir(const QString& path) const {
    return (getConstDir(path) != NULL);
}


QVariant TreeMap::get(const QString& path, const QVariant& defval) const {
    const Directory *p = getConstDir(dirname(path));
    if (!p) return defval;
    return p->getValue(basename(path), defval);
}


void TreeMap::set(const QString& path, const QVariant& value) {
    getDir(dirname(path))->setValue(basename(path), value);
}


QString TreeMap::toXml() const {
    return QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?><tree>%1</tree>")
            .arg(root.toXml());
}

void TreeMap::load(const QString& fname) {
}

void TreeMap::save(const QString& fname) {
    QString txt;
    txt.append("# TREE MAP\n");
    txt.append(root.saveSection());
    txt.append("# ### END ### #\n");
    ::save(fname, txt);
}
