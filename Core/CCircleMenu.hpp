#pragma once
#include <QWidget>
#include <QLabel>
#include "CPoint.h"
#include "CExtendedSignalWidget.hpp"

class CCircleMenu : public QWidget {
	Q_OBJECT

public:
	CCircleMenu(QWidget * parent = Q_NULLPTR);
	~CCircleMenu();
    void show(const CPoint& middle);
    template<typename _T> void addOption(const QPixmap& pixmap, const QString& description, _T clickedSlot);
    int countItems() const;
private:
    bool dirty;
    QVector<CExtendedSignalWidget*> items;
    void recalc();
};

template<typename _T>
inline void CCircleMenu::addOption(const QPixmap& pixmap, const QString& description, _T clickedSlot)
{
    dirty = true;
    CExtendedSignalWidget* newItem = new CExtendedSignalWidget(this);
    newItem->setToolTip(description);
    newItem->setPixmap(pixmap);
    newItem->show();
    connect(newItem, &CExtendedSignalWidget::leftButtonUp, clickedSlot);
    items += newItem;
}
