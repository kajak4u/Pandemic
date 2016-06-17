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
    void moveToNext(int count=1);
    void onRightBtnUp(QMouseEvent*);
protected:
    void updateOptions();
private:
    void moveTo(CPathItem* dest);
    CPathItem *position;
    QString imgSrc;
    QString oldImgSrc;
};
