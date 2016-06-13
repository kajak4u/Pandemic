#include "COverlay.hpp"
#include "CBoardItem.hpp"
#include <QMouseEvent>
#include <QDebug>
#include <QScrollArea>
#include "CBoard.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "CPlayer.h"

COverlay::COverlay(QWidget * parent) 
    : QLabel(parent), deleteOnClick(true), deleteItemOnClick(false), cancelButton(nullptr), performButton(nullptr)
{
    setGeometry(0, 0, parent->width(), parent->height());
    setStyleSheet("COverlay {background: rgba(200, 200, 200, 128);}");
    
}

COverlay::~COverlay()
{
}

void COverlay::track(const QSet<CBoardItem*>& items, bool canCancel)
{
    for (CBoardItem* item : items) {
        CBoardItem* newItem = new CBoardItem(this);
        newItem->move(mapFromGlobal(item->mapToGlobal(QPoint(0, 0))));
        newItem->resize(item->size());
        dynamic_cast<QWidget*>(newItem)->setMask(item->mask());
        newItem->setStyleSheet("background-color: rgba(64, 238, 64, 128)");
        newItem->setToolTip(item->getName());
        newItem->show();
        links[newItem] = item;
        links[item] = newItem;
        connect(item, &CBoardItem::moved, this, &COverlay::itemMoved);
        connect(item, &CBoardItem::resized, this, &COverlay::itemResized);
        connect(newItem, &CBoardItem::leftButtonUp, this, &COverlay::itemClicked);
    }
}

void COverlay::track(const QSet<CPlayer*>& players, const QSet<CPlayer*>& enabledPlayers, bool canCancel)
{
    int iks = 100, igrek=100;
    for (CPlayer* player : players) {
        QLabel* ico = player->getIco();
        CBoardItem* newItem = new CBoardItem(dynamic_cast<QWidget*>(ico->parent()));
        newItem->setGeometry(ico->geometry());
        if (enabledPlayers.contains(player)) {
            newItem->setEnabled(true);
            newItem->setStyleSheet("background-color: rgba(64,64,64,64); border: 2px solid black;");
        }
        else {
            newItem->setEnabled(false);
            newItem->setStyleSheet("background-color: rgba(64,64,64,196); border: 2px solid gray;");
        }
        newItem->setEnabled(enabledPlayers.contains(player));
        newItem->setToolTip(PlayerRole_SL[player->getRole()]);
        newItem->show();
        connect(newItem, &CBoardItem::leftButtonUp, [this, newItem, player]() {
            for(CBoardItem* sel : selected)
                sel->setStyleSheet("background-color: rgba(64,64,64,64); border: 2px solid black;");
            selected.clear();
            newItem->setStyleSheet("background-color: rgba(0,255,0,64); border: 3px solid green;");
            selected += newItem;
            emit userChosePlayer(player);
        });
        connect(this, &COverlay::destroyed, newItem, &CBoardItem::deleteLater);
    }
}

void COverlay::clearDisplay()
{
    qDeleteAll(children());
    links.clear();
    QVBoxLayout* mainLayout = dynamic_cast<QVBoxLayout*>(layout());
    if (mainLayout == nullptr)
        return;
    while (mainLayout->itemAt(0))
        delete mainLayout->takeAt(0);
    delete layout();
    setLayout(nullptr);
}

void COverlay::setDescription(const QString &desc)
{
    QLabel* descLabel = new QLabel(desc, this);
    descLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    descLabel->setWordWrap(true);
    if (layout() == nullptr) {
        descLabel->setGeometry(geometry().marginsRemoved(QMargins(100,100,100,100)));
        descLabel->show();
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
    QString color;
    if (layout()->count() % 2 == 1)
        color = "rgba(64, 238, 64, 128)";
    else
        color = "rgba(64, 64, 238, 128)";
    areaWidget->setStyleSheet("background: transparent; border: none;");
    area->setStyleSheet(QString("background: %1; border: none;").arg(color));
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
        newItem->scaleTo(item->getContainer()->minZoomFactor()*1.25);
        newItem->setStyleSheet(QString() + "border: 0px outset black; border-radius: " + QString::number(newItem->width() / 4) + "px; ");
        newItem->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
        newItem->setMinimumSize(newItem->size());
        newItem->setMaximumSize(newItem->size());
        areaHorzLayout->addWidget(newItem);
    }
    dynamic_cast<QVBoxLayout*>(layout())->addWidget(area, 1);
}

void COverlay::setEnabledItems(const QSet<CBoardItem*>& items)
{
    for (QMap<CBoardItem*, CBoardItem*>::iterator iter = links.begin(); iter != links.end(); ++iter)
        iter.key()->setEnabled((items.contains(iter.value())));
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
        performButton = new QPushButton(count==0 ? "Ok" : "perform", this);
        btnLayout->addWidget(performButton);
        connect(performButton, &QPushButton::clicked, this, &COverlay::perform);
        performButton->setEnabled(selected.size() == numberToSelect);
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
    CBoardItem* originItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* overlayItem = links[originItem];
    QPoint pt2 = dynamic_cast<QWidget*>(originItem->parent())->mapToGlobal(pt);
    overlayItem->move(dynamic_cast<QWidget*>(overlayItem->parent())->mapFromGlobal(pt2));
}

void COverlay::itemResized(const QSize& siz)
{
    CBoardItem* originItem = dynamic_cast<CBoardItem*>(sender());
    CBoardItem* overlayItem = links[originItem];
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