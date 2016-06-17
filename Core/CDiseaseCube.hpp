#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
#include "CCity.hpp"

class CDiseaseCube : public CBoardItem {
	Q_OBJECT
public:
	CDiseaseCube(DiseaseType _color=UNKNOWN, CCity *city=nullptr);
	~CDiseaseCube();
    static CDiseaseCube* createIn(CCity*, DiseaseType color);
    void disappear();
    DiseaseType getColor() const;
    void setDestination(CCity *newLocation);
private:
    static QMap<DiseaseType, QPixmap> pixmaps;
    void updateOptions();
    DiseaseType color;
    CCity *location;
    virtual QString createToolTip();
    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;
    static CPoint offsetOf(DiseaseType);
    static const QPixmap& pixmapOf(DiseaseType);
};
Q_DECLARE_METATYPE(CDiseaseCube)