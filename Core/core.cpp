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
    - dodaj kart� na wierzch
    - wyci�gnij kart� z wierzchu
    - wyci�gnij kart� ze spodu
    - potasuj
    - do�� tali� na wierzch - operator +=(const CDeck&);
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
    - nie ma potrzeby trzymania koloru (to ju� sprawa miasta) - diseases[city.getColor()].addCubes(city);
    **/
    //REGISTER_TYPE(CPlayerCard);
    /**
    CPlayerCard -> karta z talii gracza
    - akcja "u�yj"
    **/
    //REGISTER_TYPE(CEpidemicCard);
    /**
    CEpidemicCard -> karta epidemii
    - akcja "po wej�ciu"
    - po niej dobierz now� kart�
    - mo�na by to zrobi� na zasadzie "po wej�ciu - aktywuj", a dodanie kostek, znacznik�w i losowanie karty zrobi� jako akcj�
    **/
    //REGISTER_TYPE(CSpecialCard);
    /**
    CSpecialCard -> karta specjalna
    - akcja "u�yj" w dowolnym momencie
    **/
}

bool isDesigner() {
    return qApp->property("designer") == true;
}
void setDesigner() {
    qApp->setProperty("designer", true);
}