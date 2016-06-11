#pragma once
#include "CBoardItem.hpp"
#include <QSet>
class CCity;

class CResearchStation : public CBoardItem {
	Q_OBJECT

public:
	CResearchStation(QWidget * parent = (QWidget*)defaultContainer);
	~CResearchStation();
    static CResearchStation* createIn(CCity*);
    void disappear();
    QSet<CCity*> citiesWithStation() const;
    QString createToolTip();
    void setDestination(CCity * newLocation);
    void onRightBtnUp(QMouseEvent * event);
private:
	static QSet<CResearchStation*> existingStations;
    const int maxNumberOfStations = 6;
    CCity* location;
};
Q_DECLARE_METATYPE(CResearchStation)