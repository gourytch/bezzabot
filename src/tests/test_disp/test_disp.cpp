#include <iostream>
#include <QString>
#include "tools/producer.h"

using namespace std;

class Base {
protected:
    QString _data;
public:
    explicit Base(const QString& data);
    virtual ~Base();
    virtual QString toString() const;
    static bool fit(const QString& data);
};

class Class_A : public Base {
public:
    explicit Class_A(const QString& data);
    virtual ~Class_A();
    virtual QString toString() const;
    static bool fit(const QString& data);
};

class Class_B : public Base {
public:
    explicit Class_B(const QString& data);
    virtual ~Class_B();
    virtual QString toString() const;
    static bool fit(const QString& data);
};

class Class_A2 : public Class_A {
public:
    explicit Class_A2(const QString& data);
    virtual ~Class_A2();
    virtual QString toString() const;
    static bool fit(const QString& data);
};

class Class_B2 : public Class_B {
public:
    explicit Class_B2(const QString& data);
    virtual ~Class_B2();
    virtual QString toString() const;
    static bool fit(const QString& data);
};

//// Base
/////////////////////////////////////////////////////////////

Base::Base(const QString& data) {
    _data = data;
}

Base::~Base() {

}

QString Base::toString() const {
    return "Base(" + _data + ")";
}

bool Base::fit(const QString& data) {
    return data == "Base";
}

//// Class_A
/////////////////////////////////////////////////////////////

Class_A::Class_A(const QString& data) : Base(data){
}

Class_A::~Class_A() {

}

QString Class_A::toString() const {
    return "Class_A(" + _data + ")";
}

bool Class_A::fit(const QString& data) {
    return data == "Class_A";
}

//// Class_B
/////////////////////////////////////////////////////////////

Class_B::Class_B(const QString& data) : Base(data){
}

Class_B::~Class_B() {

}

QString Class_B::toString() const {
    return "Class_B(" + _data + ")";
}

bool Class_B::fit(const QString& data) {
    return data == "Class_B";
}

//// Class_A2
/////////////////////////////////////////////////////////////

Class_A2::Class_A2(const QString& data) : Class_A(data){
}

Class_A2::~Class_A2() {

}

QString Class_A2::toString() const {
    return "Class_A2(" + _data + ")";
}

bool Class_A2::fit(const QString& data) {
    return data == "Class_A2";
}


//// Class_B2
/////////////////////////////////////////////////////////////

Class_B2::Class_B2(const QString& data) : Class_B(data){
}

Class_B2::~Class_B2() {

}

QString Class_B2::toString() const {
    return "Class_B2(" + _data + ")";
}

bool Class_B2::fit(const QString& data) {
    return data == "Class_B2";
}

////////////////////////////////////////////////////////////////////////////
typedef Producer<Base, QString> BaseProducer;
BaseProducer baseProducer;

void regAll () {
    #define REGISTER(TYPE) baseProcucer.registerClass (Entry(TYPE::fit, New<TYPE>))
    baseProducer.registerClass<Class_B2>();
    baseProducer.registerClass<Class_A2>();
    baseProducer.registerClass<Class_B>();
    baseProducer.registerClass<Class_A>();
    baseProducer.registerClass<Base>();
}

void test (const char *txt) {
    BaseProducer::DataType d(txt);
    BaseProducer::BaseType *p = baseProducer.getObj(d);
    cout << "{" << txt << "} produces ";
    if (p) {
        cout << qPrintable(p->toString());
    } else {
        cout << "NULL";
    }
    cout << endl;
    if (p) {
        delete p;
        p = NULL;
    }
}

int main (...) {
    regAll();
    test("Class_B");
    test("Class_B2");
    test("Class_A");
    test("Class_A2");
    test("Base");
    test("Zero");
    return 0;
}
