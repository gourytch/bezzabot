#include <QDesktopServices>
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include "config.h"

using namespace std;

//static
Config*     Config::_global     = NULL;
QSettings*  Config::_settings   = NULL;

bool        Config::_portable = true;

QString     Config::_location_appdir;
QString     Config::_location_config;
QString     Config::_location_cache;
QString     Config::_location_data;
QString     Config::_ini_fname;

//static
void Config::init_check ()
{
    if (Config::_settings) return;

    QCoreApplication* appPtr = QCoreApplication::instance ();
    if (!appPtr)
    {
        throw "UNINITIALIZED APPLICATION";
    }
    appPtr->setOrganizationName (ORG_NAME);
    appPtr->setOrganizationDomain (ORG_DOMAIN);
    appPtr->setApplicationName (APP_NAME);
    appPtr->setApplicationVersion (APP_VERSION);
    _location_appdir = appPtr->applicationDirPath ();

    if (_portable)
    {
        _location_config = _location_appdir;
        _location_data   = _location_appdir + "/data";
        _location_cache  = _location_data + "/cache";
    }
    else
    {
        _location_config = QDesktopServices::storageLocation (
                    QDesktopServices::HomeLocation);
        _location_data  = QDesktopServices::storageLocation (
                    QDesktopServices::DataLocation);
        _location_cache = QDesktopServices::storageLocation (
                    QDesktopServices::CacheLocation);
    }

    checkDir (_location_config);
    checkDir (_location_data);
    checkDir (_location_cache);

    _ini_fname = _location_config + "/" + INI_NAME;
    _settings = new QSettings (_ini_fname, QSettings::IniFormat);
    _global = new Config ();
    if (!_global->get("initialized").toBool()) {
        _global->setTemplate();
    }
    _global->set("launched", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}


Config::Config () : QObject ()
{
    _base = NULL;
    _id = QString ();
}


Config::Config (QObject * parent, const QString& id, Config *base) :
    QObject (parent)
{
    init_check ();
    _base = base ? base : _global;
    _id     = id;
}


Config::~Config ()
{
}

void Config::setTemplate() {
    QString dummybot ("dummybot");
    _global->set("bots/list", dummybot);
    _global->set("connection/use_proxy", false);
    _global->set("connection/proxy_host", "proxy.local.net");
    _global->set("connection/proxy_port", 3128);
    Config botcfg(parent(), dummybot, _global);
    botcfg.set("login/server_id", 0);
    botcfg.set("login/email", "dummybot@dummy-mail.net");
    botcfg.set("login/password", "dummy password for dummy bot");
    _global->set("initialized", true);
}

QString Config::fullpath (const QString& path) const
{
    return _id.isNull () ? path : _id + "/" + path;
}


//static
Config& Config::global ()
{
    init_check ();
    return *_global;
}


QVariant Config::get (
        const QString& path,
        bool  strict,
        const QVariant& defval)
{
    QString fp = fullpath (path);
    if (strict || _settings->contains (fp))
    {
        return _settings->value (fp, defval);
    }
    if (_base)
    {
        return _base->get (path, strict, defval);
    }
    return defval;
}


void Config::set (
        const QString& path,
        const QVariant& value)
{
    _settings->setValue (fullpath (path), value);
    _settings->sync ();
}


//static
bool Config::checkDir (const QString& dirname)
{
    QDir d (dirname);
    if (d.exists ())
    {
        return true;
    }
    return d.mkpath (d.absolutePath ());
}

//static
const QString& Config::appDirPath()
{
    init_check ();
    return _location_appdir;
}

//static
const QString& Config::globalConfigPath ()
{
    init_check ();
    return _location_config;
}


//static
const QString& Config::globalDataPath ()
{
    init_check ();
    return _location_data;
}


//static
const QString& Config::globalCachePath ()
{
    init_check ();
    return _location_cache;
}

QString Config::configPath () const
{
    return QDir (globalConfigPath () + "/" + _id).absolutePath ();
}


QString Config::dataPath () const
{
    return QDir (globalDataPath () + "/" + _id).absolutePath ();
}

QString Config::cachePath () const
{
    return QDir (globalCachePath () + "/" + _id).absolutePath ();
}

//static
void Config::test ()
{
    Config& global = Config::global ();
    Config local (NULL, "local");

    global.set ("foo", "global foo");
    global.set ("bar", "global bar");
    global.set ("baz/buzz1", "global baz/buzz1");
    global.set ("baz/buzz2", "global baz/buzz2");
    global.set ("baz/biz/z1", "global baz/biz/z1");
    global.set ("baz/biz/z2", "global baz/biz/z2");
    global.set ("bar", "global bar");
    local.set ("foo", "local foo");
    local.set ("baz/buzz1", "local baz/buzz1");
    local.set ("baz/biz/z1", "local baz/biz/z1");

    qDebug () << "ini-file : " << _ini_fname ;
    qDebug () << "foo : " << local.get ("foo") ;
    qDebug () << "bar : " << local.get ("bar") ;
    qDebug () << "bar (strict) : " << local.get ("bar", true) ;
    qDebug () << "baz/buzz1 : " << local.get ("baz/buzz1") ;
    qDebug () << "baz/buzz2 : " << local.get ("baz/buzz2") ;
    qDebug () << "baz/buzz2 (strict): " << local.get ("baz/buzz2", true) ;
    qDebug () << "baz/biz/z1 : " << local.get ("baz/biz/z1") ;
    qDebug () << "baz/biz/z2 : " << local.get ("baz/biz/z2") ;
    qDebug () << "baz/biz/z2 (strict): " << local.get ("baz/biz/z2", true) ;
    qDebug () << "zzz/zz/z : " << local.get ("zzz/zz/z") ;
    qDebug () << "zzz/zz/z : (strict)" << local.get ("zzz/zz/z", true) ;

}
