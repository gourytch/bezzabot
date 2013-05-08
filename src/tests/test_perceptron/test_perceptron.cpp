//#include <QCoreApplication>
#include "perceptron.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "use " << argv[0] << "img [...]" << endl;
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        string src_name = argv[i];
        int last_dot = src_name.find_last_of('.');
        string dst_name = src_name.substr(0, last_dot)
                + "-bw"
                + src_name.substr(last_dot);
        string dst_name2 = src_name.substr(0, last_dot)
                + "-bw2"
                + src_name.substr(last_dot);

        cout << "read image from " << src_name << endl;
        QImage src(src_name.c_str());
        cout << "binarize" << endl;
        QImage dst = binarize(src);
        QImage dst2 = src.convertToFormat(QImage::Format_Mono,
                                          Qt::MonoOnly | Qt::AvoidDither
                                          );
        dst2.invertPixels();
        cout << "save binary image to " << dst_name << endl;
        dst.save(dst_name.c_str());
        cout << "save binary2 image to " << dst_name2 << endl;
        dst2.save(dst_name2.c_str());
        cout << "... saved" << endl;
    }
    cout << "done." << endl;
    return 0;
//    QCoreApplication a(argc, argv);
    
//    return a.exec();
}
