#include "CPathMarker.hpp"
#include <QMessageBox>
#include <QMouseEvent>
#include "CStringOption.h"
#include <QPropertyAnimation>

CPathMarker::CPathMarker(QWidget *parent)
    : CBoardItem(parent)
{
    connect(this, &CPathMarker::rightButtonUp, this, &CPathMarker::onRightBtnUp);
    options += new CStringOption("Image src", imgSrc);
}

CPathMarker::~CPathMarker()
{

}

void CPathMarker::loadFrom(QTextStream & ts)
{
    CBoardItem::loadFrom(ts);
    int pos = ts.readLine().toInt();
    references[(CBoardItem**)&position] = pos;
    imgSrc = ts.readLine();
    setObjectName(name);
    QPixmap pixmap(imgSrc);
    setImage(pixmap);
    setMask(pixmap.createMaskFromColor(Qt::transparent));
}

void CPathMarker::saveTo(QTextStream & ts) const
{
    CBoardItem::saveTo(ts);
    ts << (position == nullptr ? -1 : position->getId()) << endl;
    ts << imgSrc << endl;
}

void CPathMarker::moveToNext(int count)
{
    if (position == nullptr)
        throw "The marker has no position";
    for (int i = 0; i < count && position->getNext() != nullptr; ++i)
        position = position->getNext();
    if (position != nullptr)
        moveTo(position);
}

void CPathMarker::moveTo(CPathItem * dest)
{
    if (dest == nullptr)
        return;
    position = dest;
    QPropertyAnimation *animation = createPropertyAnimation(this, "middlePos", getStandardMiddle(), position->getStandardMiddle(), 2000, QEasingCurve::InOutQuad);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void CPathMarker::onRightBtnUp(QMouseEvent *event)
{
    CBoardItem *selected = getSelected();
    if (selected == nullptr)
        moveToNext();
    else {
        CPathItem *pathSelected = dynamic_cast<CPathItem*>(selected);
        if (pathSelected == nullptr) {
            QMessageBox::critical(nullptr, "Invalid action", QString("Error - you can move CPathMarker object only to CPathItem object (tried to move to ") + selected->metaObject()->className() + " object)");
            return;
        }
        pathSelected->unselect();
        moveTo(pathSelected);
    }
}

void CPathMarker::updateOptions()
{
    if (imgSrc != oldImgSrc) {
        oldImgSrc = imgSrc;
        QPixmap img = QPixmap(imgSrc);
        setImage(img);
        setMask(img.createMaskFromColor(Qt::transparent));
    }
}
