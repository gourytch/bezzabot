#include <QDebug>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "perceptron.h"

using namespace std;

namespace { // чтобы не видно было снаружи


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


QImage binarize(const QImage& img) {
    return threshold(img, 70, true);
}


int findTopY(const QImage& img, int h) {
    int height = img.height();
    QVector<int> v(height);

    for (int y = 0; y < height; ++y) {
        int n = 0;
        for (int x = 0; x < img.width(); ++x) {
            if (qBlue(img.pixel(x, y))) ++n;
        }
        v[y] = n;
    }

    int ymax = height - h;
    int best_y = -1;
    int best_count = -1;
    for (int y = 0; y < ymax; ++y) {
        int n = 0;
        for (int d = 0; d < h; ++d) {
            n += v[y + d];
        }
        if (best_count < n) {
            best_count = n;
            best_y = y;
        }
    }
    return best_y;
}


int findLeftPixel(const QImage& img, int topy, int h) {
    int w = img.width();
    for (int x = 0; x < w; ++x) {
        for (int y = topy; y < topy + h; ++y) {
            if (qBlue(img.pixel(x, y))) return x;
        }
    }
    return w-1;
}


int findRightPixel(const QImage& img, int topy, int h) {
    int w = img.width();
    for (int x = w - 1; x > 0; --x) {
        for (int y = topy; y < topy + h; ++y) {
            if (qBlue(img.pixel(x, y))) return x;
        }
    }
    return 0;
}


QRect findROI(const QImage& img, int h) {
//    clog << "findROI. img.size=" << img.width() << "x" << img.height() << endl;
    int top = findTopY(img, h);
//    clog << "top = " << top << endl;
    if (top == -1) return QRect();
    int left = findLeftPixel(img, top, h);
//    clog << "left = " << left << endl;
    int right = findRightPixel(img, top, h);
//    clog << "right = " << right << endl;
    if (right < left) {
//        clog << "left < right" << endl;
        return QRect();
    }
    return QRect(left, top, right - left, h);
}


double compare(const QImage& a, const QImage& b) {
    Q_ASSERT(a.size() == b.size());
    if (a.isNull()) return 1.0;

//    dump("compare", a , b);

    unsigned long int A_and_B = 0;
    unsigned long int A_or_B = 0;
    for (int y = 0; y < a.height(); ++y) {
        for (int x = 0; x < a.width(); ++x) {
            bool bA = (qBlue(a.pixel(x, y)) != 0);
            bool bB = (qBlue(b.pixel(x, y)) != 0);
            if (bA && bB) ++A_and_B;
            if (bA || bB) ++A_or_B;
        }
    }
    if (A_or_B == 0) return 1.0; // black & black

    double r = ((double)A_and_B) / A_or_B;
//    cout << "A&B=" << A_and_B << ", A|B=" << A_or_B << endl;
//    cout << "similarity = " << r << endl << endl;
    return r;
}

///////////////////////////////////////////////////////////////////////////

}// end of anonymous namespace


Perceptron::Perceptron() : digits(NULL) {
    init();
}


Perceptron::~Perceptron() {
    deinit();
}


bool Perceptron::init() {
    if (digits) return true;
    bigpix.load(":/digits.png");
    Q_ASSERT(!bigpix.isNull());
    Q_ASSERT(bigpix.width() == 10 * cell_w);
    Q_ASSERT(bigpix.height() == cell_h);
    digits = new QImage*[10];
    for (int i = 0; i < 10; ++i) {
        digits[i] = new QImage(bigpix.copy(i * cell_w, 0, cell_w, cell_h));
    }
    return true;
}


void Perceptron::deinit() {
    if (!digits) return;
    for (int i = 0; i < 10; ++i) {
        delete digits[i];
        digits[i] = NULL;
    }
    delete[] digits;
    digits = NULL;
}


int Perceptron::parseTemperature(const QImage& img, double *confidence) {
    QImage bw = binarize(img);
    QRect roi = findROI(bw, cell_h);

    if (roi.isEmpty()) {
        cout << "empty ROI" << endl;
        if (confidence) *confidence = 0.0;
        return -1;
    }
    int n;
    double c;
    QRect drift;
    drift.setLeft(max(0, roi.left() - cell_w / 2));
    drift.setRight(min(bw.width() - cell_w, roi.left() + cell_w / 2));
    drift.setTop(roi.top() - 2);
    drift.setBottom(roi.top() + 2);
    QPoint h;
    findNumber(bw, drift, h, n, c);
    if (confidence) *confidence = c;
    return n;
}


void Perceptron::guessDigit(const QImage& a,
                int& digit, double& confidence) {
    digit = -1;
    confidence = -1.0;
    for (int i = 0; i <= 9; ++i) {
        double v = compare(a, *(digits[i]));
        if (confidence < v) {
            confidence = v;
            digit = i;
        }
    }
}


void Perceptron::findDigit(const QImage& a, QRect drift,
                QPoint& home, int& digit, double& confidence) {
    digit = -1;
    confidence = 0.0;

    for (int y = drift.top(); y <= drift.bottom(); ++y) {
        for (int x = drift.left(); x <= drift.right(); ++x) {
            QImage cut = a.copy(x, y, cell_w, cell_h);
            int d_digit;
            double d_conf;
            guessDigit(cut, d_digit, d_conf);
            if (confidence < d_conf) {
                digit = d_digit;
                confidence = d_conf;
                home.setX(x);
                home.setY(y);
            }
        }
    }
}


void Perceptron::findNumber(const QImage& a, QRect drift,
                QPoint& home, int& number, double& confidence) {
    confidence = 0.0;

    bool virgin = true;

    for (;;) {
        QPoint d_home;
        int    d_digit;
        double d_conf;
        findDigit(a, drift, d_home, d_digit, d_conf);
        if (d_conf < min_confidence) break; // не нашли символ
        if (virgin) {
            virgin = false;
            home = d_home;
            number = d_digit;
            confidence = d_conf;
            drift = QRect(d_home.x(),
                          d_home.y(),
                          drift_x * 2,
                          drift_y * 2);
        } else {
            confidence *= d_conf;
            number = number * 10 + d_digit;
        }
        d_home.setX(d_home.x() + cell_w + gap_w);
        drift.moveCenter(d_home);
    }
}

