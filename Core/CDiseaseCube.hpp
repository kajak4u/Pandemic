#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
#include "CCity.hpp"

class CDiseaseCube : public CBoardItem {
	Q_OBJECT
    Q_PROPERTY(double alpha MEMBER alpha WRITE setAlpha)
public:
    static CDiseaseCube* createIn(CCity*, DiseaseType color);
	CDiseaseCube(DiseaseType _color=UNKNOWN, CCity *city=nullptr);
	~CDiseaseCube();
    DiseaseType getColor() const;
    //void scale(double factor);
    void setDestination(CCity *newLocation);
    bool isFirst() const;
    void setAlpha(double newAlpha);
private:
    static QMap<DiseaseType, QPixmap> pixmaps;
    double alpha;
    void updateOptions();
    DiseaseType color;
    CCity *location;
    virtual QString createToolTip();
    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;
    static CPoint offsetOf(DiseaseType);
    static const QPixmap& pixmapOf(DiseaseType);
    //QString whereStr;
    int nth;
};
Q_DECLARE_METATYPE(CDiseaseCube)