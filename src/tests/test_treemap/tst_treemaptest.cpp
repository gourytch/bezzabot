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

void treemapTest::testCreateTreeMap() {

}


void treemapTest::testRootExists() {

}


void treemapTest::testNonExistence() {

}


void treemapTest::testAdd() {

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
