#include "workalchemy.h"

WorkAlchemy::WorkAlchemy(Bot *bot) : Work(bot) {

}


void WorkAlchemy::configure(Config *config) {

}


bool WorkAlchemy::isPrimaryWork() const {
    return false;
}


WorkType WorkAlchemy::getWorkType() const {
    return Work_Alchemy;
}


QString WorkAlchemy::getWorkStage() const {
    return "???";
}

bool WorkAlchemy::nextStep() {
    return false;
}


bool WorkAlchemy::processPage(const Page_Game *gpage) {
    return false;
}


bool WorkAlchemy::processQuery(Query query) {
    return false;
}


bool WorkAlchemy::processCommand(Command command){
    return false;
}
