#include "vartools.h"
#include <QVariant>
#include <QPoint>
#include <QSize>
#include <QVector>

QByteArray marshalling(const QVariant &v)
{
    if (v.type() == QVariant::ByteArray) {
        return QByteArray("ByteArray::")
                .append(v.toByteArray().toBase64().constData());

    }
    switch (v.type()) {

    case QVariant::ByteArray:

    case QVariant::Bool:
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Double:
    case QVariant::Char:
    case QVariant::String:
    case QVariant::KeySequence:
        return v.toByteArray();

    case QVariant::Invalid:
    default:
        return "<invalid>";
    }
}

    /*
    case QVariant::Point:
        return QByteArray("(Point)")
                .append(v.toPoint().x())

    switch (v.type()) {
        case QVariant::Invalid:
            return "@Invalid()";
            break;

        case QVariant::ByteArray: {
            return v.toByteArray();
            result = QLatin1String();
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
            break;
        }

        case QVariant::String:
        case QVariant::LongLong:
        case QVariant::ULongLong:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::Bool:
        case QVariant::Double:
        case QVariant::KeySequence: {
            result = v.toString();
            if (result.startsWith(QLatin1Char('@')))
                result.prepend(QLatin1Char('@'));
            break;
        }
        case QVariant::Rect: {
            QRect r = qvariant_cast<QRect>(v);
            result += QLatin1String("@Rect(");
            result += QString::number(r.x());
            result += QLatin1Char(' ');
            result += QString::number(r.y());
            result += QLatin1Char(' ');
            result += QString::number(r.width());
            result += QLatin1Char(' ');
            result += QString::number(r.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Size: {
            QSize s = qvariant_cast<QSize>(v);
            result += QLatin1String("@Size(");
            result += QString::number(s.width());
            result += QLatin1Char(' ');
            result += QString::number(s.height());
            result += QLatin1Char(')');
            break;
        }
        case QVariant::Point: {
            QPoint p = qvariant_cast<QPoint>(v);
            result += QLatin1String("@Point(");
            result += QString::number(p.x());
            result += QLatin1Char(' ');
            result += QString::number(p.y());
            result += QLatin1Char(')');
            break;
        }

        default: {
            QByteArray a;
            {
                QDataStream s(&a, QIODevice::WriteOnly);
                s.setVersion(QDataStream::Qt_4_0);
                s << v;
            }

            result = QLatin1String("@Variant(");
            result += QString::fromLatin1(a.constData(), a.size());
            result += QLatin1Char(')');
            break;
        }
    }

    return result;
}
*/
