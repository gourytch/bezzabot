#include <QStringList>
#include <QStringListIterator>
#include "treemap.h"


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
        directories.insert(name, new Directory(name, this));
    }
    return directories.value(name);
}


const TreeMap::Directory* TreeMap::Directory::getDirectory(const QString &name) const {
    if (!hasDirectory(name)) {
        return NULL;
    }
    return directories.value(name);
}

QVariant TreeMap::Directory::getValue(const QString &name, const QVariant& defval) const {
    return values.value(name, defval);
}


void TreeMap::Directory::setValue(const QString &name, const QVariant& value) {
    values.insert(name, value);
}



TreeMap::TreeMap(QObject *parent) : QObject(parent) {
}


const TreeMap::Directory *TreeMap::getConstDir(const QString& path) const {
    const Directory *p = &root;
    QStringList tokens = path.split('/', QString::SkipEmptyParts);
    for (QStringListIterator i(tokens); i.hasNext();) {
        if (p == NULL) return NULL;
        p = p->getDirectory(i.next());
    }
    return p;
}


TreeMap::Directory *TreeMap::getDir(const QString& path) {
    const Directory *p = getConstDir(path);
    return const_cast<Directory *>(p);
}


void TreeMap::mkdir(const QString& path) {

}


bool TreeMap::hasValue(const QString& path) const {

}


bool TreeMap::hasDir(const QString& path) const {

}


void TreeMap::get(const QString& path, const QVariant& defval) const {

}

void TreeMap::set(const QString& path, const QVariant& value) const {

}
