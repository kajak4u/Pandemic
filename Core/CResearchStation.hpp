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
    QString createToolTip();
    void setDestination(CCity * newLocation);
    void onRightBtnUp(QMouseEvent * event);
private:
    CCity* location;
};
Q_DECLARE_METATYPE(CResearchStation)