#include "tunedpage.h"

QString TunedPage::userAgentForUrl ( const QUrl & url ) const
{
    url.isEmpty ();
    return USER_AGENT;
}
