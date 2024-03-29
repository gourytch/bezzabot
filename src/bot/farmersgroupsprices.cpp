#include "farmersgroupsprices.h"

FGPRecord FarmersGroupsPrices[] = {
    { 1,    40,    960,    320, 80, 120},
    { 3,    80,   1920,    640, 160, 240},
    { 7,   160,   3840,   1280, 320, 480},
    {15,   320,   7680,   2560, 640, 960},
    {20,   480,  11520,   3840, 960, 1440},
    {25,   640,  15360,   5120, 1280, 1920},
    {30,   960,  23040,   7680, 1920, 2880},
    {35,  1280,  30720,  10240, 2560, 3840},
    {40,  1920,  46080,  15360, 3840, 5760},
    {45,  2560,  61440,  20480, 5120, 7680},
    {50,  3520,  84480,  28160, 7040, 10560},
    {55,  4480, 107520,  35840, 8960, 13440},
    {60,  5440, 130560,  43520, 10880, 16320},
    {65,  6720, 161280,  53760, 13440, 20160},
    {70,  8000, 192000,  64000, 16000, 24000},
    {75,  9280, 222720,  74240, 18560, 27840},
    {80, 11200, 268800,  89600, 22400, 33600},
    {85, 13120, 314880, 104960, 26240, 39360},
    {90, 15040, 360960, 120320, 30080, 45120},
    {95, 18000, 432000, 144000, 36000, 54000},
    {100, 999999, 9999999, 9999999, 9999999, 9999999},
};

const FGPRecord& getFGPRecord(int level) {
    if (level < 1) level = 1;
    if (level > 99) level = 99;
    const FGPRecord *p;
    for (p = FarmersGroupsPrices; p->start_level < level; ++p);
    if (level < p->start_level) --p;
    return *p;
}

extern QString toString(const FGPRecord& fgpr) {
    return QString("{start_level:%1, farm_income:%2, safe_limit:%3, "
                   "price_pickaxe:%4, price_goggles:%5, price_helm:%6}")
            .arg(fgpr.start_level).arg(fgpr.farm_income).arg(fgpr.safe_limit)
            .arg(fgpr.price_pickaxe).arg(fgpr.price_goggles).arg(fgpr.price_helm);
}
