#pragma once
#include <QVector>
#include "enums.h"

class CPawn;
class CCard;
class CCity;
class QLabel;
class Player;

class CPlayer
{
    CPawn* pawn;
    PlayerRole role;
    QLabel* playerIco;
    QString nick;
    Player* logicObj;
public:
    CPlayer(Player*, PlayerRole, const QString&, CCity*);
    ~CPlayer();
    Player* toLogic();
    QString getColor();
    CCity* getLocation() const;
    PlayerRole getRole() const;
    void setIco(QLabel*);
    QLabel* getIco() const;
};

