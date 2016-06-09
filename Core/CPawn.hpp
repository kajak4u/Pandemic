#pragma once

#include "CBoardItem.hpp"
#include "enums.h"
class CCity;
class QMouseEvent;

class CPawn : public CBoardItem
{
    Q_OBJECT

public:
    CPawn(QWidget *parent=(QWidget*)defaultContainer);
    ~CPawn();
    static CPawn* createIn(CCity* location, PlayerRole role);
    virtual void loadFrom(QTextStream &);
    virtual void saveTo(QTextStream &) const;
    void moveTo(CCity* dest);
    void onRightBtnUp(QMouseEvent*);
    PlayerRole getRole() const;
    virtual QString createObjectName() const;
    static QString createObjectName(PlayerRole role);
protected:
    CCity *position;
    PlayerRole role;
    void updateOptions();
    QString getPath(PlayerRole) const;
};
