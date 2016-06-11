#pragma once
#include "CBoardItem.hpp"
#include <memory>

class CPathItem : public CBoardItem {
	Q_OBJECT
    Q_PROPERTY(bool highlighted MEMBER highlighted)
public:
	CPathItem(QWidget * parent = (QWidget*)defaultContainer);
	virtual ~CPathItem();
    virtual void loadFrom(QTextStream & ts);
    virtual void saveTo(QTextStream & ts) const;
    void onLeftBtnUp(QMouseEvent * event);
    void onRightBtnUp(QMouseEvent * event);
    void select();
    void unselect();
    CPathItem* getNext() const;
    void setNext(CPathItem* n);
private:
    CPathItem *next;
    bool highlighted;
    int value;
    static std::unique_ptr<QBitmap> pathItemMask;
    void changeNext(QMouseEvent * event);
};