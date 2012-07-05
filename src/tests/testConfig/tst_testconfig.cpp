#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include "tools/config.h"

class TestConfig : public QObject
{
    Q_OBJECT

public:
    TestConfig();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testNoSlash();
    void testWithSlash();
    void testDefVal();
    void testValuesFromDEFAULT();
    void testValuesFromGroup();
    void testValuesFromParentGroup();
    void testTilda();

};

TestConfig::TestConfig()
{
}

void TestConfig::initTestCase()
{
    QString fname = "testconfig.ini";
    QFile f(fname);
    f.open(QFile::WriteOnly);
    f.write(
                "[General]\n"
                "readonly = true\n"
                "initialized = true\n"
                "\n"
                "[SLASHTEST]\n"
                "st_key1 = value1\n"
                "st_key2/st_subkey1 = value2\n"
                "\n"
                "[DEFAULT]\n"
                "k0_in_default = value3\n"
                "k1_in_default = value4\n"
                "k2 = value5\n"
                "\n"
                "[BASE]\n"
                "k2 = value6\n"
                "k3_in_base = value7\n"
                "k4 = value8\n"
                "SUB/k2 = value9\n"
                "SUB/k4 = value10\n"
                "SUB/k5_in_sub = value11\n"
                "\n"
                "[BASE/SUB]\n"
                "k2 = value12\n"
                "k6_in_sub = value13\n"
                "[TILDATEST]\n"
                "var = value_TILDATEST/var\n"
                "var~foo = value_TILDATEST/var~foo\n"
                "var~bar = value_TILDATEST/var~bar\n"
                );
    f.close();

    Config::setFName(fname);
    (void)Config::global();
}

void TestConfig::cleanupTestCase()
{
}


void TestConfig::testNoSlash() {
    Config& global = Config::global();
    QVERIFY(global.get("SLASHTEST/st_key1") == "value1");
    QVERIFY(global.get("SLASHTEST/st_key2/st_subkey1") == "value2");
}

void TestConfig::testWithSlash() {
    Config& global = Config::global();
    QVERIFY(global.get("/SLASHTEST/st_key1") == "value1");
    QVERIFY(global.get("/SLASHTEST/st_key2/st_subkey1") == "value2");
}

void TestConfig::testDefVal() {
    Config& global = Config::global();
    QVERIFY(global.get("nokey").isNull());
    QVERIFY(global.get("nogroup/nokey").isNull());
    QVERIFY(global.get("nogroup/nosubgroup/nokey").isNull());

    QVERIFY(global.get("nokey", false).isNull());
    QVERIFY(global.get("nogroup/nokey", false).isNull());
    QVERIFY(global.get("nogroup/nosubgroup/nokey", false).isNull());

    QVERIFY(global.get("nokey", true).isNull());
    QVERIFY(global.get("nogroup/nokey", true).isNull());
    QVERIFY(global.get("nogroup/nosubgroup/nokey", true).isNull());

    QVariant defval = "kakabyaka";

    QVERIFY(global.get("nokey", false, defval) == defval);
    QVERIFY(global.get("nogroup/nokey", false, defval) == defval);
    QVERIFY(global.get("nogroup/nosubgroup/nokey", false, defval) == defval);

    QVERIFY(global.get("nokey", true, defval) == defval);
    QVERIFY(global.get("nogroup/nokey", true, defval) == defval);
    QVERIFY(global.get("nogroup/nosubgroup/nokey", true, defval) == defval);

}


void TestConfig::testValuesFromDEFAULT() {
    Config& global = Config::global();
    QVERIFY(global.get("/BASE/k0_in_default") == "value3");
    QVERIFY(global.get("/BASE/SUB/k1_in_default") == "value4");
}

void TestConfig::testValuesFromGroup() {

}

void TestConfig::testValuesFromParentGroup() {

}

void TestConfig::testTilda() {
    Config& global = Config::global();
    QVERIFY(global.get("/TILDATEST/var") == "value_TILDATEST/var");
    QVERIFY(global.get("/TILDATEST/var~foo") == "value_TILDATEST/var~foo");
    QVERIFY(global.get("/TILDATEST/var~bar") == "value_TILDATEST/var~bar");
    QVERIFY(global.get("/TILDATEST/var~baz") == "value_TILDATEST/var");
    QVERIFY(global.get("/TILDATEST/k2") == "value5");
    QVERIFY(global.get("/TILDATEST/k2~foo") == "value5");
}

QTEST_MAIN(TestConfig);

#include "tst_testconfig.moc"
