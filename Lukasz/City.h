#pragma once
#include <vector>
#include <string>
#include "enumShort.h"
#include <map>
//#include "Disease.h"
//#include <qset>
//#include <qmap>

#include <iostream>
using namespace std;
class City
{
	//static vector<City*> visitedInSpread;
	
	string name;
	DiseaseType type;				//kolor miasta
	bool hasResearchStation;
	vector<City*> neighbourhood;
	map<DiseaseType, int> diseaseCubes;
public:
	//static void ClearVisitedList();

	int DiseaseCounter(DiseaseType i);	  //getter
	string GetName() const;
	DiseaseType GetColor() const;
	bool IsStation() const;
	vector<City*> Neighbours() const;
	void BuildResearchStation();
	void RemoveResearchStation();

	static vector<City*> AddNeighbours(const map<City*, vector<string>>& citiesWithNeighbourhood);
																			   
	//void Spread(Disease* disease); //sprawdzic wyzej - czy przy wywolaniu nie jest uz wyleczona choroba...
	void AddCube(DiseaseType diseaseID);
	void RemoveCube(DiseaseType diseaseID);
	void RemoveAllCubes(DiseaseType diseaseID);
	//void HealOne(Disease* disease);
	//void HealAll(Disease* disease);

	City();
	City(string cityName, DiseaseType color);
	~City();

	/*int cubesOf(DiseaseType) const;
	void increaseCubes(DiseaseType);
	void decreaseCubes(DiseaseType);
	void eraseCubes(DiseaseType);*/
};