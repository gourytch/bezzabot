#ifndef PRODUCER_H
#define PRODUCER_H

#include <QList>

//
// создатель объекта из данных ТипДанных. по дереву классов
// условия:
//  1. классы должны порождаться от общего предка
//  2. все описания класса должны иметь статический метод bool fit(const ТипДанных&)
//  3. все описания класса должны иметь конструктор от (ТипДанных&)
//
/////////////////////////////////////////////////////////////////////////////

template<class BaseT, class DataT>
class Producer {
public:

    typedef BaseT BaseType;
    typedef DataT DataType;

protected:

    template<class SubType>
    static BaseType* New(DataType& data) {
        return new SubType(data);
    }

    typedef bool      FuncFitT(const DataType& data); // STRICT
    typedef BaseType* FuncNewT(DataType& data);

    struct Entry {
        FuncFitT *Fit;
        FuncNewT *New;

        Entry() {}

        Entry (FuncFitT *_Fit, FuncNewT *_New) :
            Fit (_Fit), New(_New) {}

        Entry (const Entry& that) :
            Fit (that.Fit), New(that.New) {}

        const Entry& operator= (const Entry& that) {
            Fit = that.Fit;
            New = that.New;
            return *this;
        }
    };
    typedef QList<Entry> Entries;
    typedef QListIterator<Entry> EntriesIterator;

    Entries entries;

public:

    template<class SubType>
    void registerClass() {
        entries.append(Entry(SubType::fit, New<SubType>));
    }

    BaseType *getObj(DataType& data) {
        EntriesIterator i (entries);
        while (i.hasNext()) {
            const Entry &e = i.next();
            if (e.Fit(data)) {
                return (e.New(data));
            }
        }
        return NULL;
    }
};

#endif // PRODUCER_H
