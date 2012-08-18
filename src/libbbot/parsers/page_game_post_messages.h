#ifndef PAGE_GAME_POSTBOX_H
#define PAGE_GAME_POSTBOX_H

#include <QList>
#include <QListIterator>
#include "page_game.h"

struct Message {
    int         id;
    QDateTime   pit;
    QWebElement td;

    Message();
    Message(const Message& that);
    bool parse(QWebElement& tr);
    const Message& operator=(const Message& that);
    QString toString() const;
};

class Page_Game_Post_Messages : public Page_Game
{
    Q_OBJECT
public:

    int current_folder;

    int page_index;

    int page_count;

    typedef QList<Message> Messages;

    Messages page_messages;

public:

    explicit Page_Game_Post_Messages(QWebElement& doc);

    static bool fit(const QWebElement& doc);

    virtual QString toString (const QString& pfx = QString ()) const;

    void parse();

    bool doSelectFolder(int number);

    bool doSelectPage(int number);

    bool doSelectMessage(int id);

    bool doUnselectMessage(int id);

    bool doRemoveSelectedMessages();

    bool doRemoveAllMessages();

};

#endif // PAGE_GAME_POSTBOX_H
