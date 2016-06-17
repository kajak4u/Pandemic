#include "CPlayer.h"
#include "CCity.hpp"
#include "CPawn.hpp"


CPlayer::CPlayer(Player* logic, PlayerRole role, const QString& nick, CCity *startLocation)
{
    logicObj = logic;
    this->role = role;
    pawn = CPawn::createIn(startLocation, role);
    this->nick = nick;
    }

CPlayer::~CPlayer()
{
}

PlayerRole CPlayer::getRole() const
{
    return role;
}
QLabel * CPlayer::getIco() const
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

QString CPlayer::getColor()
{
    switch (role)
    {
    case ROLE_DISPATCHER:
        return "rgba(199,  52, 146, 192)";
        break;
    case ROLE_MEDIC:
        return "rgba(218, 136,  18, 192)";
        break;
    case ROLE_OPERATIONSEXPERT:
        return "rgba(158, 199,  59, 192)";
        break;
    case ROLE_RESEARCHER:
        return "rgba(192, 130,  85, 192)";
        break;
    case ROLE_SCIENTIST:
        return "rgba(229, 224, 220, 192)";
        break;
    default:
        return "transparent";
    }
}

CCity * CPlayer::getLocation() const
{
    return pawn->getPosition();
}