#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
class CCard;

class CDeck : public CBoardItem {
	Q_OBJECT
    bool hasReversedCards;
    DeckType type;
public:
	CDeck(QWidget * parent = (QWidget*)defaultContainer);
    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;
    void addCard(CardType type, const QString& name);
    void revealCard();
    void revealBottomCard();
	virtual ~CDeck();
    void setType(DeckType);
    void setReversed(bool reversed);
    virtual QString createObjectName() const;
    static QString createObjectName(const QString& name);
    static QString createObjectName(DeckType dt);
private:
	
};
