#include "CPawn.hpp"
#include "CEnumOption.h"
#include "CCity.hpp"
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QMouseEvent>
#include "core.h"
#include "CBoard.hpp"
#include "CBoardItemsFactory.h"

CPawn::CPawn(QWidget *parent)
    : CBoardItem(parent), position(nullptr), role(ROLE_RANDOM)
{
    disableResize();
    if (isDesigner()) {
        connect(this, &CPawn::rightButtonUp, this, &CPawn::onRightBtnUp);
        options += new CEnumOption<PlayerRole>("Pawn's Role", PlayerRole_SL, role);
    }

}

CPawn::~CPawn()
{

}

CPawn * CPawn::createIn(CCity * location, PlayerRole role)
{
    CPawn* newPawn = dynamic_cast<CPawn*>(CBoardItemsFactory().create("CPawn", location->getZoom(), location->pos()));
    newPawn->role = role;
    newPawn->updateOptions();
    newPawn->setStandardMiddle(location->getStandardMiddle());
    newPawn->moveTo(location);
    location->getContainer()->addItem(newPawn);
    return newPawn;
}

void CPawn::updateOptions()
{
    QString src = getPath(role);
    QPixmap img = QPixmap(src);
    setImage(img);
    setMask(img.createMaskFromColor(Qt::transparent));
}

QString CPawn::getPath(PlayerRole role) const
{
    //:/pawns/img/dispatcherPawn.png
    return QString(":/pawns/img/")+PlayerRole_SL[role].toLower()+"Pawn.png";
}

void CPawn::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    role = (PlayerRole)PlayerRole_SL.indexOf(ts.readLine());
    int posId = ts.readLine().toInt();
    CCity *location = dynamic_cast<CCity*>(container->findItemById(posId));
    moveTo(location);
    updateOptions();
}

void CPawn::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << PlayerRole_SL[role] << endl;
    ts << (position ? position->getId() : -1) << endl;
}

void CPawn::moveTo(CCity * dest)
{
    raise();
    if (dest == nullptr)
        return;
    if (position != nullptr)
        position->pawnEscapes();
    position = dest;
    int nth = position->pawnEnters();
    CPoint newPos = position->getStandardMiddle() + CPoint(17,40) + nth*CPoint(0, -12);
    setStandardMiddleAnim(newPos);
    //QPropertyAnimation *animation = createPropertyAnimation(this, "middlePos", getStandardMiddle(), newPos, 2000, QEasingCurve::InOutQuart);
    //animation->start();
}

void CPawn::onRightBtnUp(QMouseEvent *event)
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
        //pathSelected->unselect();
        moveTo(pathSelected);
    }
}

PlayerRole CPawn::getRole() const
{
    return role;
}

QString CPawn::createObjectName() const
{
    return QString("CPawn %1").arg(PlayerRole_SL[role]);
}

QString CPawn::createObjectName(PlayerRole role)
{
    return QString("CPawn %1").arg(PlayerRole_SL[role]);
}

CCity * CPawn::getPosition() const
{
    return position;
}
