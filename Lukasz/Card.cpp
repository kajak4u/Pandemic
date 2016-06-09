#include "Card.h"

string Card::GetName() const
{
	return cityName;
}

DiseaseType Card::GetColor() const
{
	return cityColor;
}

Card::Card(string cName, DiseaseType cColor) :cityName(cName), cityColor(cColor)
{
}


Card::Card()
{
}


Card::~Card()
{
}
