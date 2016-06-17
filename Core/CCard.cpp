#ifdef _DEBUG
//#define _NOCARDS 
#endif

#include "CCard.hpp"
#include "CStringOption.h"
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QMouseEvent>
#include "CBoard.hpp"
#include <QDebug>
#include "CPlayer.h"
#include "core.h"

std::unique_ptr<QPixmap> CCard::playerReverse = nullptr,
        CCard::diseaseReverse = nullptr,
        CCard::epidemicObverse = nullptr;

CCard::CCard(QWidget * parent) : CBoardItem(parent), reversed(false), nextReversed(false), type(CT_UNKNOWN)
{
    if (playerReverse == nullptr)
        loadStaticGraphics();
    options += new CStringOption("City name", cardName);
    disableResize();
    if (isDesigner()) {
        connect(this, &CCard::rightButtonUp, this, &CCard::invert);
    }
    else {
        connect(this, &CCard::leftButtonUp, container, &CBoard::seeDeck);
    }
}

CCard::~CCard()
{
	
}

void CCard::setCardName(const QString & name)
{
    cardName = name;
}

void CCard::setType(CardType newType)
{
    type = newType;
}

void CCard::setReversed(bool rev)
{
    reversed = rev;
    nextReversed = rev;
}

bool CCard::isReversed() const
{
    return nextReversed;
}

QString CCard::getCityName() const
{
    return cardName;
}

QString CCard::createObjectName() const
{
    return QString("CCard %1 %2").arg(cardName).arg(CardType_SL[type]);
}

QString CCard::createObjectName(const QString & cardName, CardType type)
{
    return QString("CCard %1 %2").arg(cardName).arg(CardType_SL[type]);
}

void CCard::scale(double factor)
{
    CBoardItem::scale(factor);
}

void CCard::bindLogic(Card *card)
{
    logicObj = card;
}

Card * CCard::toLogic() const
{
    return logicObj;
}

CardType CCard::getType() const
{
    return type;
}

void CCard::cloneTo(CBoardItem *target) const
{
    CBoardItem::cloneTo(target);
    if(reversed)
        target->setImage(pxm);
}

void CCard::loadStaticGraphics()
{
    playerReverse = std::unique_ptr<QPixmap>(new QPixmap(":/img/cards/Player/reverse.png"));
    diseaseReverse = std::unique_ptr<QPixmap>(new QPixmap(":/img/cards/Disease/reverse.png"));
    epidemicObverse = std::unique_ptr<QPixmap>(new QPixmap(":/img/cards/Epidemic/epidemic.png"));
}
void CCard::updateOptions()
{
    QString typeStr = CardType_SL[type];
#ifndef _NOCARDS
    if (type == CT_EPIDEMIC) {
        pxm = *epidemicObverse;
        imgsrc = ":/img/cards/Epidemic/epidemic.png";
    }
    else {
        imgsrc = ":/img/cards/" + typeStr + "/" + cardName + ".png";
        pxm = QPixmap(imgsrc);
    }
#endif
    setText(QString("<h1>%1</h1><h2>%2</h2>").arg(cardName).arg(typeStr));
    if(!pxm.isNull())
        pxm = pxm.scaledToWidth(standardSize.width(), Qt::SmoothTransformation);
    if (reversed)
        setImage(getReverse().scaledToWidth(standardSize.width(), Qt::SmoothTransformation));
    else
        setImage(pxm);
    setMask(pxm.createMaskFromColor(Qt::transparent));
    update();
    repaint();
}

QString CCard::getSrc() const
{
    return imgsrc;
}

const QPixmap & CCard::getReverse() const
{
    switch (type)
    {
    case CT_DISEASE:
        return *diseaseReverse;
    case CT_PLAYER:
    case CT_SPECIAL:
    case CT_EPIDEMIC:
        return *playerReverse;
    default:
        return *epidemicObverse;
    }
}


void CCard::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    cardName = ts.readLine();
    reversed = ts.readLine().toInt();
    nextReversed = reversed;
    updateOptions();
}

void CCard::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << cardName << endl;
    ts << QString::number((int)reversed) << endl;
}

void CCard::restoreParent()
{
    auto children = container->children();
    QWidget* newParent = dynamic_cast<QWidget*>(children[0]);
    QWidget* par = dynamic_cast<QWidget*>(parent());
    QPoint newPos = newParent->mapFrom(par, mapTo(par, CPoint(size()) / 2));
    setParent(newParent);
    show();
    setStandardMiddle(newPos / zoomFactor);
}

void CCard::invert()
{
    if (nextReversed == !reversed) {
        nextReversed = reversed;
        qDebug() << this << " will be inverted to " << nextReversed << " with no animation";
        return;
    }
    nextReversed = !reversed;
    qDebug() << this << " will be inverted to " << nextReversed;
    QSequentialAnimationGroup* animation = new QSequentialAnimationGroup(this);
    QPropertyAnimation *fadeOut = createPropertyAnimation(this, "angleY", 0, 90, 250, QEasingCurve::InSine);
    connect(fadeOut, &QPropertyAnimation::stateChanged, this, &CCard::invertStateChanged);
    animation->addAnimation(fadeOut);
    QPropertyAnimation *fadeIn = createPropertyAnimation(this, "angleY", 90, 0, 250, QEasingCurve::OutSine);
    animation->addAnimation(fadeIn);
    container->addAnimation(animation);
}
void CCard::invertStateChanged(QAbstractAnimation::State newState)
{
    if (newState == QAbstractAnimation::Running)
        raise();
    if (newState == QAbstractAnimation::Stopped) {
        reversed = nextReversed;
        qDebug() << this << " is reversed to" << reversed;
        if (reversed)
            setImage(getReverse().scaledToWidth(standardSize.width(), Qt::SmoothTransformation));
        else if (pxm.isNull()) {
            setImage(QPixmap());
            setText(QString("<h1>%1</h1><h2>%2</h2>").arg(cardName).arg(CardType_SL[type]));
        } 
        else
            setImage(pxm);
    }
}