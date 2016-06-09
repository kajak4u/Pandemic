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
    //REGISTER_TYPE(CDeck);
    /**
    CDeck -> talia kart
    - przechowuj karty
    - dodaj kartê na wierzch
    - wyci¹gnij kartê z wierzchu
    - wyci¹gnij kartê ze spodu
    - potasuj
    - do³ó¿ taliê na wierzch - operator +=(const CDeck&);
    - czy pusta?
    **/
    //REGISTER_TYPE(CCard);
    /**
    CCard -> podstawowy typ karty
    - grafika
    - miasto odniesienia
    **/
    //REGISTER_TYPE(CDiseaseCard);
    /**
    CDiseaseCard -> karta choroby
    - nie ma potrzeby trzymania koloru (to ju¿ sprawa miasta) - diseases[city.getColor()].addCubes(city);
    **/
    //REGISTER_TYPE(CPlayerCard);
    /**
    CPlayerCard -> karta z talii gracza
    - akcja "u¿yj"
    **/
    //REGISTER_TYPE(CEpidemicCard);
    /**
    CEpidemicCard -> karta epidemii
    - akcja "po wejœciu"
    - po niej dobierz now¹ kartê
    - mo¿na by to zrobiæ na zasadzie "po wejœciu - aktywuj", a dodanie kostek, znaczników i losowanie karty zrobiæ jako akcjê
    **/
    //REGISTER_TYPE(CSpecialCard);
    /**
    CSpecialCard -> karta specjalna
    - akcja "u¿yj" w dowolnym momencie
    **/
}

bool isDesigner() {
    return qApp->property("designer") == true;
}
void setDesigner() {
    qApp->setProperty("designer", true);
}