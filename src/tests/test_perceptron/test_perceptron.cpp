//#include <QCoreApplication>
//#include "perceptron.h"
#include <QImage>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <QVector>
//#include <QDebug>

using namespace std;

namespace { // чтобы не видно было снаружи


const int cell_w    = 8;
const int cell_h    = 10;
const int gap_w     = 1;
const int drift_x   = 2;
const int drift_y   = 1;

const double min_confidence = 0.5;
const double min_mul_confidence = 0.01;

QImage **digits = NULL;
QImage bigpix;

/*
void dump(const char *name, const QImage& img) {
    cout << "image: " << name
         << " " << img.width() << "x" << img.height() << endl;
    for (int y = 0; y < min(15, img.height()); ++y) {
        for (int x = 0; x < min(40, img.width()); ++x) {
//          cout << hex << setw(2) << qBlue(img.pixel(x, y)) << " ";
            cout << (qBlue(img.pixel(x, y)) ? "X" : ".") << " ";
        }
        cout << endl;
    }
//    cout << setw(0) << dec;
    cout.flush();
}

void dump(const char *name, const QImage& img1, const QImage& img2) {
    cout << "image: " << name
         << " " << img1.width() << "x" << img1.height() << " & "
         << " " << img2.width() << "x" << img2.height()
         << endl;
    for (int y = 0; y < min(15, img1.height()); ++y) {
        for (int x = 0; x < min(40, img1.width()); ++x) {
            cout << (qBlue(img1.pixel(x, y)) ? "X" : ".");
        }
        cout << " | ";
        for (int x = 0; x < min(40, img2.width()); ++x) {
            cout << (qBlue(img2.pixel(x, y)) ? "X" : ".");
        }
        cout << endl;
    }
//    cout << setw(0) << dec;
    cout.flush();
}

*/

void init() {
    bigpix.load(":/digits.png");
    Q_ASSERT(!bigpix.isNull());
    Q_ASSERT(bigpix.width() == 10 * cell_w);
    Q_ASSERT(bigpix.height() == cell_h);
    digits = new QImage*[10];
    for (int i = 0; i < 10; ++i) {
        digits[i] = new QImage(bigpix.copy(i * cell_w, 0, cell_w, cell_h));
    }
}


void deinit() {
    for (int i = 0; i < 10; ++i) {
        delete digits[i];
    }
    delete[] digits;
}


QImage& getDigit(int ix) {
    return *(digits[ix]);
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
//    qDebug() << "findROI. img.size=" << img.size();
    int top = findTopY(img, h);
 //   qDebug() << "top = " << top;
    if (top == -1) return QRect();
    int left = findLeftPixel(img, top, h);
 //   qDebug() << "left = " << left;
    int right = findRightPixel(img, top, h);
//    qDebug() << "right = " << right;
    if (right < left) return QRect();
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


void guessDigit(const QImage& a, int& digit, double& confidence) {
    digit = -1;
    confidence = -1.0;
    for (int i = 0; i <= 9; ++i) {
        double v = compare(a, getDigit(i));
        if (confidence < v) {
//            cout << "new candidate: digit=" << i
//                 << ", confidence " << v << " (vs old) " << confidence << endl;
            confidence = v;
            digit = i;
        }
    }
//    cout << ": " << endl << ": "
//         << "guessed digit " << digit
//         << " confidence " << confidence << endl
//         << ":" << endl;
}


void findDigit(const QImage& a, QRect drift,
                QPoint& home, int& digit, double& confidence) {
//    cout << "findDigit drift=" << drift.width() << "x" << drift.height()
//         << "+" << drift.left() << "+" << drift.top()
//         << endl;
    digit = -1;
    confidence = 0.0;

    for (int y = drift.top(); y <= drift.bottom(); ++y) {
        for (int x = drift.left(); x <= drift.right(); ++x) {
//            cout << "look at " << x << ";" << y << endl;
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
//    cout << "sample #" << digit
//         << ", pos " << home.x() << ";" << home.y()
//         << ", confidence " << confidence << endl;
}


void findNumber(const QImage& a, QRect drift,
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
//    cout << "found number " << number
//         << ", confidence " << confidence << endl;
}


void guessNumber(const QImage& a, int offsX, int offsY,
                 int& number, double& confidence) {
    number = 0;
    confidence = 0.0;

    int n = 0;
    for (;;) {
        QRect r(offsX + n * (cell_w + gap_w), offsY, cell_w, cell_h);
        QImage d = a.copy(r);
        int digit;
        double d_conf;
        guessDigit(d, digit, d_conf);
        if (d_conf < min_confidence) break;
        number = number * 10 + digit;
        confidence *= d_conf;
        ++n;
        if (n > 4) break;
    }
//    qDebug() << "guessed number " << number << " confidence" << confidence;
}


void guessBestNumber(const QImage& a, QRect drift, int& number, double& confidence) {
    number = 0;
    confidence = 0.0;
    int best_x = 0;
    int best_y = 0;

    for (int dy = 0; dy <= drift.height(); ++dy) {
        for (int dx = 0; dx <= drift.width(); ++dx) {
            int left = drift.left() + dx;
            int top = drift.top() + dy;
//            cout << endl << endl
//                 << "#" << endl
//                 << "# drift " << dx << ";" << dy << endl
//                 << "# home " << left << ";" << top << endl;
//            dump("# pix", a.copy(left, top, a.width() - left, a.height() - top));
            cout << endl;

            int n;
            double c;
            guessNumber(a, left, top, n, c);
            if (confidence < c) {
                confidence = c;
                number = n;
                best_x = left;
                best_y = top;
            }
        }
    }
//    cout << "best guessed number " << number << " confidence " << confidence << endl;
//    cout << "best pos=" << best_x << ";" << best_y << endl;
//    dump("best pix", a.copy(best_x, best_y, a.width() - best_x, a.height() - best_y));
}


int percept(const QImage& img, double *confidence) {
    QImage bw = binarize(img);
//    dump("bw", bw);

    QRect roi = findROI(bw, cell_h);

    if (roi.isEmpty()) {
        if (confidence) *confidence = 0.0;
        return -1;
    }
//    dump("by roi", bw.copy(roi));
    int n;
    double c;
    QRect drift;
    drift.setLeft(max(0, roi.left() - cell_w / 2));
    drift.setRight(min(bw.width() - cell_w, roi.left() + cell_w / 2));
//    drift.setTop(max(0, roi.top() - 1));
//    drift.setBottom(min(bw.height() - cell_h, roi.top() + 1));
    drift.setTop(roi.top());
    drift.setBottom(roi.top());
//    guessBestNumber(bw, drift, n, c);
    QPoint h;
    findNumber(bw, drift, h, n, c);
    if (confidence) *confidence = c;
    return n;
}

}// end of anonymous namespace

//////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "use " << argv[0] << "img [...]" << endl;
        return 1;
    }

    init();
//    dump("digit 0", getDigit(0));
//    dump("digit 1", getDigit(1));
//    dump("digit 2", getDigit(2));

    for (int i = 1; i < argc; ++i) {
        string src_name = argv[i];
        cout << "read image from " << src_name << endl;
        QImage src(src_name.c_str());
        int number;
        double confidence;
        number = percept(src, &confidence);
        cout << "number " << number << " with confidence " << confidence << endl;
    }
    cout << "done" << endl;

    deinit();
    return 0;
}
