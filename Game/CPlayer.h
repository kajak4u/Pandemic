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
    CPlayer(Player*, CCity*);
    ~CPlayer();
    Player* toLogic();
    CCity* getLocation() const;
    PlayerRole getRole() const;
    void setIco(QLabel*);
    QLabel* getIco() const;
};

