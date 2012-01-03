#ifndef FARMERSGROUPSPRICES_H
#define FARMERSGROUPSPRICES_H

struct FGPRecord {
    int start_level;
    int farm_income;
    int safe_limit;
    int price_pickaxe;
    int price_goggles;
    int price_helm;
};

extern FGPRecord FarmersGroupdPrices[];

extern const FGPRecord& getFGPRecord(int level);

#endif // FARMERSGROUPSPRICES_H
