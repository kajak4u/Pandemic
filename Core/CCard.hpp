#pragma once
#include "CBoardItem.hpp"
#include "enums.h"
#include <memory>
#include <QAbstractAnimation>

class QSequentialAnimationGroup;
class Card;

class CCard : public CBoardItem {
	Q_OBJECT

public:
	CCard(QWidget * parent = (QWidget*)defaultContainer);
	~CCard();
    void setCardName(const QString& name);
    void setType(CardType newType);
    void setReversed(bool);
    bool isReversed() const;
    QString getCityName() const;
    virtual QString createObjectName() const;
    static QString createObjectName(const QString& cardName, CardType type);
    virtual void scale(double factor);
    void bindLogic(Card*);
    Card* toLogic() const;
    CardType getType() const;
    virtual void cloneTo(CBoardItem*) const;
private:
    static std::unique_ptr<QPixmap> playerReverse,
        diseaseReverse,
        epidemicObverse; //epidemicReverse==playerReverse
    QString cardName;
    CardType type;
    Card* logicObj;
    void loadStaticGraphics();
    void updateOptions();
    const QPixmap& getReverse() const;
    void loadFrom(QTextStream & ts);
    void saveTo(QTextStream & ts) const;
    QPixmap pxm;
    bool reversed;
    
public slots:
    void invert();
    void gotoPlayer();
    void restoreParent();
private slots:
    void invertStateChanged(QAbstractAnimation::State); // wywoływane przez animację z ::invert() - bo lambda może rzucać nullpointerexceptionem

};
