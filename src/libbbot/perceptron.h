#ifndef PERCEPTRON_H
#define PERCEPTRON_H

#include <QImage>


// распознавание текста на картинке
class Perceptron {

public:

    Perceptron();

    virtual ~Perceptron();

    bool init(); // загружаем библиотеку шаблонов из ресурсов
    void deinit(); // освобождаем ресурсы

    // распознаём и перевести в число.
    // если получилось - вернём число, если нет - вернём "-1"
    // дополнительно выставим коэффициенит уверенности в результате [0..1]
    int parseTemperature(const QImage& img, double *confidence);

public: // разные полезные константы

    static const int cell_w    = 8;
    static const int cell_h    = 10;
    static const int gap_w     = 1;
    static const int drift_x   = 2;
    static const int drift_y   = 1;

    static const double min_confidence = 0.5;
    static const double min_mul_confidence = 0.01;

protected:

    void guessDigit(const QImage& a, int& digit, double& confidence);
    void findDigit(const QImage& a, QRect drift,
                    QPoint& home, int& digit, double& confidence);
    void findNumber(const QImage& a, QRect drift,
                    QPoint& home, int& number, double& confidence);

    QImage bigpix;
    QImage **digits;

};

#endif // PERCEPTRON_H
