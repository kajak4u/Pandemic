#include "designer.hpp"
#include <QtWidgets/QApplication>
#include "CCity.hpp"
#include "CCureMarker.hpp"
#include "CPathItem.hpp"
#include "CPawn.hpp"
#include "CDiseaseCube.hpp"
#include "CPathMarker.hpp"
#include "CResearchStation.hpp"
#include "core.h"
#include <QResource>

int main(int argc, char *argv[])
{
    QResource::registerResource("cards.rcc");
    QResource::registerResource("pandemic.rcc");
    registerMetaTypes();
    QApplication a(argc, argv);
    Designer w;
    if (w.errorOccured())
        return 1;
    w.showFullScreen();
    qApp->setStyleSheet("CBoardItem {/*border: 1px outset black;*/ background-color: rgba(255,255,255,63);}\n"
        "CBoardItem[selected=true] {/*border-color: green;*/ background-color: rgba(127, 192, 192, 191);}"
        "CBoardItem[highlighted=true] {background-color: rgba(100,50,200,127);}"
        "CPathItem {background-color: none;/*rgba(100,50,200,127);*/}"
        "CPawn, CDiseaseCube, CCureMarker {border: none;}"
        "CCureMarker {background-color: none;}"
    );
    return a.exec();
}
