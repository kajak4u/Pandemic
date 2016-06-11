#include "core.h"
#include "CBoardItem.hpp"
#include "CCity.hpp"
#include "CCureMarker.hpp"
#include "CPathMarker.hpp"
#include "CPathItem.hpp"
#include "CPawn.hpp"
#include "CDiseaseCube.hpp"
#include "CResearchStation.hpp"
#include "CCard.hpp"
#include "CDeck.hpp"
#include <QApplication>

#define REGISTER_TYPE(type) qRegisterMetaType<type>(#type)

void registerMetaTypes() {
    REGISTER_TYPE(CBoardItem);
    REGISTER_TYPE(CCity);
    REGISTER_TYPE(CCureMarker);
    REGISTER_TYPE(CPathMarker);
    REGISTER_TYPE(CPathItem);
    REGISTER_TYPE(CPawn);
    REGISTER_TYPE(CDiseaseCube);
    REGISTER_TYPE(CResearchStation);
    REGISTER_TYPE(CDeck);
    REGISTER_TYPE(CCard);
}

bool isDesigner() {
    return qApp->property("designer") == true;
}
void setDesigner() {
    qApp->setProperty("designer", true);
}