#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <QImage>

/*
// гистограмма
class Hist {
    Hist();
    Hist(int minValue, int maxValue, int num_bins);
    ~Hist();

    setup(int minValue, int maxValue, int num_bins);
    void start();
    void add(int bin);
    void stop();
    double getBin(int bin) const;
    int centilleBin() const;

protected:

};
*/


QImage binarize(const QImage& img);
double compare(const QImage& a, const QImage& b);

// распознавание текста на картинке
class Perceptron {

public:

    Perceptron();

    virtual ~Perceptron();

    // загружаем библиотеку шаблонов из ресурсов
    bool init();
    bool deinit();

    // распознаём и перевести в число.
    // если получилось - вернём число, если нет - вернём "-1"
    // дополнительно выставим *ok
    int parseTemperature(const QImage& img, bool *ok);

protected:

    QImage **digits;

};

#endif // PERCEPTRON_H
