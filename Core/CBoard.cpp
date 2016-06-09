﻿#include "CBoard.hpp"
#include <QApplication>
#include <QResizeEvent>
#include <QMainWindow>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPAuseAnimation>
#include <QDebug>
#include "CCity.hpp"
#include "CPoint.h"
#include "CBoardItemsFactory.h"
#include "CDiseaseCube.hpp"
#include <QDesktopWidget>
#include "CPathItem.hpp"
#include "CPawn.hpp"
#include "core.h"
#include "COverlay.hpp"
#include "CCircleMenu.hpp"
#include "CGameWindow.hpp"
#include "Lukasz/Board.h"
#include "CCard.hpp"
#include "CPlayer.h"

CBoard::CBoard(QWidget * parent)
    : CExtendedSignalWidget(parent), zoomFactor(1.0), actualInserted(CLASS_City), activeMenu(nullptr), animation(nullptr)
{
    container = new CExtendedSignalWidget(this);
    container->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    image = QPixmap("img/board.jpg");
    smallImage = QPixmap("img/board_small.jpg");
    container->setPixmap(image);
    QSize pixmapSize = container->pixmap()->size();
    container->resize(pixmapSize);
    CBoardItem::setDefaultContainer(this);
    connect(this, &CBoard::leftButtonDown, this, &CBoard::startDragging);
    connect(this, &CBoard::mouseMove, this, &CBoard::onMouseMove);
    if (isDesigner()) {
        connect(this, &CBoard::leftButtonUp, this, &CBoard::addItemFromClick);
        connect(this, &CBoard::rightButtonUp, this, &CBoard::changeInsertableClass);
    }
    connect(this, &CBoard::created, this, &CBoard::afterCreate, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(container, &CExtendedSignalWidget::moved, [this]() {
        for (CBoardItem* item : items)
            emit item->moved(item->pos());
    });
    animation = new QParallelAnimationGroup(this);
    emit created();
}
void CBoard::afterCreate()
{
    double minFactor = minZoomFactor();
    zoomTo(minFactor);
    CPoint offset = size() - image.size()*minFactor;
    container->move(offset / 2);
}

CBoard::~CBoard() {
    delete container;
    container = nullptr;
}
void CBoard::wheelEvent(QWheelEvent* event)
{
    CExtendedSignalWidget::wheelEvent(event);
    Qt::KeyboardModifiers keys = event->modifiers();
    CPoint mousePos = event->pos();
    CPoint pt = event->angleDelta();
    if (keys & Qt::ControlModifier) {
        if (pt.y() > 0)
            zoom(1.25, mousePos);
        else
            zoom(0.8, mousePos);
        return;
    }
    else if (keys & Qt::ShiftModifier) {
        pt = CPoint(pt.y(), pt.x());
    }
    move(pt);
}

void CBoard::setPlayerIconLabels(QLabel* arr[4])
{
    playerLabels = QVector<QLabel*>();
    for (int i = 0; i < 4; ++i)
        playerLabels << arr[i];
}

double CBoard::scaleContent(double factor)
{
    if (container == nullptr)
        return factor;
    QSize resolution = size();
    double oldZoomFactor = zoomFactor;
    double minFactor = minZoomFactor();
    if (zoomFactor*factor > 1.0)
        factor = 1.0 / zoomFactor;
    else if (zoomFactor*factor < minFactor)
        factor = minFactor / zoomFactor;
    zoomFactor = zoomFactor*factor;
    QSize newSize = zoomFactor*image.size();
    bool smallEnough = newSize.width() < smallImage.width() && newSize.height() < smallImage.height();
    QPixmap pixmap2 = (smallEnough ? smallImage : image).scaled(newSize, Qt::IgnoreAspectRatio, scaleMode);
    container->setPixmap(pixmap2);
    container->resize(newSize);
    for (CBoardItem *item : items)
        item->scale(factor);
    if (activeMenu != nullptr)
        activeMenu->move(activeMenu->pos()*factor - CPoint(activeMenu->size() / 2)*(1 - factor));
    return factor;
}

void CBoard::zoom(double factor, CPoint refPoint) {
    if (container == nullptr) //zabezpieczenie; jeśli rzucono wyjątek to może próbować takie jaja
        return;
    factor = scaleContent(factor);
    CPoint containerOffset = (1 - factor)*(refPoint - container->pos());
    container->move(container->pos() + containerOffset);
}

void CBoard::zoomTo(double newZoomFactor)
{
    scaleContent(newZoomFactor / zoomFactor);
}

void CBoard::zoomOut()
{
    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    QPropertyAnimation *scaleAnimation = new QPropertyAnimation(this, "zoomFactor", this);
    scaleAnimation->setStartValue(zoomFactor);
    setMode(Qt::FastTransformation);
    double minZoomFactor = this->minZoomFactor();
    scaleAnimation->setEndValue(minZoomFactor);
    scaleAnimation->setDuration(2000);
    scaleAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    group->addAnimation(scaleAnimation);
    connect(scaleAnimation, &QPropertyAnimation::finished, this, &CBoard::setSmoothMode);
    QPropertyAnimation *offsetAnimation = new QPropertyAnimation(container, "pos", container);
    offsetAnimation->setStartValue(container->pos());
    CPoint offset = size() - image.size()*minZoomFactor;
    offsetAnimation->setEndValue(offset / 2);
    offsetAnimation->setDuration(2000);
    offsetAnimation->setEasingCurve(QEasingCurve::InOutCubic);
    group->addAnimation(offsetAnimation);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void CBoard::setPos(CPoint pos)
{
    QRect geom = container->geometry();
    CPoint delta = zoomFactor*pos - container->pos();
    scroll(delta.x(), delta.y());
}

void CBoard::move(CPoint offset)
{
    CPoint newPos = container->pos()+offset*zoomFactor;
    QMargins margin = { 200, 100, 200, 100 };
    QRect allowedArea = QRect(QPoint(0, 0), size() - container->size()).marginsAdded(-margin);
    newPos = newPos.limited(container->pos(), allowedArea);
    container->move(newPos);
}


void CBoard::onMouseMove(QMouseEvent *event)
{
    move((event->pos() - dragStart)/zoomFactor);
    qDebug() << "MOVE " << (event->pos() - dragStart) / zoomFactor;
    dragStart = event->pos();
    isDragging = true;
    event->accept();
}

void CBoard::startDragging(QMouseEvent *event)
{
    dragStart = event->pos();
    isDragging = false;
}

void CBoard::addItemFromClick(QMouseEvent *event)
{
    if (qApp->widgetAt(event->pos()) != container)
        return;
    if (isDragging)
        return;

    if (!(event->modifiers() & Qt::ControlModifier)) { //Ctrl+Leftbutton -> delete object
        QString className = Insertable_SL[(int)actualInserted];
        CBoardItem *newItem = CBoardItemsFactory().create(className, zoomFactor, event->pos());
        items.push_back(newItem);
    }
}

void CBoard::changeInsertableClass(QMouseEvent *event)
{
    bool ok;
    QString chosen = QInputDialog::getItem(nullptr, "Insertable class", "Choose insertable class", Insertable_SL, (int)actualInserted, false, &ok);
    if (ok)
        actualInserted = (Insertable)Insertable_SL.indexOf(chosen);
}

void CBoard::loadFrom(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream ts(&file);
    CBoardItemsFactory factory;
    factory.initFrom(ts)
           .setWith(this, container);
    int outbreaksId, infectionsId;
    ts >> outbreaksId >> infectionsId;
    int count = ts.readLine().toInt();
    items.resize(count);
    for (CBoardItem*& item : items)
        item = factory.create(ts);
    for (CBoardItem*& item : items)
        item->delayedLoad();
    outbreaksPath = dynamic_cast<CPathItem*>(findItemById(outbreaksId));
    infectionsPath = dynamic_cast<CPathItem*>(findItemById(infectionsId));
}
void CBoard::saveTo(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream ts(&file);
    CBoardItemsFactory().saveTo(ts);
    auto getId = [](CBoardItem* item) {return item == nullptr ? -1 : item->getId(); };
    ts << getId(outbreaksPath) << " " << getId(infectionsPath) << " "
        << items.size() << endl;
    for (CBoardItem* item : items)
        item->saveTo(ts);
}

CBoardItem * CBoard::findItemByName(const QString &name)
{
    for (CBoardItem *item : items)
        if (item && item->getName() == name)
            return item;
    return nullptr;
}

CBoardItem * CBoard::findItemById(int id)
{
    for (CBoardItem *item : items)
        if (item && item->getId() == id)
            return item;
    return nullptr;
}

CPawn * CBoard::findPawn(PlayerRole role)
{
    for (CBoardItem* item : items) {
        CPawn* pawn = dynamic_cast<CPawn*>(item);
        if (pawn != nullptr && pawn->getRole() == role)
            return pawn;
    }
    return nullptr;
}

void CBoard::addItem(CBoardItem *item)
{
    items.push_back(item);
}

void CBoard::removeItem(CBoardItem *item)
{
    items.removeOne(item);
    item->unload();
    item->deleteLater();
}

void CBoard::firstPathItem(CPathItem * item)
{
    bool ok;
    PathType current = (outbreaksPath == item) ? PATH_OUTBREAKS : (infectionsPath == item) ? PATH_INFECTIONS : PATH_NONE;
    QString typed = QInputDialog::getItem(this, "Path type", "Choose, which path starts from this item:", PathType_SL, current, false, &ok);
    if (!ok)
        return;
    PathType chosen = (PathType)PathType_SL.indexOf(typed);
    switch (chosen) {
        case PATH_NONE:
            switch (current) {
                case PATH_NONE:
                    break;
                case PATH_OUTBREAKS:
                    outbreaksPath = nullptr;
                    break;
                case PATH_INFECTIONS:
                    infectionsPath = nullptr;
            }
            break;
        case PATH_OUTBREAKS:
            outbreaksPath = item;
            break;
        case PATH_INFECTIONS:
            infectionsPath = item;
            break;
    }
}

void CBoard::setMode(Qt::TransformationMode newMode)
{
    scaleMode = newMode;
    zoom(1.0);
}

void CBoard::setSmoothMode()
{
    setMode(Qt::SmoothTransformation);
}

void CBoard::showCityMenu(const CPoint & pos)
{
    cityMenu->show(pos);
    activeMenu = cityMenu;
}

void CBoard::setCityMenu(CCircleMenu *menu)
{
    cityMenu = menu;
    menu->setParent(container);
}

COverlay * CBoard::showOverlay()
{
    COverlay* overlay = new COverlay(this);
    overlay->show();
    overlay->raise();
    return overlay;
}

void CBoard::addAnimation(QAbstractAnimation *newAnim)
{
    int lastStart = 1000;
    if (animation->animationCount() != 0) {
        QSequentialAnimationGroup* lastGroup = dynamic_cast<QSequentialAnimationGroup*>(animation->animationAt(animation->animationCount() - 1));
        QPauseAnimation* pause = dynamic_cast<QPauseAnimation*>(lastGroup->animationAt(0));
        lastStart = pause->duration() - animation->currentTime();
    }
    QSequentialAnimationGroup* newGroup = new QSequentialAnimationGroup(this);
    newGroup->addPause(lastStart + 500);
    newGroup->addAnimation(newAnim);
    if (animation->state() == QAbstractAnimation::Stopped) {
        animation->clear();
        animation->addAnimation(newGroup);
        animation->start();
    }
    else
        animation->addAnimation(newGroup);
}

void CBoard::addPlayer(CPlayer* newPlayer)
{
    newPlayer->setIco(playerLabels[players.size()]);
    players.push_back(newPlayer);
}

CPlayer * CBoard::findPlayer(PlayerRole role)
{
    for (CPlayer* player : players)
        if (player->getRole() == role)
            return player;
    return nullptr;
}

CPlayer * CBoard::currentPlayer() const
{
    return players[0];
}

void CBoard::setActiveCities(const QSet<CCity*>&newActive)
{
    for (CCity* city : activeCities) {
        city->setProperty("selected", false);
        city->update();
    }
    activeCities = newActive;
    for (CCity* city : newActive) {
        city->setProperty("selected", true);
        city->update();
    }
}

double CBoard::minZoomFactor() const
{
    return qMin(double(width()) / image.width(), double(height()) / image.height());
}
