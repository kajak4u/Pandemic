#pragma once
#include "CExtendedSignalWidget.hpp"
#include "enums.h"

class CCard;

class CHandCard : public CExtendedSignalWidget {
    Q_OBJECT
public:
	CHandCard(CCard* origin, QWidget * parent = 0);
	~CHandCard();
    CardType getType() const;
private:
    CCard* origin;
public slots:
    void activate();
signals:
    void cardActivated(CCard*);
};
