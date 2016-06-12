#include "COverlay.hpp"
#include "CBoardItem.hpp"
#include <QMouseEvent>
#include <QDebug>
#include <QScrollArea>
#include "CBoard.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

COverlay::COverlay(QWidget * parent) : QLabel(parent), deleteOnClick(true), deleteItemOnClick(false) {
    //setGeometry(0, 0, 1366, 768);
    setGeometry(0, 0, parent->width(), parent->height());
    setStyleSheet("COverlay {background: rgba(200, 200, 200, 128);}");
    
}

COverlay::~COverlay()
{
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
        links[newItem] = item;
        connect(item, &CBoardItem::moved, this, &COverlay::itemMoved);
        connect(item, &CBoardItem::resized, this, &COverlay::itemResized);
        connect(newItem, &CBoardItem::leftButtonUp, this, &COverlay::itemClicked);
    }
}

void COverlay::setDescription(const QString &desc)
{
    QLabel* descLabel = new QLabel(desc, this);
    descLabel->setWordWrap(true);
    if (layout() == nullptr) {
        descLabel->setGeometry(geometry().marginsRemoved(QMargins(100,100,100,100)));
    }
    else {
        QVBoxLayout* lay = dynamic_cast<QVBoxLayout*>(layout());
        lay->insertWidget(0, descLabel);
    }
}

void COverlay::displayItems(const QVector<CBoardItem*>& items)
{
    if (layout() == nullptr) {
        QVBoxLayout* overlayLayout = new QVBoxLayout(this);
        overlayLayout->setSpacing(0);
        overlayLayout->setContentsMargins(0, 100, 0, 0);
    }
    QScrollArea* area = new QScrollArea(this);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget* areaWidget = new QWidget();
    areaWidget->setStyleSheet("background: transparent; border: none;");
    area->setStyleSheet("background: transparent; border: none;");
    area->setWidgetResizable(true);
    area->setWidget(areaWidget);
    area->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    area->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QHBoxLayout* areaHorzLayout = new QHBoxLayout(areaWidget);
    areaHorzLayout->setSpacing(0);
    areaHorzLayout->setObjectName(QStringLiteral("areaHorzLayout"));
    areaHorzLayout->setContentsMargins(0, 0, 0, 0);
    areaHorzLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    for (CBoardItem* item : items) {
        CBoardItem* newItem = new CBoardItem(areaWidget);
        links[newItem] = item;
        item->cloneTo(newItem);
        newItem->scaleTo(item->getContainer()->minZoomFactor()*2);
        newItem->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        newItem->setMinimumSize(newItem->size());
        newItem->setMaximumSize(newItem->size());
        areaHorzLayout->addWidget(newItem);
    }
    dynamic_cast<QVBoxLayout*>(layout())->addWidget(area, 1);
}

void COverlay::setDeleteOnClick(bool delOnClick)
{
    deleteOnClick = delOnClick;
}

void COverlay::setItemDeleteOnClick(bool delOnClick)
{
    deleteItemOnClick = delOnClick;
}

void COverlay::letPlayerChoose(int count, bool canCancel)
{
    numberToSelect = count;
    if (count == 1) {
        for (QMap<CBoardItem*, CBoardItem*>::iterator iter = links.begin(); iter != links.end(); ++iter)
            connect(iter.key(), &CBoardItem::leftButtonUp, this, &COverlay::itemClicked);
    }
    else {
        for (QMap<CBoardItem*, CBoardItem*>::iterator iter = links.begin(); iter != links.end(); ++iter)
            connect(iter.key(), &CBoardItem::leftButtonUp, this, &COverlay::itemToggled);
    }
    QWidget* btns = new QWidget(this);
    QHBoxLayout* btnLayout = new QHBoxLayout(btns);
    if (count != 1) {
        performButton = new QPushButton("perform", this);
        btnLayout->addWidget(performButton);
        connect(performButton, &QPushButton::clicked, this, &COverlay::perform);
    }
    btnLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    if (canCancel) {
        cancelButton = new QPushButton("cancel", this);
        connect(cancelButton, &QPushButton::clicked, this, &COverlay::cancel);
        btnLayout->addWidget(cancelButton);
    }
    layout()->addWidget(btns);
}

void COverlay::itemMoved(const QPoint& pt)
{
    CBoardItem* overlayItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* originItem = links[overlayItem];
    QPoint pt2 = dynamic_cast<QWidget*>(originItem->parent())->mapToGlobal(pt);
    overlayItem->move(dynamic_cast<QWidget*>(overlayItem->parent())->mapFromGlobal(pt2));
}

void COverlay::itemResized(const QSize& siz)
{
    CBoardItem* overlayItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* originItem = links[overlayItem];
    dynamic_cast<QWidget*>(overlayItem)->setMask(originItem->mask());
    overlayItem->resize(siz);
}

void COverlay::itemClicked()
{
    CBoardItem* overlayItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* originItem = links[overlayItem];
    emit userChoseOne(originItem);
    if (deleteItemOnClick)
    {
        overlayItem->deleteLater();
    }
    if (deleteOnClick)
        deleteLater();
}

void COverlay::itemToggled()
{
    CBoardItem* overlayItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* originItem = links[overlayItem];
    if (selected.contains(originItem)) {
        selected -= originItem;
        overlayItem->unselect();
        overlayItem->setStyleSheet("");
    }
    else {
        selected += originItem;
        overlayItem->select();
        overlayItem->setStyleSheet("border: 10px solid red;");
    }
    performButton->setEnabled(selected.size() == numberToSelect);
}

void COverlay::perform()
{
    emit userChoseMany(selected);
    deleteLater();
}

void COverlay::cancel()
{
    if (numberToSelect == 1)
        emit userChoseOne(nullptr);
    else
        emit userChoseMany(QSet<CBoardItem*>());
    deleteLater();
}