#include "CDeck.hpp"
#include "CCard.hpp"
#include "CBoard.hpp"
#include "CBoardItemsFactory.h"

CDeck::CDeck(QWidget * parent) : CBoardItem(parent), hasReversedCards(false) {
	
}

CDeck::~CDeck() {
	
}


void CDeck::loadFrom(QTextStream &ts)
{
    CBoardItem::loadFrom(ts);
    type = (DeckType)DeckType_SL.indexOf(ts.readLine());
}

void CDeck::saveTo(QTextStream &ts) const
{
    CBoardItem::saveTo(ts);
    ts << DeckType_SL[type] << endl;
}


void CDeck::setType(DeckType newType)
{
    type = newType;
}

void CDeck::setReversed(bool reversed)
{
    hasReversedCards = reversed;
}

bool CDeck::isReversed() const
{
    return hasReversedCards;
}

QString CDeck::createObjectName() const
{
    return QString("CDeck %1").arg(DeckType_SL[type]);
}

QString CDeck::createObjectName(const QString & name)
{
    return QString("CDeck %1").arg(name);
}

QString CDeck::createObjectName(DeckType dt)
{
    return QString("CDeck %1").arg(DeckType_SL[dt]);
}

void CDeck::addNewCard(CardType type, const QString& name, Card* logicObj)
{
    CCard* card = dynamic_cast<CCard*>(CBoardItemsFactory().create("CCard", zoomFactor, getStandardMiddle()));
    if (card == nullptr)
        throw QString()+"Error: failed to create card "+name;
    card->setStandardSize(standardSize);
    card->setStandardMiddle(getStandardMiddle());
    card->setType(type);
    card->setCardName(name);
    card->setReversed(hasReversedCards);
    card->updateOptions();
    card->rename();
    card->bindLogic(logicObj);
    container->addItem(card);
}