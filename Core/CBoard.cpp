﻿#include "CBoard.hpp"
#include <QApplication>
#include <QResizeEvent>
#include <QMainWindow>
#include <QFile>
#include "CDeck.hpp"
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
#include "CCard.hpp"
#include "CPlayer.h"
#include "CHandCard.hpp"

CBoard::CBoard(QWidget * parent)
    : CExtendedSignalWidget(parent), zoomFactor(1.0), actualInserted(CLASS_City), activeMenu(nullptr)
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

void CBoard::setPlayerWidgets(QLabel* arr[4], QLayout* playerArea)
{
    this->playerArea = playerArea;
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

double CBoard::getZoom() const
{
    return zoomFactor;
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


void CBoard::setCardsEnabled(CardType type, bool enable)
{
    for (int i = 0; i < hand.size(); ++i)
        if (hand[i]->getType() == type)
            hand[i]->setEnabled(enable);
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

void CBoard::seeDeck()
{
    CCard* snd = dynamic_cast<CCard*>(sender());
    if (snd == nullptr)
        return;
    QSet<DeckType> typesToSee = {
        DT_DISEASEDISCARD,
        DT_PLAYERDISCARD
    };
    for (DeckType type: typesToSee) {
        CDeck* deck = findChild<CDeck*>(CDeck::createObjectName(type));
        if (snd->geometry().intersects(deck->geometry())) {
            emit showDeck(type);
            break;
        }
    }
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

void CBoard::clickCity(QMouseEvent* event)
{
    if (!isDragging) {
        event->accept();
        CCity* city = dynamic_cast<CCity*>(sender());
        emit cityClicked(city);
    }
}

void CBoard::setMode(Qt::TransformationMode newMode)
{
    scaleMode = newMode;
    QAbstractAnimation::State st;
    if (newMode == Qt::FastTransformation)
        st = QAbstractAnimation::Running;
    else
        st = QAbstractAnimation::Stopped;
    for (CBoardItem* item : items)
        item->scaleAnimationChanged(st);
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
    closeMenuConn = connect(this, &CBoard::leftButtonUp, this, &CBoard::closeCityMenu);
}
void CBoard::closeCityMenu() {
    disconnect(closeMenuConn);
    cityMenu->hide();
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

//chciałem to zrobić ładniej, ale się nie da... bo Qt robi jaja jak dostanie dwie animacje tej samej cechy tego samego obiektu...
void CBoard::addAnimation(QAbstractAnimation *newAnim)
{
    static int maxduration = 0;
    static int animCount = 0;
    static int lastStart = 0;
    static QAbstractAnimation* lastAnim = nullptr;
    int pauseDuration = animCount==0 ? 0 : qMax(lastStart - lastAnim->currentTime(), 0);
    QSequentialAnimationGroup* newGroup = new QSequentialAnimationGroup(this);
    newGroup->addPause(pauseDuration);
    newGroup->addAnimation(newAnim);
    lastStart = pauseDuration + 256;
    lastAnim = newGroup;
    ++animCount;
    //connect(newGroup, &QAbstractAnimation::stateChanged, [this](QAbstractAnimation::State state) {
    //    if (state == QAbstractAnimation::Stopped) {
    //        --animCount;
    //        qDebug() << "anim finished, left " << animCount;
    //        if (animCount == 0)
    //            emit animationFinished();
    //    }
    //});
    connect(newGroup, &QAbstractAnimation::destroyed, [this]() {
        --animCount;
        qDebug() << "anim destroyed, left " << animCount;
        if (animCount == 0)
            emit animationFinished();
    });
    newGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void CBoard::addPlayer(CPlayer* newPlayer)
{
    newPlayer->setIco(playerLabels[players.size()]);
    newPlayer->getIco()->setObjectName(QString("playerLabel %1").arg(players.size()));
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
        city->unselect();
        city->update();
        disconnect(city, &CCity::leftButtonUp, this, &CBoard::clickCity);
    }
    activeCities = newActive;
    for (CCity* city : newActive) {
        city->select();
        city->update();
        connect(city, &CCity::leftButtonUp, this, &CBoard::clickCity, Qt::QueuedConnection);
    }
}

bool CBoard::isCurrentCity(CCity *city) const
{
    return players[0]->getLocation() == city;
}

void CBoard::addCardToHand(CCard *card)
{
    qDebug() << "CBoard::addNewCard " << card << "to hand of " << PlayerRole_SL[currentPlayer()->getRole()];
    CHandCard* item = new CHandCard(card);
    hand.push_back(item);
    dynamic_cast<QVBoxLayout*>(playerArea)->insertWidget(hand.size(), item);
    connect(item, &CHandCard::cardActivated, this, &CBoard::useCard);
    item->show();
}

void CBoard::removeCardFromHand(CCard *card)
{
    QString desiredName = QString("CardInHand_%1").arg(card->getCityName());
    for (int i = 0; i < hand.size(); ++i) {
        if (hand[i]->objectName() == desiredName) {
            hand[i]->deleteLater();
            hand.erase(&hand[i]);
            return;
        }
    }

}

void CBoard::clearHand()
{
    qDebug() << "CBoard::clearHand of " << PlayerRole_SL[currentPlayer()->getRole()];;
    while (!hand.isEmpty()) { // itemAt(0) == spacer
        qDebug() << "removed " << hand.back();
        delete hand.back();
        hand.pop_back();
    }
}

void CBoard::nextPlayer()
{
    players[0]->getIco()->setStyleSheet("");
    QVector<CPoint> positions;
    for (const CPlayer* player : players)
        positions += player->getIco()->pos();
    players[0]->getIco()->stackUnder(players.back()->getIco());
    for (int i = 0; i < players.size()-1; ++i)
        std::swap(players[i + 1], players[i]);
    int i = 0;
    for (CPlayer* player : players)
        addAnimation(createPropertyAnimation(player->getIco(), "pos", player->getIco()->pos(), positions[i++], 1000, QEasingCurve::InOutCirc));
    QSize size = players[0]->getIco()->size();
    QString qss = QString("border: 2px outset #0F0; width: %1px; height: %2px").arg(size.width() + 4).arg(size.height() + 4);
    players[0]->getIco()->setStyleSheet(QString("border: 2px outset #0F0; width: %1px; height: %2px").arg(size.width()+4).arg(size.height()+4));
}

double CBoard::minZoomFactor() const
{
    return qMin(double(width()) / image.width(), double(height()) / image.height());
}
