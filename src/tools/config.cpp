#include <QDesktopServices>
#include <QCoreApplication>
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QTextCodec>
#include <QDebug>
#include <QRegExp>
#include "config.h"
#include "tools.h"

//static
Config*     Config::_global     = NULL;
QSettings*  Config::_settings   = NULL;
VarMap*     Config::_overrides  = NULL;

bool        Config::_portable = true;
bool        Config::_readonly = false;

QString     Config::_location_appdir;
QString     Config::_location_config;
QString     Config::_location_cache;
QString     Config::_location_data;
QString     Config::_app_name;
QString     Config::_ini_fname;

QDateTime Config::_uptime0;

//static
void Config::init_check ()
{
    if (Config::_settings) return;

    QCoreApplication* appPtr = QCoreApplication::instance ();
    if (!appPtr)
    {
        throw "UNINITIALIZED APPLICATION";
    }

    _uptime0 = QDateTime::currentDateTime();

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);

    appPtr->setOrganizationName (ORG_NAME);
    appPtr->setOrganizationDomain (ORG_DOMAIN);
    appPtr->setApplicationName (APP_NAME);
    appPtr->setApplicationVersion (APP_VERSION);
    _location_appdir = QDir(appPtr->applicationDirPath ()).absolutePath();
    QFileInfo fi(QCoreApplication::applicationFilePath());
    _app_name = fi.baseName();

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

    QString ini_name = _app_name + ".ini";

    QRegExp rx("config=(.+)");
    foreach (QString s, appPtr->arguments()) {
        if (rx.indexIn(s) != -1) {
            ini_name = rx.cap(1);
            break;
        }
    }

    _ini_fname = ini_name.contains('/')
            ? ini_name
            : _location_config + "/" + ini_name;

    qDebug("using config file {"  + _ini_fname + "}");
    _settings = new QSettings (_ini_fname, QSettings::IniFormat);
    _settings->setIniCodec(codec);

    QString varmap_fname = _ini_fname + ".varmap";

    qDebug("using varmap file {"  + varmap_fname + "}");

    _overrides = new VarMap(varmap_fname);

    _global = new Config ();

    _readonly = _global->get("readonly").toBool();

    if (!_readonly) {
        if (!_global->get("initialized").toBool()) {
            _global->setTemplate();
        }
    }
    _global->set("launched",
                 QDateTime::currentDateTime().
                 toString("yyyy-MM-dd hh:mm:ss"));
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
    if (_overrides->contains(fp)) {
        return _overrides->get(fp);
    }
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
    if (!_readonly) {
        _settings->setValue (fullpath (path), value);
        _settings->sync ();
    } else {
        _overrides->set(fullpath(path), value);
        _overrides->sync();
    }
}

//static
const QString& Config::appName () {
    init_check ();
    return _app_name;
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
