#ifndef FARMERSGROUPSPRICES_H
#define FARMERSGROUPSPRICES_H

#include <QString>

struct FGPRecord {
    int start_level;
    int farm_income;
    int safe_limit;
    int price_pickaxe;
    int price_goggles;
    int price_helm;
};

extern FGPRecord FarmersGroupsPrices[];

extern const FGPRecord& getFGPRecord(int level);

extern QString toString(const FGPRecord& fgpr);

#endif // FARMERSGROUPSPRICES_H
