#pragma once
#include "PlayerCard.h"
//#include "CardDeck.h"
class EpidemicCard :
	public PlayerCard
{
	//const int orderNumber;
public:
	//void Epidemy(CardDeck& newCards, CardDeck& oldCards);

	EpidemicCard(int number);
	~EpidemicCard();
};

