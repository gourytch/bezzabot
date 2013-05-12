#include <QDebug>
#include <algorithm>
#include "perceptron.h"

using namespace std;

QImage red2gray(const QImage& img) {
    QImage dst(img.size(), QImage::Format_RGB888);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            int s =  qRed(img.pixel(x, y));
            dst.setPixel(x, y, qRgb(s, s, s));
        }
    }
    return dst;
}


QImage grayblur(const QImage& img) {
    QImage dst(img.size(), QImage::Format_RGB888);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            int s = 0;
            for (int yi = -1; yi <= +1; ++yi) {
                int yy = max(0, min(img.height() - 1, yi + y));
                for (int xi = -1; xi <= +1; ++xi) {
                    int xx = max(0, min(img.width() - 1, xi + x));
                    //s += qGray(img.pixel(xx, yy));
                    s += qRed(img.pixel(xx, yy));
                    if (xi == 0 && yi == 0) {
                        s += 3 * qRed(img.pixel(xx, yy));
                    }
                }
            }
            s /= 12;
            dst.setPixel(x, y, qRgb(s, s, s));
        }
    }
    return dst;
}


QImage threshold(const QImage& img, int value, bool inverted) {
    int black = 0;
    int white = 1;
    QImage bw(img.size(), QImage::Format_Mono);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            bool lt = qGray(img.pixel(x, y)) < value;
            bw.setPixel(x, y, lt == inverted ? white : black);
        }
    }
    return bw;
}


int findBinarizer(const QImage& img, qreal watermark) {
    // составим гистограмму
    int hst[256];
    for (int i = 0; i < 256; ++i) {
        hst[i] = 0;
    }

//  long int total_v = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            int v = qGray(img.pixel(x, y));
            ++hst[v];
//            total_v += v;
        }
    }
    // ищем точку разделения черное/белое
    int total = img.height() * img.width();
    int topval = total * watermark;
    qDebug() << "total =" << total;

    int acc = 0;
    int color;
    for (color = 0; color < 256; ++color) {
//        qDebug() << "acc =" << acc << ", hst ["<< color << "] =" << hst[color];

        if (topval <= acc) {
            break; // color - найденное значение демаркационного цвета
        }
        acc += hst[color];
    }
//    qDebug() << "found color =" << color;
    return color;
}


QImage binarize(const QImage& img) {
//    QImage gray = grayblur(img);
//    QImage gray = red2gray(img);
//    int color = findBinarizer(gray, 0.05);
    // сделаем теперь инвертированную ч/б картинку
//    return threshold(gray, color, true);
    return threshold(img, 70, true);
}


// по бинарному изображению ищем его границы
QRect findCrop(const QImage& img) {
    QRect r;
    bool virgin = true;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (img.pixel(x, y) == 0) continue;
            if (virgin) {
                r = QRect(x, y, 1, 1);
            } else {
                r.setRight(x);
                r.setBottom(x);
            }

        }
    }
    return r;
}

double compare(const QImage& a, const QImage& b) {
    Q_ASSERT(a.size() == b.size());
    if (a.isNull()) return 1.0;

    unsigned long int equals = 0;
    for (int y = 0; y < a.height(); ++y) {
        for (int x = 0; x < a.width(); ++x) {
            if (a.pixel(x, y) == b.pixel(x,y)) ++equals;
        }
    }
    return ((double)equals) / (a.width() * a.height());
}

/////////////////////////////////////////////////////////////////////////////


Perceptron::Perceptron() : digits(NULL) {
}


Perceptron::~Perceptron() {
    deinit();
}


// загружаем библиотеку шаблонов из ресурсов
bool Perceptron::init() {
    const int w = 8;
    const int h = 10;
    if (digits) return true;
    QImage bigpix(":/digits.png");
    if (bigpix.isNull()) return false;
    Q_ASSERT(bigpix.width() == 10 * w);
    Q_ASSERT(bigpix.height() == h);
    digits = new QImage*[10];
    for (int i = 0; i <= 9; ++i) { // грузим из ресурсов
        digits[i] = new QImage(bigpix.copy(i * w, 0, w, h));
    }
    return true;
}


bool Perceptron::deinit() {
    if (digits) {
        for (int i = 0; i <= 9; ++i) {
            if (digits[i]) {
                delete digits[i];
                digits[i] = NULL;
            }
        }
        delete[] digits;
        digits = NULL;
    }
    return true;
}


/*****************************************************************************

  алгоритм для распознавания текста из символов фиксированной ширины:
  1. преобразуем в b/w прямоугольную битовую карту
  FIXME как лучше всего найти место с текстом?
  2. ищем строку символов в картинке
     (место на картинке по высоте равное высоте знакоместа
     и вмещающее наибольшее количество белых точек)
  3. ищем края текста

     если
  3. разрезаем на символы. признак раздельных символов -
     отсутствие связности
  4. распознаём каждый символ по отдельности:
     для каждого шаблона:
     0. нормализуем размеры изображения под шаблон
     1. вычитаем (absdiff) из изображения символа изображение шаблона
     2. подсчитываем сумму разностей (0 - полное совпадение)
     3. вычисляем процент соответствия как соотношение совпало / всего
     4. вставляем в массив в порядке уменьшения соответствия
        [{similarity, char}...]
     5. выбираем наиболее соответствующий элемент
 5. вставляем составляем из них строку

  ----

  упрощения:
  1. размер знакоместа 8x10, spacing 1px

*****************************************************************************/
int Perceptron::parseTemperature(const QImage& img, bool *ok) {
    QImage bw = binarize(img);
    QRect r = findCrop(bw);
}


