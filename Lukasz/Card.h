#pragma once
#include "enumShort.h"

#include <string>
using namespace std;

class Card
{
protected:
	string cityName;
	DiseaseType cityColor;
public:
	string GetName() const;
	DiseaseType GetColor() const;
	Card(string cName, DiseaseType cColor);
	Card();
	virtual ~Card();
};

