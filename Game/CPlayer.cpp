#include "CPlayer.h"
#include "CCity.hpp"
#include "CPawn.hpp"
#include "Lukasz\Player.h"


CPlayer::CPlayer(Player* logic, CCity *startLocation)
{
    logicObj = logic;
    this->role = logic->GetRole();
    pawn = CPawn::createIn(startLocation, role);
    this->nick = QString::fromStdString(logic->GetNick());
    }

CPlayer::~CPlayer()
{
}

PlayerRole CPlayer::getRole() const
{
    return role;
}
QLabel * CPlayer::getIco()
{
    return playerIco;
}
void CPlayer::setIco(QLabel* ico)
{
    this->playerIco = ico;
    ico->show();
    ico->setPixmap(QPixmap(QString(":/roles/img/%1.png").arg(PlayerRole_SL[role].toLower())).scaledToWidth(ico->width(), Qt::SmoothTransformation));
}

Player* CPlayer::toLogic()
{
    return logicObj;
}

CCity * CPlayer::getLocation() const
{
    return pawn->getPosition();
}