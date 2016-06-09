#pragma once
#include "Card.h"
#include "enumShort.h"
class DiseaseCard :
	public Card
{
public:
	DiseaseCard(string cityName, DiseaseType cityColor);
	~DiseaseCard();
};

