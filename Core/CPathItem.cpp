#include "CPathItem.hpp"
#include "CIntOption.h"
#include <QMouseEvent>
#include <QMessageBox>
#include "CBoard.hpp"

std::unique_ptr<QBitmap> CPathItem::pathItemMask = nullptr;

CPathItem::CPathItem(QWidget * parent) : CBoardItem(parent), next(nullptr), highlighted(false), value(-1) {
    connect(this, &CPathItem::rightButtonUp, this, &CPathItem::onRightBtnUp);
    connect(this, &CPathItem::leftButtonUp, this, &CPathItem::onLeftBtnUp);
    options += new CIntOption("item value", value, 0, 10);
    if (pathItemMask == nullptr)
        pathItemMask = std::unique_ptr<QBitmap>(new QBitmap(QPixmap("img/pathItemMask.png").createMaskFromColor(Qt::transparent)));
    setMask(*pathItemMask);
}

CPathItem::~CPathItem() {
	
}

void CPathItem::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    value = ts.readLine().toInt();
    int nextId = ts.readLine().toInt();
    references[(CBoardItem**)&next] = nextId;
    updateOptions();
}

void CPathItem::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << value << endl;
    ts << (next ? next->getId() : -1) << endl;
}


void CPathItem::onLeftBtnUp(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ShiftModifier)
        changeNext(event);
}

void CPathItem::onRightBtnUp(QMouseEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier)
        container->firstPathItem(this);
    else
        toggleSelect();
}

void CPathItem::select()
{
    CBoardItem::select();
    if (next) {
        next->setProperty("highlighted", true);
        next->update();
    }
}

void CPathItem::unselect()
{
    CBoardItem::unselect();
    if (next) {
        next->setProperty("highlighted", false);
        next->update();
    }
}

CPathItem * CPathItem::getNext() const
{
    return next;
}

void CPathItem::setNext(CPathItem * n)
{
    next = n;
}

void CPathItem::changeNext(QMouseEvent * event)
{
    CBoardItem *selected = getSelected();
    if (selected == nullptr) {
        QMessageBox::critical(nullptr, "Invalid action", "Error - no path marker selected");
        return;
    }
    CPathItem *selMarker = dynamic_cast<CPathItem*>(selected);
    if (selMarker == nullptr) {
        QMessageBox::critical(nullptr, "Invalid action", QString("Error - you cannot bind ") + metaObject()->className() + " object with " + selected->metaObject()->className() + " object");
        return;
    }
    if (selMarker->next == this) {
        selMarker->next = nullptr;
        setProperty("highlighted", false);
        return;
    }
    if (selMarker->next != nullptr)
        if (QMessageBox::question(nullptr, "Change next?", QString("Do you really want to change ") + QString::number(selMarker->getId()) + "'s descendand from " + QString::number(selMarker->next->getId()) + " to " + QString::number(getId()) + "?") == QMessageBox::No)
            return;
    if (selMarker->next != nullptr) {
        selMarker->next->setProperty("highlighted", false);
        selMarker->next->update();
    }
    selMarker->next = this;
    setProperty("highlighted", true);
    update();
}
