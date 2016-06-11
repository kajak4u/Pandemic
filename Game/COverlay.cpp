#include "COverlay.hpp"
#include "CBoardItem.hpp"
#include <QMouseEvent>
#include <QDebug>

COverlay::COverlay(QWidget * parent) : QLabel(parent) {
    setGeometry(0, 0, 1366, 768);
    setStyleSheet("background: rgba(200, 200, 200, 128);");
    
}

COverlay::~COverlay() {
    for (QMetaObject::Connection conn : references)
        disconnect(conn);
}

void COverlay::track(const QSet<CBoardItem*>& items)
{
    for (CBoardItem* item : items) {
        CBoardItem* newItem = new CBoardItem(this);
        newItem->move(mapFromGlobal(item->mapToGlobal(QPoint(0, 0))));
        newItem->resize(item->size());
        dynamic_cast<QWidget*>(newItem)->setMask(item->mask());
        newItem->setStyleSheet("background-color: #0CE");
        newItem->setToolTip(item->getName());
        newItem->show();
        references += connect(item, &CBoardItem::moved, [newItem, item](const QPoint& pt) {
            QPoint pt2 = dynamic_cast<QWidget*>(item->parent())->mapToGlobal(pt);
            newItem->move(dynamic_cast<QWidget*>(newItem->parent())->mapFromGlobal(pt2));
        });
        references += connect(item, &CBoardItem::resized, [newItem, item](const QSize& siz) {
            dynamic_cast<QWidget*>(newItem)->setMask(item->mask());
            newItem->resize(siz);
        });
        references += connect(newItem, &CBoardItem::leftButtonUp, [this, item]() {
            emit userMadeChoice(item);
            deleteLater();
        });
    }
}
