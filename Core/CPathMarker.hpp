#pragma once

#include "CBoardItem.hpp"
#include "CPathItem.hpp"
class CPathItem;

class CPathMarker : public CBoardItem
{
    Q_OBJECT

public:
    CPathMarker(QWidget * parent = (QWidget*)defaultContainer);
    ~CPathMarker();
    virtual void loadFrom(QTextStream & ts);
    virtual void saveTo(QTextStream & ts) const;
    CPathItem *getPosition() const;
    int getValue() const;
    bool isAtEnd() const;
    void moveTo(CPathItem* dest);
    void moveToNext(int count=1);
    void onRightBtnUp(QMouseEvent*);
protected:
    void updateOptions();
private:
    CPathItem *position;
    QString imgSrc;
    QString oldImgSrc;
};
