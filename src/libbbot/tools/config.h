#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QDateTime>
#include "varmap.h"


#define ORG_DOMAIN  "zhukoid.net"
#define ORG_NAME    "zhukoid"
#define APP_NAME    "bezzabot"
#define APP_VERSION "0.0.1"

#define INI_NAME "bezzabot.ini"


class Config : public QObject
{
    Q_OBJECT

protected:

    static Config*      _global;
    static QSettings*   _settings;
    static VarMap*      _overrides;

    static bool     _portable;
    static QString  _app_name;
    static QString  _location_appdir;
    static QString  _location_config;
    static QString  _location_cache;
    static QString  _location_data;
    static QString  _ini_fname;
    static bool     _readonly;

    static QDateTime _uptime0;

    Config*     _base;
    QString     _id;

    QString fullpath (const QString& path) const;

    static void init_check ();

    explicit Config ();

    void setTemplate ();

public:

    explicit Config (QObject *parent, const QString& _id, Config *base = 0);

    virtual ~Config ();

    static Config& global ();

    static QDateTime uptime0() {return _uptime0; }


    static QSettings& settings ()
    {
        global ();
        return *_settings;
    }

    QVariant get (
            const QString& path, // in form "group/[subgroup/…]/key[~suffix]"
            bool  strict = false, // true ::= in this config, not in parent one
            const QVariant& defval = QVariant ());

    void set (
            const QString& path,
            const QVariant& value);

    static const QString& appName ();

    static const QString& appDirPath ();

    static const QString& globalConfigPath ();

    static const QString& globalDataPath ();

    static const QString& globalCachePath ();

    QString configPath () const;

    QString dataPath () const;

    QString cachePath () const;

    static void setFName (const QString &fname);

    static QString getFName ();

    static void shutdown();

    static void test ();

};

#endif // CONFIG_H
