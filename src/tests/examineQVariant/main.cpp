#include <iostream>
#include <QCoreApplication>
#include <QIODevice>
#include <QDataStream>
#include <QVariant>
#include <QSize>
#include <QSizeF>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QDate>
#include <QTime>
#include <QDateTime>

#include <QSettings>

//QByteArray marshalling

template<typename T>
void dump_(const T& v) {
    QByteArray arr;
    QDataStream stream(&arr, QIODevice::WriteOnly);
    stream << v;
    std::cout << "{" << arr.toBase64().constBegin() << "}" << std::endl;
}

void dump(const QVariant& qv) {
    QByteArray value;
    if (qv.canConvert(QVariant::ByteArray)) {
        value.append("[")
                .append(qv.typeName())
                .append("]")
                .append(qv.toByteArray());
    } else {
        QByteArray arr;
        QDataStream stream(&arr, QIODevice::WriteOnly);
        stream << qv;
        value.append("[BASE64:")
                .append(qv.typeName())
                .append("]")
                .append(arr.toBase64());
    }
    std::cout << value.constBegin() << std::endl;
//    std::cout << "[#" << qv.type() << ":" << qv.typeName()
//              << "] : {" << qv.toByteArray().constData()
//              << "} = " << arr.toHex().constData() << std::endl;

}

void examine() {
    dump(12345);
    dump(3.14);
    dump(3.14159265D);
    dump("Kaka");
    dump(QString("Byaka"));
    dump(QSize(1024, 768));
    dump(QSizeF(1024.5, 768.5));
    dump(QPoint(512, 128));
    dump(QPointF(512.5, 128.5));
    dump(QRect(10, 20, 30, 40));
    dump(QRectF(10, 20, 30, 40));
    dump(QDate(2012, 4, 22));
    dump(QTime(23, 30, 59, 1234));
    dump(QDateTime(QDate(2012, 2, 28), QTime(23, 30, 59, 321)));
    dump_(QPoint(512, 128));
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    examine();
    return 0;

    return a.exec();
}
