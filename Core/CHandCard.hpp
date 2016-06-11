#pragma once
#include "CExtendedSignalWidget.hpp"

class CCard;

class CHandCard : public CExtendedSignalWidget {
    Q_OBJECT
public:
	CHandCard(CCard* origin, QWidget * parent = 0);
	~CHandCard();

private:
    CCard* origin;
public slots:
    void activate();
signals:
    void cardActivated(CCard*);
};
