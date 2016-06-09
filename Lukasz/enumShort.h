#pragma once
#include "enums.h"
#ifdef UNDEFINED
#include <string>
using namespace std;
#include <QStringList>

enum DiseaseType {
    UNKNOWN = 0,
    BLUE = 1,
    YELLOW = 2,
    BLACK = 3,
    RED = 4
};
const QStringList DiseaseType_SL = { "<NULL>", "BLUE", "YELLOW", "BLACK", "RED" };

enum CureStatus {
    NEW = 0,
    DISCOVERED = 1,
    TREATED = 2
};
const QStringList CureStatus_SL = {"New", "Discovered", "Treated"};

enum PlayerRole {
    ROLE_RANDOM = 0,
    ROLE_DISPATCHER,
    ROLE_MEDIC,
    ROLE_OPERATIONSEXPERT,
    ROLE_RESEARCHER,
    ROLE_SCIENTIST
};
static const QStringList PlayerRole_SL = { "[Random]", "Dispatcher", "Medic", "Operations Expert", "Researcher", "Scientist" };

enum Difficulty {
    DIFF_INTRODUCTORY = 0,
    DIFF_MEDIUM,
    DIFF_HEROIC
};
static const QStringList Difficulty_SL = {"Introductory", "Medium", "Heroic"};

enum Decision
{
	DEC_MOVE_TO_PLAYER = -2,		//j.n.
	DEC_MOVE_ANOTHER = -1,			//dispatcher
	DEC_MOVE_SHORT = 0,
	DEC_MOVE_TO_CARD = 1,				
	DEC_MOVE_EVERYWHERE,
	//DEC_MOVE_TO_STATION,
	DEC_BUILD_STATION,
	DEC_DISCOVER_CURE,
	DEC_TREAT,
	DEC_GAIN_CARD,
	DEC_GIVE_CARD,
	DEC_USE_SPECIAL,
	DEC_PASS
};

enum SpecialCardType //wartosci dla pewnosci w petli dodawania
{
	SC_ONE_QUIET_NIGHT = 0,
	SC_FORECAST = 1,
	SC_RESILIENT_POPULATION = 2,
	SC_AIRLIFT = 3,
	SC_GOVERNMENT_GRANT = 4
};			   
static const QStringList SpecialCardType_SL = {"One Quiet Night", "Forecast", "Resilient Population","Airlift","Government Grant"};

enum GameResult
{
	FAILED = -1,
	IN_PROGRESS = 0,
	WON = 1
};	
#endif			