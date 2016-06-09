#pragma once
#include "Card.h"

class PlayerCard :
	public Card
{
public:
	//void virtual use();

	PlayerCard(string cName, DiseaseType cColor);
	PlayerCard(); //dla kart specjalnych i epidemii -> klas potomnych
	~PlayerCard();
};

