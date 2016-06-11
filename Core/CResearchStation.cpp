#include "CResearchStation.hpp"
#include "CCity.hpp"
#include <QMessageBox>
#include <QMouseEvent>
#include "CBoardItemsFactory.h"
#include "core.h"
#include "CBoard.hpp"

CResearchStation* CResearchStation::createIn(CCity* destination)
{
    CResearchStation* newStation = dynamic_cast<CResearchStation*>(CBoardItemsFactory().create("CResearchStation", destination->getZoom(), destination->pos()));
    newStation->setDestination(destination);
    newStation->updateOptions();
    newStation->stackUnder(destination);
    return newStation;
}
void CResearchStation::disappear()
{
    CPoint endPos = getStandardMiddle();
    endPos.setY(-standardSize.height());
    QPropertyAnimation *anim = createPropertyAnimation(this, "middlePos", getStandardMiddle(), endPos, 1500, QEasingCurve::OutBounce);
    connect(anim, &QPropertyAnimation::finished, this, &CResearchStation::deleteLater);
    container->addAnimation(anim);
}

CResearchStation::CResearchStation(QWidget * parent) : CBoardItem(parent) {
    disableResize();
    QPixmap pixmap("img\\base.png");
    setImage(pixmap);
    setMask(pixmap.createMaskFromColor(Qt::transparent));
    if(isDesigner())
        connect(this, &CResearchStation::rightButtonUp, this, &CResearchStation::onRightBtnUp);
}

CResearchStation::~CResearchStation() {
}

QString CResearchStation::createToolTip()
{
    QString tooltip = CBoardItem::createToolTip();
    if (location != nullptr)
        tooltip += "\nLocation: " + location->getName();
    return tooltip;
}
void CResearchStation::setDestination(CCity * newLocation)
{
    location = newLocation;
    if (newLocation == nullptr)
        return;
    CPoint cityPoint = newLocation->getStandardPos();
    CPoint stationOffset = { 0,0 };
    CPoint endPos = cityPoint + stationOffset + standardSize / 2;
    setStandardMiddle(CPoint(endPos.x(), endPos.y() % 100));
    CPoint stdm1 = getStandardMiddle();
    QPropertyAnimation *anim = createPropertyAnimation(this, "middlePos", getStandardMiddle(), endPos, 1500, QEasingCurve::OutBounce);
    container->addAnimation(anim);
}

void CResearchStation::onRightBtnUp(QMouseEvent *event)
{
    CBoardItem *selected = getSelected();
    if (selected == nullptr)
        return;
    else {
        CCity *pathSelected = dynamic_cast<CCity*>(selected);
        if (pathSelected == nullptr) {
            QMessageBox::critical(nullptr, "Invalid action", QString("Error - you can move CPawn object only to CCity object (tried to move to ") + selected->metaObject()->className() + " object)");
            return;
        }
        pathSelected->unselect();
        setDestination(pathSelected);
    }
}