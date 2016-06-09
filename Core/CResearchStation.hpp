#pragma once
#include "CBoardItem.hpp"
#include <QSet>
class CCity;

class CResearchStation : public CBoardItem {
	Q_OBJECT

public:
    static CResearchStation* createIn(CCity*);
	CResearchStation(QWidget * parent = (QWidget*)defaultContainer);
	~CResearchStation();
    QSet<CCity*> citiesWithStation() const;
    QString createToolTip();
    void moveTo(CCity * newLocation);
    void onRightBtnUp(QMouseEvent * event);
private:
	static QSet<CResearchStation*> existingStations;
    const int maxNumberOfStations = 6;
    CCity* location;
};
Q_DECLARE_METATYPE(CResearchStation)