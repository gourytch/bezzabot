#include <QDebug>
#include <QString>
#include <QtTest>
#include "tools/treemap.h"

class treemapTest : public QObject
{
    Q_OBJECT

public:
    treemapTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testCase1();

    void testDirectoryInit();
    void testDirectoryAddValue();
    void testDirectoryAddDirectory();

    void testCreateTreeMap();
    void testRootExists();
    void testNonExistence();
    void testAdd();
    void testReplace();
    void testRemove();
    void testDefaults();
};

treemapTest::treemapTest()
{
}

void treemapTest::initTestCase()
{
}

void treemapTest::cleanupTestCase()
{
}

void treemapTest::testCase1()
{
    QVERIFY2(true, "Failure");
}


void treemapTest::testDirectoryInit() {
    TreeMap::Directory d;
    QVERIFY(d.getPath() == "/");
    QVERIFY(d.hasDirectory("foo") == false);
    QVERIFY(d.hasValue("foo") == false);
}


void treemapTest::testDirectoryAddValue() {
    TreeMap::Directory d;
    QVERIFY(d.hasValue("foo") == false);
    QVERIFY(d.getValue("foo").isNull());
    d.setValue("foo", 123);
    QVERIFY(d.hasDirectory("foo") == false);
    QVERIFY(d.hasValue("foo") == true);
    QVERIFY(d.getValue("foo") == 123);
    d.setValue("foo", "kaka");
    QVERIFY(d.getValue("foo") == "kaka");
}


void treemapTest::testDirectoryAddDirectory() {
    TreeMap::Directory d;
    QVERIFY(d.hasDirectory("foo") == false);
    QVERIFY(d.getConstDirectory("foo") == NULL);
    TreeMap::Directory *p = d.getDirectory("foo");
    QVERIFY(p != NULL);
    QVERIFY(p->name == "foo");
    QVERIFY(d.hasDirectory("foo") == true);
    QVERIFY(d.getConstDirectory("foo") == p);
    QVERIFY(d.getDirectory("foo") == p);
    QVERIFY(d.hasValue("foo") == false);
    QVERIFY(d.getValue("foo").isNull());
}


void treemapTest::testCreateTreeMap() {
    TreeMap m;
    QVERIFY(m.hasDir("/") == true);
    QVERIFY(m.hasValue("/") == false);
    QVERIFY(m.hasDir("/foo") == false);
    QVERIFY(m.hasValue("/foo") == false);
}


void treemapTest::testRootExists() {
    TreeMap m;
    QVERIFY(m.hasDir("/") == true);
    QVERIFY(m.hasDir("") == true);
    QVERIFY(m.hasDir("////////") == true);
    QVERIFY(m.hasValue("/") == false);
}


void treemapTest::testNonExistence() {
    TreeMap m;
    QVERIFY(m.hasDir("/") == true);
    QVERIFY(m.hasDir("/foo") == false);
    QVERIFY(m.hasValue("/foo") == false);

}


void treemapTest::testAdd() {
    TreeMap m;
    const TreeMap::Directory *p = m.getConstDir("/");
    QVERIFY(p != NULL);
    QVERIFY(m.hasDir("/kaka") == false);
    QVERIFY(m.getConstDir("/kaka") == NULL);
    TreeMap::Directory *q = m.getDir("/kaka");
    QVERIFY(q != NULL);
    QVERIFY(m.hasDir("/kaka") == true);
    QVERIFY(m.getConstDir("/kaka") == q);
    QVERIFY(m.getConstDir("/kaka") == p->getConstDirectory("kaka"));

    m.set("/foo/bar/baz", 123);
    qDebug() << m.toXml();
    QVERIFY(m.hasDir("foo") == true);
    QVERIFY(m.hasDir("/foo") == true);
    QVERIFY(m.hasDir("/foo/") == true);
    QVERIFY(m.hasDir("/foo/bar") == true);
    QVERIFY(m.hasDir("/foo/bar/baz") == false);
    QVERIFY(m.hasValue("/foo") == false);
    QVERIFY(m.hasValue("/foo/bar") == false);
    QVERIFY(m.hasValue("/foo/bar/baz") == true);

}


void treemapTest::testReplace() {

}


void treemapTest::testRemove() {

}
\

void treemapTest::testDefaults() {

}


QTEST_APPLESS_MAIN(treemapTest)

#include "tst_treemaptest.moc"
