#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
#include <QMap>
#include <QTimer>
class QMouseEvent;
class QSequentialAnimationGroup;

class CCureMarker : public CBoardItem {
	Q_OBJECT

public:
	CCureMarker(QWidget * parent = (QWidget*)defaultContainer, DiseaseType _color=UNKNOWN);
	virtual ~CCureMarker();
    virtual void loadFrom(QTextStream & ts);
    void increaseStatus(QMouseEvent*);
    virtual void saveTo(QTextStream & ts) const;
    CureStatus getStatus() const;
    void setStatus(const CureStatus& s);
    void connectIcon(QLabel*);
    virtual QString createObjectName() const;
    static QString createObjectName(DiseaseType color);
private:
    void updateOptions();
    QString imagePath(CureStatus) const;
    QString iconPath(CureStatus) const;
    CureStatus status;
    DiseaseType color;
    QMap<CureStatus, QPixmap> images;
    QSequentialAnimationGroup* animation;
    QLabel *connectedIcon;
};