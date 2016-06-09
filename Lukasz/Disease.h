#pragma once

#include <string>
#include "City.h"
#include "enumShort.h"

using namespace std;

class Disease
{
	DiseaseType diseaseID;
	int markersLeft;
	CureStatus status;
public:
	void Discover();
	void Eradicate();

	void RemoveMarkers(int count); //z planszy
	void PutMarker(); //na plansze
	int MarkersLeft() const;
	QString getColor() const;
	DiseaseType getID() const;
	CureStatus Status() const;
	
	void addSingleCube(City* city);
	void diseaseAppearsIn(City * source, int count);
	void healDisease(City * location, PlayerRole role);
	//bool Infect(City* city, bool spreadFlag);
	//void Spread(City* basicCity);

	Disease(QString color);
	~Disease();
};

