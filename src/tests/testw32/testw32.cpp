#include <QApplication>
#include <QWebView>


int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QWebView w;
    w.show();
    w.load(QUrl("http://localhost/"));
    return app.exec();
}
