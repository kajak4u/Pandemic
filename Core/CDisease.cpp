#include "CDisease.hpp"
#include "CDiseaseCube.hpp"
#include "CCity.hpp"
#include "CBoardItemsFactory.h"
#include "CBoard.hpp"
#include <QTextStream>
#include <QMessageBox>

CDisease::CDisease(DiseaseType _color) : color(_color), status(NEW)
{
}

CDisease::~CDisease()
{
}

void CDisease::addSingleCube(CCity * location)
{
    if (status == TREATED)
        return;
    if (location->cubesOf(color) < 3) {
        CDiseaseCube* newCube = dynamic_cast<CDiseaseCube*>(CBoardItemsFactory().create("CDiseaseCube", location->getZoom(), location->pos()));
        //newCube->whereStr = location->getName();
        newCube->color = color;
        newCube->moveTo(location);
        newCube->updateOptions();
        cubesInGame += newCube;
        newCube->stackUnder(location);
        CBoard* board = newCube->container;
        for (int index = DiseaseType_SL.size() - 1; index > (int)color; --index)
            for (CDiseaseCube* cube : board->disease((DiseaseType)index)->cubesIn(location))
                if (cube->nth == 1)
                    newCube->stackUnder(cube);
    }
    else
        QMessageBox::critical(nullptr, "Add too many cubes", "Error - city " + location->getName() + " cannot contain more than 3 cubes");
}

bool CDisease::healDisease(CCity * location)
{
    int count = location->cubesOf(color);
    int numberToRemove = 0;
    if (count == 0)
        return false;
    if (status == NEW) {
        location->decreaseCubes(color);
        numberToRemove = 1;
    }
    else { //status == DISCOVERED
        location->eraseCubes(color);
        numberToRemove = count;
    }
    QSet<CDiseaseCube*> cubesToRemove = cubesIn(location);
    for (int i = 0; i < numberToRemove; ++i) { //usuwamy od tych najwyżej położonych
        --count;
        for(CDiseaseCube* cube : cubesToRemove)
            if (cube->nth == count) {
                cubesInGame -= cube;
                delete cube;
                break;
            }
    }
    if (cubesInGame.count() == 0 && status == DISCOVERED)
        status = TREATED;
    return true;
}

CureStatus CDisease::getStatus() const
{
    return status;
}

void CDisease::discoverCure()
{
    if (status == NEW)
        status = DISCOVERED;
}

//dodawanie kostki, ewentualnie wybuch; zwraca liczbę wybuchów
int CDisease::diseaseAppearsIn(CCity * source, int count)
{
    if (cubesInGame.size() + count > maxNumberOfCubes) {
        QMessageBox::critical(nullptr, "Game over!", DiseaseType_SL[color] + " cubes are exhausted");
        return 0;
    }
    QSet<CCity*> processed = { source };
    QSet<CCity*> toProcess;
    int outbreaks = 0;
    if (source->cubesOf(color) + count <= 3) { //nie będzie outbreaka
        for (int i = 0; i < count; ++i)
            addSingleCube(source);
    }
    else {  //outbreak
        for (int i = source->cubesOf(color); i < 3; ++i)
            addSingleCube(source);
        ++outbreaks;
        for (CCity *neighbour : source->neighbours())
            toProcess += neighbour;
        while (!toProcess.empty()) {
            CCity *actual = *toProcess.begin();
            toProcess -= actual;
            processed += actual;
            if (actual->cubesOf(color) == 3) {
                ++outbreaks;
                for (CCity *neighbour : actual->neighbours())
                    if (!processed.contains(neighbour))
                        toProcess += neighbour;
            }
            else
                addSingleCube(actual);
        }
    }
    return outbreaks;
}

DiseaseType CDisease::getColor() const
{
    return color;
}

void CDisease::scaleCubes(double factor)
{
    for (CDiseaseCube* cube : cubesInGame)
        cube->scale(factor);
}

QSet<CDiseaseCube*> CDisease::cubesIn(CCity * location)
{
    QSet<CDiseaseCube*> result;
    for (CDiseaseCube* cube : cubesInGame)
        if (cube->location == location)
            result += cube;
    return result;
}

void CDisease::loadFrom(QTextStream &is)
{
    QString colorStr = is.readLine();
    color = (DiseaseType)DiseaseType_SL.indexOf(colorStr);
    QString cureStatusStr = is.readLine();
    status = (CureStatus)CureStatus_SL.indexOf(cureStatusStr);
    int cubesSize = is.readLine().toInt();
    CBoardItemsFactory factory;
    CBoard* board = CBoardItemsFactory().getBoard();
    for (int i = 0; i < cubesSize; ++i) {
        //CDiseaseCube *newCube = dynamic_cast<CDiseaseCube*>(factory.create(is));
        //newCube->delayedLoad();
        //int id = newCube->location->getId();
        //delete newCube;
        //cubesInGame += newCube;
        int id;
        is >> id;
        CCity* destination = dynamic_cast<CCity*>(board->findItemById(id));
        if (destination == nullptr) {
            QMessageBox::critical(nullptr, "Load error", "Error - cannot restore "+DiseaseType_SL[color]+" to an unknown city #"+QString::number(id));
        }
        else {
            addSingleCube(destination);
        }
    }
    is.readLine();
}

void CDisease::saveTo(QTextStream &os) const
{
    os << DiseaseType_SL[(int)color] << endl;
    os << CureStatus_SL[(int)status] << endl;
    os << cubesInGame.size() << endl;
    for (const CDiseaseCube* cube : cubesInGame)
        os << cube->location->getId() << " ";
    os << endl;
}

