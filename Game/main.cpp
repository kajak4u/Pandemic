#include <QApplication>
#include <QResource>
#include "CMainWindow.hpp"
#include "CGameWindow.hpp"
#include <QMessageBox>
#include "core.h"

int main(int argc, char *argv[])
{
    QResource::registerResource("pandemic.rcc");
    QResource::registerResource("cards.rcc");
    registerMetaTypes();
    QApplication a(argc, argv);
#ifdef _DEBUG
    reloadStyle();
    QVector<QPair<QString, PlayerRole>> players;
    players << qMakePair(QString("£ukasz"), ROLE_MEDIC) << qMakePair(QString("Krzysiek"), ROLE_RANDOM);
    CGameWindow* w = new CGameWindow(DIFF_MEDIUM, players);
    w->showFullScreen();
#else
    CMainWindow* w = new CMainWindow(false);
    w->showFullScreen();
#endif
    return a.exec();
}
