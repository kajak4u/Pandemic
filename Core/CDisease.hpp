#pragma once
#include <QObject>
#include <QSet>
#include "enums.h"

class CDiseaseCube;
class CCity;
class QTextStream;

class CDisease {
public:
	CDisease(DiseaseType _color=UNKNOWN);
	~CDisease();
    void addSingleCube(CCity* location);
    bool healDisease(CCity* location);
    CureStatus getStatus() const;
    void discoverCure();
    int diseaseAppearsIn(CCity *source, int count=1); //return number of outbreaks
    DiseaseType getColor() const;
    void scaleCubes(double factor);
    QSet<CDiseaseCube*> cubesIn(CCity* location);

    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;

private:
    QSet<CDiseaseCube*> cubesInGame;
    static const int maxNumberOfCubes = 240;
    DiseaseType color;
    CureStatus status;
};
