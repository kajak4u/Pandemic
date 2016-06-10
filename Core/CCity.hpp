#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
#include <QSet>
#include <QMap>
#include <QTextStream>
#include <memory> //std::unique_ptr<_T>
class CDiseaseCube;
class CResearchStation;
class City;

class CCity : public CBoardItem {
	Q_OBJECT

public:
	CCity(QWidget * parent = (QWidget*)defaultContainer);
    CCity(const CCity& other);
	~CCity();

    virtual void select();
    virtual void unselect();
    void toggleHighlight();
    void unload();
    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;
    const QSet<CCity*> neighbours() const;

    int cubesOf(DiseaseType) const;
    void addCube(DiseaseType);
    void removeCube(DiseaseType);
    void buildResearchStation();
    void removeResearchStation();
    int pawnEnters();
    void pawnEscapes();
    virtual void scale(double factor);
    City* toLogic() const;
    void bindLogic(City*);
    virtual QString createObjectName() const;
    static QString createObjectName(const QString& name);
    DiseaseType getColor() const;
private:
    City* logicObj;
    static std::unique_ptr<QBitmap> cityMask;
    void updateOptions();
    void onLeftBtnUp(QMouseEvent*);
    void onRightBtnUp(QMouseEvent*);
    int pawnsInCity;
    DiseaseType type;
    QMap<DiseaseType, QVector<CDiseaseCube*> > diseaseCubes;
    CResearchStation* researchStation;
    QSet<CCity*> connections;
    CBoardItem* bottom;
private slots:
    void showCityMenu(QMouseEvent*);
};

