#ifndef CURRENCY_H
#define CURRENCY_H

#include <QString>

enum Currency {
    currency_Gold,
    currency_Crystals,
    currency_Green,
    currency_Fish,
    currency_Atlants,
    currency_Slaves,
    currency_Undefined
};

extern QString toString(Currency v);

#endif // CURRENCY_H
