#include "CHandCard.hpp"
#include "enums.h"
#include "CBoard.hpp"
#include "CCity.hpp"
#include "CCard.hpp"
#include <QMouseEvent>

CHandCard::CHandCard(CCard* origin, QWidget * parent) : CExtendedSignalWidget(parent), origin(origin)
{
    DiseaseType type;
    CCity* city = origin->getContainer()->findChild<CCity*>(CCity::createObjectName(origin->getCityName()));
    if (city == nullptr) {
        type = UNKNOWN;
    }
    else {
        type = city->getColor();
    }
    QLabel* item = new QLabel();
    setText(QString("<h2>%1</h2>").arg(origin->getCityName()));
    setObjectName(QString("CardInHand_%1").arg(origin->getCityName()));
    QString fontColor = type == BLACK ? "white" : "black";
    QString backgroundColor = type == UNKNOWN ? "orange; font-style: italic" : DiseaseType_SL[type];
    setStyleSheet(QString("border: 2px outset grey; border-radius: 10px; background-color: %1; color: %2;").arg(backgroundColor).arg(fontColor));
    setToolTip(QString("<img src=\"%1\" width=\"303\"/>").arg(origin->getSrc()));
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setMinimumHeight(70);
    setWordWrap(true);
    connect(this, &CHandCard::leftButtonUp, this, &CHandCard::activate);
}

CHandCard::~CHandCard()
{
}

CardType CHandCard::getType() const
{
    return origin->getType();
}

void CHandCard::activate()
{
    emit cardActivated(origin);
}
