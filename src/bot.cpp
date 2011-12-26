#include <QDesktopServices>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include "mainwindow.h"
#include "bot.h"
#include "webactor.h"
#include "persistentcookiejar.h"


Bot::Bot(const QString& id, QObject *parent) :
    QObject(parent)
{
    _id = id;
    _config = new Config (this, _id);
    Config::checkDir (_config->dataPath ());
    Config::checkDir (_config->cachePath ());
    qDebug () << "BOT ID : " << _id;
    qDebug () << "BOTDIR : " << _config->dataPath ();
    qDebug () << "CACHE  : " << _config->cachePath ();

    _cookies = new PersistentCookieJar (this);
    _cookies->load ();

    _actor = new WebActor (this);
    _actor->page()->networkAccessManager ()->setCookieJar (_cookies);

    MainWindow *wnd = MainWindow::getInstance();
    connect (_actor->page (), SIGNAL (loadFinished (bool)),
             this, SLOT (onPageFinished (bool)));

    connect (this, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    connect (_actor, SIGNAL (log (const QString &)),
             wnd, SLOT (log (const QString &)));

    _started = false;
    _regular = true;
    _page = NULL;
    _awaiting = false;
    _step_counter = 0;
    _step_timer.setInterval (1000);

    connect (&_step_timer, SIGNAL (timeout()),
             this, SLOT (onStep()));

  //  start();

//    _actor->request (QUrl ("http://g3.botva.ru/"));

}



Bot::~Bot ()
{
    stop ();
//    delete _actor;
//    delete _cookies;
//    delete _config;
}

void Bot::onPageFinished (bool ok)
{
    if (ok)
    {
//        pCookies->debug ();
        _cookies->save ();
    }
    _page = _actor->parse();
    switch (_page->pagekind)
    {
    case page_Login:
        handle_Page_Login();
        break;
    case page_Game_Index:
        handle_Page_Game_Index();
        break;
    case page_Generic:
        handle_Page_Generic();
        break;
    default:
        qDebug()<<"unhandled page with kind=" << toString(_page->pagekind);
        break;
    }
    _awaiting = false;
}


void Bot::start() {
    if (_started) return;
    _started = true;
    emit log(tr("start bot"));
    if (_regular) {
        _step_timer.start();
    } else {
        QTimer::singleShot(1000, this, SLOT(onStep()));
    }
}

void Bot::stop() {
    if (!_started) return;
    _started = false;
    emit log(tr("stop bot"));
    if (_regular) {
        _step_timer.stop();
    }
}

void Bot::step()
{
    if (!_started)
    {
        qDebug() << "not started. skip step";
    }
    if (_regular)
    {
        _step_timer.stop();
    }
    qDebug() << "STEP" << ++_step_counter;
    if (_awaiting) {
        emit log (tr("bot is awaiting for responce. skip"));
    } else {
        emit log (tr("bot is ready for work"));
        if (_actor->busy()) {
            emit log (tr("actor is busy. do nothing"));
        } else {
            emit log (tr("actor is available"));
            if (_page == NULL)
            {
                emit log (tr("shoot request"));
                _awaiting = true;

                _actor->request(QUrl("http://www.google.com/"));
//                QTimer::singleShot(0, _actor, SLOT(request(QUrl("http://www.google.com/"))));

                emit log (tr("puff!"));
//                emit log (tr("no page yet. do login sequence"));
//                if (!action_login()) {
//                    emit log (tr("login failed"));
//                }
            }
        } // end if (actor->busy())
    } // end if (_awaiting)

    if (_started) {
        if (_regular) {
            _step_timer.start();
        } else {
            emit log(tr("bot next shot"));
            QTimer::singleShot(1000, this, SLOT(onStep()));
        }
    }
}

//////////// page handlers //////////////////////////////////////////////////

void Bot::handle_Page_Generic () {
}


void Bot::handle_Page_Login () {
//    if (sequence == Sequence_Login) {
//        qDebug() << "(already in login sequence)";
//        return;
//    }
//    sequence = Sequence_Login;

    qDebug() << "hangle login page";
//
//    if (action_login()) {
//        qDebug() << "ok";
//    } else {
//        qDebug () << "fail";
//    }
//    sequence = Sequence_Undefined;
}


void Bot::handle_Page_Game_Generic () {

}


void Bot::handle_Page_Game_Index () {

}


void Bot::handle_Page_Game_Mine_Open () {

}


void Bot::handle_Page_Game_Farm () {

}

bool Bot::action_login () {
    int server_id       = _config->get("login/server_id", true, -1).toInt();
    QString email       = _config->get("login/email", true, "").toString();
    QString password    = _config->get("login/password", true, "").toString();
    bool good = true;
    if (server_id < 1 || server_id > 3) {
        qDebug() << "missing/bad: login/server_id (" << server_id << ")";
        _config->set("login/server_id", -1111);
        good = false;
    }
    if (email == "") {
        qDebug() << "missing: login/email";
        _config->set("login/email", "Enter@Your.Email.Here");
        good = false;
    }
    if (password == "") {
        qDebug() << "missing: login/password";
        _config->set("login/password", "EnterYourPasswordHere");
        good = false;
    }
    if (!good) {
        qDebug() << "abort login sequence";
        return false;
    }
    qDebug() << "initiate login sequence for "
             << email
             << " at server " << server_id;
    QString sid = QString::number(server_id);
    QUrl url = "http://g" + sid  + ".botva.ru/login.php";

    url = "http://gwi.papillon.ru/GWI/table/table.php?depid=107";

    qDebug() << "retrieve login page";
    _actor->request(url);
    if (!_actor->is_ok()) {
        qDebug() << "page load error";
        return false;
    }

    {
        Page_Generic *page = _actor->parse();
        if (page->pagekind != page_Game_Index) {
            qDebug() << toString(page->pagekind) << " is not login page";
            return false;
        }
        delete page;
    }

    QStringList params;
    params.append("do_cmd");    params.append("login");
    params.append("server");    params.append(sid);
    params.append("email");     params.append(email);
    params.append("password");  params.append(password);
    _actor->request(url, params);
    _actor->wait();
    if (!_actor->is_loaded()) {
        qDebug() << "??? page not retrieved";
        return false;
    }
    if (!_actor->is_ok()) {
        qDebug() << "??? page error";
        return false;
    }
    Page_Generic *page = _actor->parse();
    if (page->pagekind != page_Game_Index) {
        qDebug() << toString(page->pagekind) << " is not index page";
        return false;
    }
    qDebug () << "logged in succesfuly";
    return true;

}

bool Bot::action_look () {
    return true;
}


void Bot::onStep() {
    step();
}

