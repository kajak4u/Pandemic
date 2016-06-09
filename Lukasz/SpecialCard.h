#pragma once
#include "PlayerCard.h"
#include "City.h"
#include "Player.h"
#include "enumShort.h"

//enum SpecialCardType
//{
//	SC_ONE_QUIET_NIGHT,
//	SC_FORECAST,
//	SC_RESILIENT_POPULATION,
//	SC_AIRLIFT,
//	SC_GOVERNMENT_GRANT
//};

class SpecialCard :
	public PlayerCard
{
	/*string actionName;*/
	SpecialCardType actionType;
public:
	void QuietNight();
	void Forecast();
	/*void use();
	void use(Player* playerToMove, City* target);
	void use(City* cityForStation);*/


	SpecialCard(SpecialCardType type);
	~SpecialCard();
};	   