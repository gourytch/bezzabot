#include <QString>
#include <QUrl>
#include <QDateTime>
#include <QTime>
#include "tools/tools.h"
#include "parsers/all_pages.h"
#include "bot.h"

void Bot::one_step () {
    if (_page == NULL || (_gpage == NULL && _page->pagekind != page_Login) ) {
        qCritical("we're not at game page. [re]login");
        GoTo(_baseurl);
        return;
    }
    QDateTime ts = QDateTime::currentDateTime();

    return;
}
