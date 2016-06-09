#include "SpecialCard.h"

//void SpecialCard::use()
//{
//	switch (actionType)
//	{
//	case SC_ONE_QUIET_NIGHT:
//		//goto  - jakies lepsze pomysly?
//		break;
//	case SC_FORECAST:
//
//		break;
//	case SC_RESILIENT_POPULATION:
//		break;
//	case SC_AIRLIFT:
//		break;
//	case SC_GOVERNMENT_GRANT:
//		break;
//	default:
//		break;
//	}
//}

//void SpecialCard::use(Player* playerToMove, City* target)
//{
//	playerToMove
//}
//
//void SpecialCard::use(City* cityForStation)
//{
//	cityForStation->BuildResearchStation();
//}

SpecialCard::SpecialCard(SpecialCardType type) 
	: PlayerCard(SpecialCardType_SL[type].toStdString(), UNKNOWN)//nieznany kolor jako wyznacznik specjalnej karty
	, actionType(type)
{
}		 

SpecialCard::~SpecialCard()
{
}
